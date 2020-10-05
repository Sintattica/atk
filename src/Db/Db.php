<?php

namespace Sintattica\Atk\Db;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\TmpFile;
use Sintattica\Atk\Utils\Debugger;

/**
 * Some helper classes around PDO and initialization functions.
 *
 * Notes:
 *  - it autostarts a transaction on initialization, so don't forget to commit
 * when you need it.
 *  - query and prepare will halt() on error (unless $m_haltonerror set to
 * false), so you can expect a valid result from them.
 *
 * @author Peter Verhage <peter@achievo.org>
 * @author Ivo Jansch <ivo@achievo.org>
 * @author Samuel BF
 */
class Db extends \PDO
{
    /**
     * Field types for database values
     */
    const FT_UNSUPPORTED = 1;
    const FT_BOOLEAN = 2;
    const FT_NUMBER = 3;
    const FT_DECIMAL = 4;
    const FT_STRING = 5;
    const FT_DATE = 6;
    const FT_TIME = 7;
    const FT_DATETIME = 8;

    /**
     * Field flags
     */
    const MF_PRIMARY = 1;
    const MF_UNIQUE = 2;
    const MF_NOT_NULL = 4;
    const MF_AUTO_INCREMENT = 8;

    /*
     * Force case insensitive searching and ordering.
     * @var boolean
     */
    protected $m_force_ci = false;

    /*
     * The current connection name.
     * @access private
     * @var String
     */
    public $m_connection = '';

    /*
     * If true, an atkerror is raised when an error occurred.
     *
     * The calling script can use this to stop execution and rollback.
     * If false, the error will be ignored and script execution
     * continues. Use this only for queries that may fail but still
     * be valid.
     * @access private
     * @var boolean
     */
    public $m_haltonerror = true;

    /*
     * Driver name.
     *
     * Derived classes should add their own m_type var to the class
     * definition and put the correct name in it. (e.g. "mysql" etc.)
     * @abstract
     * @access private
     * @var String
     */
    public $m_type = '';

    /*
     * array to cache meta-information about tables.
     * @access private
     * @var array
     */
    private $m_tableMeta = [];

    /*
     * List of error codes that could be caused by an end-user.
     *
     * This type of errors is 'recoverable'. An example is a violation of a
     * unique constraint.
     * @access private
     * @var array
     */
    private $m_user_error = ['23000'];

    /**
     * sequence table. Used for AUTO_INCREMENT attributes
     * that are not defined as AUTO_INCREMENT in database,
     * or for custom calls of 'nextid()'
     *
     * @var string
     */
    private $m_seq_table = 'db_sequence';

    /**
     * the field in the seq_table that contains the counter..
     *
     * @var string
     */
    public $m_seq_field = 'nextid';

    /**
     * the field in the seq_table that countains the name of the sequence..
     *
     * @var string
     */
    public $m_seq_namefield = 'seq_name';

    /*********************************************** Getters / Setters *************************************************/
    /**
     * Get the database driver type.
     *
     * @return string driver type
     */
    public function getType()
    {
        return $this->m_type;
    }

    /**
     * Halt on error?
     *
     * @return bool halt on error?
     */
    public function getHaltOnError()
    {
        return $this->m_haltonerror;
    }

    /**
     * Halt on error or not?
     *
     * @param bool $state
     */
    public function setHaltOnError($state = true)
    {
        $this->m_haltonerror = $state;
    }

    /**
     * Force case-insensitive search ?
     *
     * @param bool $state
     */
    public function getForceCaseInsensitive()
    {
        return $this->m_force_ci;
    }

    /**
     * This function indicates what searchmodes the database supports.
     *
     * @return array with search modes
     */
    public function getSearchModes()
    {
        // exact match and substring search should be supported by any database.
        // (the LIKE function is ANSI standard SQL, and both substring and wildcard
        // searches can be implemented using LIKE)
        return array(
            'exact',
            'substring',
            'wildcard',
            'regexp',
            'greaterthan',
            'greaterthanequal',
            'lessthan',
            'lessthanequal',
            'between',
        );
    }

    /*********************************************** Class initialization *************************************************/
    /**
     * Get a database instance (singleton)
     *
     * This method instantiates and returns the correct (vendor specific)
     * database instance, depending on the configuration.
     *
     * @static
     *
     * @param string $conn The name of the connection as defined in the
     *                      config/parameters.ENV.php file (defaults to 'default')
     *
     * @return Db Instance of the database class
     */
    public static function getInstance($conn = 'default')
    {
        static $s_dbInstances = null;
        if ($s_dbInstances == null) {
            $s_dbInstances = [];
            if (!Config::getGlobal('meta_caching')) {
                Tools::atkwarning("Table metadata caching is disabled. Turn on \$config_meta_caching to improve your application's performance!");
            }
        }

        $dbInstance = $s_dbInstances[$conn]??null;

        if (!$dbInstance) {
            $dbconfig = Config::getGlobal('db');

            try {
                $dsn = self::dsnFromConfig($dbconfig[$conn]);
                $driver = explode(':', $dsn)[0];
                $driverClass = __NAMESPACE__.'\\'.ucfirst($driver).'Db';
                Tools::atkdebug("Connecting to '{$conn}' with dsn '{$dsn}' and '{$driverClass}' driver");

                /** @var Db $dbInstance */
                $dbInstance = new $driverClass(
                    $dsn,
                    $dbconfig[$conn]['user'] ?? '',
                    $dbconfig[$conn]['password'] ?? '',
                    $dbconfig[$conn]['options'] ?? []
                );
            } catch (\PDOException $e) {
                Tools::atkhalt(Tools::atktext('db_access_failed', 'atk'), 'critical');
                return null;
            }
            $dbInstance->init($conn, $driver, $dbconfig[$conn]);
            $s_dbInstances[$conn] = $dbInstance;
        }

        return $dbInstance;
    }

    /**
     * Computes DSN string from the config array as defined in config/parameters.ENV.php
     *
     * @param array $config with values from ATK config
     *
     * @return $dsn string
     */
    private static function dsnFromConfig($config)
    {
        // User can supply dsn string directly
        if (isset($config['dsn'])) {
            return $config['dsn'];
        }

        $driver = strtolower($config['driver'])??'';
        switch ($driver) {
            case 'mysql':
            case 'mysqli':
                $driver = 'mysql';
                break;
            case 'pgsql':
            case 'postgresql':
                $driver = 'pgsql';
                break;
            case 'sqlite':
                $driver = 'sqlite';
                break;
            default:
                Tools::atkhalt(sprintf(Tools::atktext('db_unsupported_driver', 'atk'), $driver));
        }

        $options = [];
        if (isset($config['host'])) {
            $options[] = "host={$config['host']}";
        }
        if (isset($config['port'])) {
            $options[] = "port={$config['port']}";
        }
        if (isset($config['db'])) {
            $options[] = "dbname={$config['db']}";
        }
        if ($driver == 'mysql') {
            $options[] = 'charset='.
                ($config['charset'] ?? 'utf8mb4');
        }
        if ($driver == 'sqlite') {
            $options = [$config['file']];
        }

        return $driver.':'.implode(';', $options);
    }

    /**
     * Intialize some default options and start the first transaction
     *
     * @param string $connectionName    from the config files
     * @param string $driver            used for the connection
     * @param string $connectionOptions from the config files
     */
    protected function init($connectionName, $driver, $connectionOptions = [])
    {
        Tools::atkdebug("Initializing '{$connectionName}' database instance");
        $this->m_type = $driver;
        $this->m_connection = $connectionName;
        $this->m_force_ci = $connectionOptions[$connectionName]['force_ci'] ?? false;
        $this->setAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_ASSOC);
        if (!$this->beginTransaction()) {
            $this->halt('Starting transaction failed');
        }
    }

    /*********************************************** Error handling *************************************************/
    /**
     * Has error?
     *
     * Relies on PDO::errorCode, which returns a SQLSTATE error code. See :
     * https://en.wikipedia.org/wiki/SQLSTATE
     *
     * @return bool
     */
    public function hasError()
    {
        $errorCategory == substr($this->errorCode(), 0, 2);
        return $errorCategory == '00' || $errorCategory == '02';
    }

    /**
     * Determine whether an error that occurred is a recoverable (user) error
     * or a system error.
     *
     * @return string "user" or "system"
     */
    public function getErrorType()
    {
        if (in_array($this->errorCode(), $this->m_user_error)) {
            return 'user';
        }

        return 'system';
    }

    /**
     * Get SQLSTATE errorcode (the default one).
     *
     * DEPRECATED : use errorCode() in place.
     *
     * @return SQLSTATE error code.
     */
    public function getDbErrno()
    {
        return $this->errorCode();
    }

    /**
     * Get error-info message returned by PDO.
     *
     * @return string error message.
     */
    public function getDbError()
    {
        return $this->errorInfo()[2];
    }

    /**
     * Define custom user error codes.
     *
     * Error codes passed to this method will be treated as recoverable user
     * errors.
     *
     * @param mixed $errno Vendor-dependent database error code
     */
    public function setUserError($errno)
    {
        Tools::atkdebug(__CLASS__.'::setUserError() -> '.$errno);
        $this->m_user_error[] = $errno;
    }

    /**
     * Get localized error message (for display in the application).
     *
     * @var \PDOStatement $stmt fetch error information from this statement.
     *                          if not set, will fetch error info from $this.
     *
     * @return string Error message
     */
    public function getErrorMsg($stmt = null)
    {
        $errInfo = $stmt ? $stmt->errorInfo() : $this->errorInfo();
        return Tools::atktext('unknown_error').": SQLSTATE:{$errInfo[0]} [{$errInfo[1]}] ({$errInfo[2]})";
    }

    /**
     * If haltonerror is set, this will raise an atkerror. If not, it will
     * place the error in atkdebug and continue.
     *
     * @param string $message     to print out to user
     * @param \PDOStatement $stmt where to find error information (if not set : on self::)
     */
    public function halt($message = '', $stmt = null)
    {
        Tools::atkdebug($message . ' : ' . $this->getErrorMsg($stmt));
        if (!$this->m_haltonerror or $this->getErrorType() == 'user') {
            Tools::atkdebug(__CLASS__.'::halt() on user error (not halting)');
            return;
        }
        $this->rollBack();
        Tools::atkdebug(__CLASS__.'::halt() on system error');
        Tools::atkhalt($message, 'critical');
    }

    /*********************************************** Helper functions *************************************************/
    /**
     * Create an Query object for constructing queries.
     *
     * @param string $table the query will run on.
     *
     * @return Query Query class.
     */
    public function createQuery($table = '')
    {
        return new Query($table, $this);
    }

    /**
     * Rollback the current transaction.
     *
     * DEPRECATED in favor of standard PDO::rollBack.
     *
     * @return bool
     */
    public function rollback()
    {
        return parent::rollBack();
    }

    /**
     * Commit current transaction and start a new one.
     *
     * @return bool $succss
     */
    public function commit()
    {
        Tools::atkdebug('Committing');
        $retVal = parent::commit();
        if (!$this->beginTransaction()) {
            $this->halt('Starting transaction failed');
        }
        return $retVal;
    }

    /**
     * Creates a new statement for the given query.
     *
     * @param string $query SQL query
     *
     * @return \PDOStatement statement
     */
    public function prepare($query, $options = [])
    {
        Tools::atkdebug("Preparing query : $query");
        $stmt = parent::prepare($query, $options);
        if (!$stmt) {
            $this->halt('Query preparation failed');
        }

        return $stmt;
    }

    /**
     * Execute a query.
     *
     * @param string $query The SQL query to execute
     *
     * @return \PDOStatement
     */
    public function query($query)
    {
        Tools::atkdebug("Running query : $query");
        $stmt = parent::query($query);
        if (!$stmt) {
            $this->halt('Query execution failed');
        }
        return $stmt;
    }

    /**
     * Prepare & execute a query with a prepared statement.
     *
     * @param QueryPart|string $query to prepare and execute
     * @param array $parameters for the query (only if $query is not already au QueryPart)
     *
     * @return \PDOStatement
     */
    public function queryP($query, $parameters = [])
    {
        if (!$query instanceof QueryPart) {
            $query = new QueryPart($query, $parameters);
        }

        // Fast-track : no parameters -> using \PDO::query
        if (!count($query->parameters)) {
            return $this->query($query->sql);
        }

        $stmt = $this->prepare($query->sql);
        if (!$stmt) {
            // Error handling already have been done in $this->prepare.
            return null;
        }
        foreach ($query->parameters as $placeholder => $parameter) {
            $stmt->bindValue($placeholder, $parameter[0], $parameter[1]);
        }
        Tools::atkdebug("Executing query");
        if(!$stmt->execute()) {
            $this->halt('Query execution failed', $stmt);
        }
        return $stmt;
    }

    /**
     * Get the next sequence number of a certain sequence.
     *
     * If the sequence does not exist, it is created automatically.
     *
     * @param string $sequence The sequence name
     *
     * @return int|bool The next sequence value or false on fail
     */
    public function nextid($sequence)
    {
        $result = $this->getValue(
            'SELECT '.self::quoteIdentifier($this->m_seq_field).' FROM '.self::quoteIdentifier($this->m_seq_table).
            ' WHERE '.self::quoteIdentifier($this->m_seq_namefield).' = :sequence_name',
            [':sequence_name' => $sequence]);
        /* no current value, make one */
        if (!$result) {
            $this->prepare('INSERT INTO '.self::quoteIdentifier($this->m_seq_table)." VALUES(:sequence_name, 1)")
                ->execute([':sequence_name' => $sequence]);
            $nextid = 1;
        } else {
           $nextid = $result[$this->m_seq_field] + 1;
            $this->prepare('UPDATE '.self::quoteIdentifier($this->m_seq_table).' SET '.
                self::quoteIdentifier($this->m_seq_field).' = :nextid WHERE '.
                self::quoteIdentifier($this->m_seq_field).' = :sequence_name')
                ->execute([':nextid' => $nextid, ':sequence_name' => ':sequence_name']);
        }
        return $nextid;
    }

    /**
     * Returns the first row for the given query.
     *
     * Please note: this method does *not* add a limit to the query
     *
     * @param QueryPart|string $query query
     * @param array $parameters for the query (only if $query is not already au QueryPart)
     *
     * @return array row
     */
    public function getRow($query, $parameters = [])
    {
        return $this->queryP($query, $parameters)->fetch();
    }

    /**
     * Get all rows for the given query.
     *
     * NOTE:
     * This is not an efficient way to retrieve records, as this
     * will load all records into one array into memory. If you
     * retrieve a lot of records, you might hit the memory_limit
     * and your script will die.
     *
     * @param QueryPart|string $query query
     * @param array $parameters for the query (only if $query is not already au QueryPart)
     *
     * @return array rows
     */
    public function getRows($query, $parameters = [])
    {
        return $this->queryP($query, $parameters)->fetchAll();
    }

    /**
     * Get the value of the first column of the first row returned by $query
     *
     * @param QueryPart|string $query query
     * @param array $parameters for the query (only if $query is not already au QueryPart)
     *
     * @return mixed value if found, null if not
     */
    public function getValue($query, $parameters = [])
    {
        $row =  $this->queryP($query, $parameters)->fetch(\PDO::FETCH_NUM);
        return $row[0] ?? null;
    }

    /*********************************************** Metadata computation *************************************************/
    /**
     * Fetches table meta data from memory if already loaded, from cache into file if present,
     * from database if not.
     *
     * @param string $table
     *
     * @return array
     */
    public function tableMeta($table)
    {
        if (isset($this->m_tableMeta[$table])) {
            return $this->m_tableMeta[$table];
        }

        if (Config::getGlobal('meta_caching')) {
            $this->m_tableMeta[$table] = $this->_getTableMetaFromCache($table);
        } else {
            $this->m_tableMeta[$table] = $this->metadata($table);
        }

        return $this->m_tableMeta[$table];
    }

    /**
     * If cached it'll return the table metadata
     * from cache.
     *
     * @param string $table
     *
     * @return array
     */
    private function _getTableMetaFromCache($table)
    {
        $tmpfile = new TmpFile('tablemeta/'.$this->m_connection.'/'.$table.'.php');

        $tablemeta = [];
        if ($tmpfile->exists()) {
            include $tmpfile->getPath();
        } else {
            $tablemeta = $this->metadata($table);
            $tmpfile->writeAsPhp('tablemeta', $tablemeta);
        }

        return $tablemeta;
    }

    /**
     * Retrieve the meta data of a certain table from the database
     *
     * Return an column name-indexed array of arrays containing :
     *     "table"          table name
     *     "name"           field name
     *     "type"           field type (driver-specific)
     *     "gentype"        field type (one of self::FT_* constants)
     *     "len"            field length (for text and number fields)
     *     "precision"      the numeric precision of this column for decimal types
     *     "flags"          taken from self::MF_ list
     *
     * @param string $table     the table name
     * @param array  $extraCols to add columns from information_schema query
     *
     * @return array with meta data
     */
    protected function metadata($table, $extraCols = [])
    {
        /* list fields */
        Tools::atkdebug("Retrieving metadata for $table");

        /* Array from our meta titles ton information_shema ones : */
        $columns = array_merge([
            'table_name',
            'column_name',
            'data_type',
            'character_maximum_length',
            'numeric_precision',
            'numeric_scale',
            'is_nullable',
            ],
            $extraCols);
        /* get meta information */
        $stmt = $this->queryP(
            'SELECT '.implode(',', $columns). ' FROM information_schema.columns WHERE table_name = :tablename;',
            [':tablename' => $table]
        );

        /* Transforming to our destination array */
        $result = [];
        while ($field = $stmt->fetch()) {
            $key = $field['column_name'];
            $result[$key] = [
                'table' => $field['table_name'],
                'name' => $field['column_name'],
                'type' => $field['data_type'],
                'gentype' => $this->getGenericType($field['data_type']),
                'len' => $field['character_maximum_length'] ?? $field['numeric_precision'],
                'precision' => $field['numeric_scale'],
                'flags' => ($field['is_nullable'] == 'NO') ? self::MF_NOT_NULL : 0,
                ];
            foreach ($extraCols as $column) {
                $result[$key][$column] = $field[$column];
            }
        }

        Tools::atkdebug("Metadata for $table complete");

        return $result;
    }

    /**
     * Get self::FT_ type from the type returned by the database driver
     *
     * @param string $type returned by the database driver
     *
     * @return int self::FT_type
     */
    protected function getGenericType($type) {
        // Make the string more generic :
        $type = strtolower($type);
        // Fast-tracks : '*int*', '*char*' and timestamp types :
        if (strpos($type, 'int') !== false) {
            return self::FT_NUMBER;
        }
        if (strpos($type, 'char') !== false or strpos($type, 'blob') !== false) {
            return self::FT_STRING;
        }
        if (strpos($type, 'timestamp') !== false) {
            return self::FT_DATETIME;
        }

        switch (strtolower($type)) {
            case 'boolean':
                return self::FT_BOOLEAN;
                break;
            case 'varchar':
            case 'text':
            case 'character varying':
            case 'enum':
                return self::FT_STRING;
                break;
            case 'decimal':
            case 'float':
            case 'numeric':
                return self::FT_DECIMAL;
                break;
            case 'datetime':
                return self::FT_DATETIME;
                break;
            case 'date':
                return self::FT_DATE;
                break;
            case 'time':
                return self::FT_TIME;
                break;
            default:
                Tools::atkwarning("Unsupported database type '{$type}'");
                return self::FT_UNSUPPORTED; // NOT SUPPORTED FIELD TYPES 
                break;
        }
    }

    /**
     * This function checks the database for a table with
     * the provide name.
     *
     * @param string $tableName the table to find
     *
     * @return bool true if found, false if not found
     */
    public function tableExists($tableName)
    {
        return $this->getValue('select COUNT(*) from information_schema.tables WHERE table_name = :table', [':table' => $tableName]
        );
    }

    /*********************************************** SQL functions *************************************************/
    /**
     * get NOW() or SYSDATE() equivalent for the current database.
     *
     * Every database has it's own implementation to get the current date
     *
     * @return string
     */
    public function func_now()
    {
        return 'NOW()';
    }

    /**
     * get SUBSTRING() equivalent for the current database.
     *
     * @param string $fieldname The database fieldname (already quoted)
     * @param int $startat The position to start from
     * @param int $length The number of characters
     *
     * @return string
     */
    public function func_substring($fieldname, $startat = 0, $length = 0)
    {
        return "SUBSTRING($fieldname, $startat".($length != 0 ? ", $length" : '').')';
    }

    /**
     * Get TO_CHAR() equivalent for the current database.
     * Each database driver should override this method to perform vendor
     * specific conversion.
     *
     * @param string $fieldname The field to generate the to_char for (already quoted)
     * @param string $format Format specifier. The format is compatible with
     *                          php's date() function (http://www.php.net/date)
     *                          The default is what's specified by
     *                          $config_date_to_char, or "Y-m-d" if not
     *                          set in the configuration.
     *
     * @return string Piece of sql query that converts a date field to char
     *                for the current database
     */
    public function func_datetochar($fieldname, $format = '')
    {
        if ($format == '') {
            $format = Config::getGlobal('date_to_char', 'Y-m-d');
        }

        return "TO_CHAR($fieldname, '".$this->vendorDateFormat($format)."')";
    }

    /**
     * Get CONCAT_WS() equivalent for the current database.
     *
     * The main interest is to concat strings together without a null string making
     * the generated expression null : null strings are replaced by ''.
     *
     * @DEPRECATED
     *
     * @param array $fields (quoted)
     * @param string $separator (unquoted)
     * @param bool $remove_all_spaces remove all spaces in result (atkAggrecatedColumns searches for string without spaces)
     *
     * @return string $query_part
     */
    public function func_concat_ws(array $fields, string $separator, bool $remove_all_spaces = false)
    {
        Tools::atkwarning('DEPRECATED : use func_concat_coalesce() or ad-hoc functions if you need a separator');
        return $this->func_concat_coalesce($fields);
    }

    /**
     * Concat all fields/expressions and coalesce to empty string null values.
     *
     * The main interest is to concat strings together without a null string making
     * the generated expression null.
     *
     * @param array $fields (quoted)
     *
     * @return string $query_part
     */
    public function func_concat_coalesce(array $fields) : string
    {
        if (empty($fields)) {
            return '';
        }
        return 'COALESCE('.implode(",'')||COALESCE(", $fields).",'')";
    }

    /**
     * Convert a php date() format specifier to a vendor specific format
     * specifier.
     * The default implementation returns the format as used by many
     * database vendors ('YYYYMMDD HH24:MI'). Databases that use different
     * formatting, should override this method.
     *
     * Note that currently, only the common specifiers Y, m, d, H, h, i and
     * s are supported.
     *
     * @param string $format Format specifier. The format is compatible with
     *                       php's date() function (http://www.php.net/date)
     *
     * @return string Vendor specific format specifier.
     */
    public function vendorDateFormat($format)
    {
        $php_fmt = array('Y', 'm', 'd', 'H', 'h', 'i', 's');
        $db_fmt = array('YYYY', 'MM', 'DD', 'HH24', 'HH12', 'MI', 'SS');

        return str_replace($php_fmt, $db_fmt, $format);
    }

    /**
     * Get TO_CHAR() equivalent for the current database.
     *
     * TODO/FIXME: add format parameter. Current format is always yyyy-mm-dd hh:mi.
     *
     * @param string $fieldname The field to generate the to_char for (quoted).
     *
     * @return string Piece of sql query that converts a datetime field to char
     *                for the current database
     */
    public function func_datetimetochar($fieldname)
    {
        return "TO_CHAR($fieldname, 'YYYY-MM-DD hh:mi')";
    }

    /******************************************** Escaping functions **********************************************************/
    /**
     * escapes quotes for use in SQL: ' -> '' (and sometimes % -> %%).
     *
     * DEPRECATED. Use prepared queries. See
     * http://php.net/manual/en/pdo.quote.php
     *
     * @param string $string The string to escape
     * @param bool $wildcard also escape wildcards?
     *
     * @return string The escaped SQL string
     */
    public function escapeSQL($string, $wildcard = false)
    {
        Tools::atkerror('escapeSQL function is deprecated and not safe. Don\'t use it.');
    }

    /**
     * Quote Indentifier with " (which works on most DB vendor and on MySQL in ANSI mode)
     *
     * This function should be applied to every field, table or sequence name which comes from 
     * the framework user.
     * examples :
     *   Db::quoteIdentifier($field);
     *   Db::quoteIdentifier($table, $field);
     *
     * @param string $identifier1 to escape
     * @param string $identifier2 : if present, will return $identifier1.$identifier2 (both escaped).
     *
     * @return string
     */
    public static function quoteIdentifier($identifier, $secondIdentifier = '')
    {
        if ($secondIdentifier) {
            return self::quoteIdentifier($identifier).'.'.self::quoteIdentifier($secondIdentifier);
        }
        return '"'.str_replace('"', '""', $identifier).'"';
    }
}
