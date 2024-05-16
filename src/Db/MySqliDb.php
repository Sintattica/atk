<?php

namespace Sintattica\Atk\Db;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Utils\Debugger;

/**
 * Driver for MySQL databases > 4.1.3.
 *
 * @author Eldad Ran <eldad@tele-concept.com>
 */
class MySqliDb extends Db
{
    /*
     * The last insert id from the last query
     * @var integer
     * @access protected
     */
    public $m_insert_id;

    /* sequence table */
    public $m_seq_table = 'db_sequence';
    // the field in the seq_table that contains the counter..
    public $m_seq_field = 'nextid';
    // the field in the seq_table that countains the name of the sequence..
    public $m_seq_namefield = 'seq_name';
    public $m_type = 'mysql';
    protected $m_identifierQuoting = array('start' => '`', 'end' => '`', 'escape' => '`');

    /**
     * Base constructor.
     */
    public function __construct()
    {
        if (!function_exists('mysqli_connect')) {
            trigger_error('MySQLi not supported by your PHP version', E_USER_ERROR);
        }

        parent::__construct();

        // set type
        $this->m_type = 'MySqli';
        $this->m_vendor = 'mysql';
        $this->m_user_error = array(1451);
    }

    /**
     * Connect to the database.
     *
     * @param string $host Hostname
     * @param string $user Username
     * @param string $password Password
     * @param string $database The database to connect to
     * @param int $port The portnumber to use for connecting
     * @param string $charset The charset to use
     *
     * @return mixed Connection status
     */
    public function doConnect($host, $user, $password, $database, $port, $charset)
    {
        /* establish connection */
        if (empty($this->m_link_id)) {
            if (empty($port)) {
                $port = null;
            }
            $this->m_link_id = @mysqli_connect($host, $user, $password, $database, $port);
            if (!$this->m_link_id) {
                $this->halt($this->getErrorMsg());

                return $this->_translateError();
            }

            /* set character set */
            if (!empty($charset)) {
                Tools::atkdebug("Set database character set to: {$charset}");
                $this->_query("SET NAMES '{$charset}'", true);
            }

            /* set autoCommit to off */
            mysqli_autocommit($this->m_link_id, false);
        }

        /* return link identifier */

        return self::DB_SUCCESS;
    }

    /**
     * Determine whether an error that occurred is a recoverable (user) error
     * or a system error.
     *
     * @return string "user" or "system"
     */
    public function getErrorType()
    {
        $this->_setErrorVariables();

        return parent::getErrorType();
    }

    /**
     * Translates known database errors to developer-friendly messages.
     *
     * @param int $errno
     *
     * @return int Flag of the error
     */
    public function _translateError($errno = null)
    {
        $this->_setErrorVariables();
        switch ($this->m_errno) {
            case 0:
                return self::DB_SUCCESS;
            case 1044:
                return self::DB_ACCESSDENIED_DB;  // todofixme: deze komt bij mysql pas na de eerste query.
            case 1045:
                return self::DB_ACCESSDENIED_USER;
            case 1049:
                return self::DB_UNKNOWNDATABASE;
            case 2004:
            case 2005:
                return self::DB_UNKNOWNHOST;
            default:
                Tools::atkdebug('mysqldb::translateError -> MySQL Error: ' . $this->m_errno . ' -> ' . $this->m_error);

                return self::DB_UNKNOWNERROR;
        }
    }

    /**
     * Store MySQL errors in internal variables.
     */
    public function _setErrorVariables()
    {
        if (!empty($this->m_link_id)) {
            $this->m_errno = mysqli_errno($this->m_link_id);
            $this->m_error = mysqli_error($this->m_link_id);
        } else {
            $this->m_errno = mysqli_connect_errno();
            $this->m_error = mysqli_connect_error();
        }
    }

    /**
     * Disconnect from database.
     */
    public function disconnect()
    {
        if ($this->m_link_id) {
            Tools::atkdebug('Disconnecting from database...');
            @mysqli_close($this->m_link_id);
            $this->m_link_id = 0;
        }
    }

    /**
     * Escaping a MySQL string, in a mysqli safe way.
     *
     * @param string $string
     * @param bool $wildcard
     *
     * @return string
     */
    public function escapeSQL($string, $wildcard = false)
    {
        if ($this->connect() === self::DB_SUCCESS) {
            if ($wildcard == true) {
                $string = str_replace('%', '\%', $string);
            }

            return mysqli_real_escape_string($this->m_link_id, $string);
        }

        return '';
    }

    /**
     * Performs a query.
     *
     * @param string $query the query
     * @param int $offset offset in record list
     * @param int $limit maximum number of records
     *
     * @return bool
     */
    public function query($query, $offset = -1, $limit = -1)
    {
        /* limit? */
        if ($offset >= 0 && $limit > 0) {
            $query .= " LIMIT $offset, $limit";
        }

        /* connect to database */
        $mode = $this->getQueryMode($query);
        if ($this->connect() == self::DB_SUCCESS) {
            /* free old results */
            if ($this->m_query_id) {
                if (is_object($this->m_query_id)) {
                    mysqli_free_result($this->m_query_id);
                }
                $this->m_query_id = 0;
            }

            $this->m_affected_rows = 0;

            /* query database */
            $this->m_query_id = $this->_query($query, false);

            /* get the last insert id
             * this is harmless and returns 0 if the query wasn't
             * an insert or update or if the table has no autoincrement
             */
            $this->m_insert_id = mysqli_insert_id($this->m_link_id);

            $unlock_table = false;
            if (mysqli_errno($this->m_link_id) == 1100) {
                $this->locktables_fallback_on_error($query, $mode);
                $unlock_table = true;
            } else {
                $this->m_affected_rows = mysqli_affected_rows($this->m_link_id);
            }

            $this->m_row = 0;

            /* invalid query */
            if (!$this->m_query_id) {
                $this->halt("Invalid SQL: $query");

                return false;
            }

            if ($unlock_table) {
                $this->unlock();
            }

            if (Config::getGlobal('debug') >= 1) {
                $this->debugWarnings();
            }

            return true;
        }

        return false;
    }

    /**
     * Execute and log query.
     *
     * @param string $query query
     * @param bool $isSystemQuery is system query? (e.g. for retrieving metadata, warnings, setting locks etc.)
     *
     * @return mixed
     */
    protected function _query($query, $isSystemQuery)
    {
        if (Config::getGlobal('debug') >= 0) {
            Debugger::addQuery($query, $isSystemQuery);
        }

        $result = @mysqli_query($this->m_link_id, $query);
        if (!$result && mysqli_errno($this->m_link_id) === 2006) {
            Tools::atkdebug('DB has gone away, try to reconnect');
            $this->disconnect();
            if ($this->connect() !== Db::DB_SUCCESS) {
                Tools::atkerror('Cannot connect to database.');
            } else {
                $result = @mysqli_query($this->m_link_id, $query);
            }
        }

        return $result;
    }

    /**
     * Get all MySQL warnings for the previously executed query
     * and make atkwarnings of them.
     */
    public function debugWarnings()
    {
        $stmt = $this->_query('SHOW WARNINGS', true);

        $warnings = [];
        while ($warning = $stmt->fetch_assoc()) {
            $warnings[] = $warning;
        }

        foreach ($warnings as $warning) {
            Tools::atkwarning("MYSQL warning '{$warning['Level']}' (Code: {$warning['Code']}): {$warning['Message']}");
        }
    }

    /**
     * This method provides a fallback when error 1100 occurs
     * (Table ... not locked using LOCK TABLES). This method locks
     * the table and runs the query again.
     *
     * @param string $query The original query that failed
     * @param string $querymode Kind of query - 'w' for write or 'r' for read
     */
    public function locktables_fallback_on_error($query, $querymode = 'w')
    {
        $error = mysqli_error($this->m_link_id);

        $matches = [];
        preg_match("/\'(.*)\'/U", $error, $matches);

        if (is_array($matches) && sizeof($matches) == 2) {
            Tools::atkdebug("<b>Fallback feature called because error '1100' occured during the last query. Running query again using table lock for table '{$matches[1]}'.</b>");
            $table = $matches[1];

            if ($this->m_query_id) {
                if (!empty($this->m_query_id)) {
                    mysqli_free_result($this->m_query_id);
                }
                $this->m_query_id = 0;
            }
            $this->m_affected_rows = 0;

            $this->lock($table, ($querymode == 'r' ? 'read' : 'write'));
            $this->m_query_id = $this->_query($query, true);

            $this->m_affected_rows = mysqli_affected_rows($this->m_link_id);
        }
    }

    /**
     * Goto the next record in the result set.
     *
     * @return bool of going to the next record
     */
    public function next_record()
    {
        /* goto next record */
        $this->m_record = @mysqli_fetch_array($this->m_query_id, MYSQLI_ASSOC | Config::getGlobal('mysqlfetchmode'));
        ++$this->m_row;
        $this->m_errno = mysqli_errno($this->m_link_id);
        $this->m_error = mysqli_error($this->m_link_id);

        /* are we there? */
        $result = is_array($this->m_record);
        if (!$result && $this->m_auto_free) {
            @mysqli_free_result($this->m_query_id);
            $this->m_query_id = 0;
        }

        /* return result */

        return $result;
    }

    /**
     * Goto a certain position in result set.
     * Not specifying a position will set the pointer
     * at the beginning of the result set.
     *
     * @param int $position the position
     */
    public function seek($position = 0)
    {
        $result = @mysqli_data_seek($this->m_query_id, $position);
        if ($result) {
            $this->m_row = $position;
        } else {
            $this->halt("seek($position) failed: result has " . $this->num_rows() . ' rows');
        }
    }

    /**
     * Lock a certain table in the database.
     *
     * @param string $table the table name
     * @param string $mode the type of locking
     *
     * @return bool of locking
     */
    public function lock($table, $mode = 'write')
    {
        /* connect first */
        if ($this->connect() == self::DB_SUCCESS) {
            /* lock */
            $query = "LOCK TABLES $table $mode";

            if (Config::getGlobal('debug') >= 0) {
                Debugger::addQuery($query);
            }

            $result = $this->_query($query, true);
            if (!$result) {
                $this->halt("$mode lock on $table failed.");
            }

            /* return result */

            return $result;
        }

        return 0;
    }

    /**
     * Unlock table(s) in the database.
     *
     * @return bool result of unlocking
     */
    public function unlock()
    {
        /* connect first */
        if ($this->connect() == self::DB_SUCCESS) {
            /* unlock */
            Tools::atkdebug('unlock tables');
            $result = $this->_query('UNLOCK TABLES', true);
            if (!$result) {
                $this->halt('unlock tables failed.');
            }

            /* return result */

            return $result;
        }

        return 0;
    }

    /**
     * Evaluate the result; which rows were
     * affected by the query.
     *
     * @return int affected rows
     */
    public function affected_rows()
    {
        return $this->m_affected_rows;
    }

    /**
     * Evaluate the result; how many rows
     * were affected by the query.
     *
     * @return number of affected rows
     */
    public function num_rows()
    {
        return @mysqli_num_rows($this->m_query_id);
    }

    /**
     * Evaluate the result; how many fields
     * where affected by the query.
     *
     * @return int number of affected fields
     */
    public function num_fields()
    {
        return @mysqli_num_fields($this->m_query_id);
    }

    /**
     * Get the next sequence number
     * of a certain sequence.
     *
     * @param string $sequence the sequence name
     *
     * @return int the next sequence id
     */
    public function nextid($sequence)
    {
        /* first connect */
        if ($this->connect() == self::DB_SUCCESS) {
            /* lock sequence table */
            if ($this->lock($this->m_seq_table)) {
                /* get sequence number (locked) and increment */
                $query = 'SELECT ' . $this->m_seq_field . ' FROM ' . $this->m_seq_table . ' WHERE ' . $this->m_seq_namefield . " = '$sequence'";

                $id = $this->_query($query, true);
                $result = @mysqli_fetch_array($id);

                /* no current value, make one */
                if (!is_array($result)) {
                    $query = 'INSERT INTO ' . $this->m_seq_table . " VALUES('$sequence', 1)";
                    $this->_query($query, true);
                    $this->unlock();

                    return 1;
                } /* enter next value */ else {
                    $nextid = $result[$this->m_seq_field] + 1;
                    $query = 'UPDATE ' . $this->m_seq_table . ' SET ' . $this->m_seq_field . " = '$nextid' WHERE " . $this->m_seq_namefield . " = '$sequence'";

                    $this->_query($query, true);
                    $this->unlock();

                    return $nextid;
                }
            }

            return 0;
        } /* cannot connect */ else {
            $this->halt('cannot connect to ' . $this->m_host);
        }
    }

    /**
     * Drop all database tables.
     */
    public function dropAll()
    {
        $tables = $this->table_names();
        foreach ($tables as $table) {
            $this->query('DROP TABLE `' . $table['table_name'] . '`');
        }
    }

    /**
     * This function checks the database for a table with
     * the provide name.
     *
     * @param string $table the table to find
     *
     * @return bool true if found, false if not found
     */
    public function tableExists($table)
    {
        $this->connect();
        if (strpos($table, '.') !== false) {
            list($dbname, $tablename) = explode('.', $table);
            $id = $this->_query('SHOW TABLES FROM `' . $dbname . "` LIKE '" . $tablename . "'", true);
        } else {
            $id = $this->_query("SHOW TABLES LIKE '" . $table . "'", true);
        }

        $result = @mysqli_num_rows($id) > 0;
        Tools::atkdebug("Table exists? $table => " . ($result ? 'yes' : 'no'));

        return $result;
    }

    /**
     * This function indicates what searchmodes the database supports.
     *
     * @return array with search modes
     */
    public function getSearchModes()
    {
        return array(
            'exact',
            'substring',
            'wildcard',
            'regexp',
            'soundex',
            'greaterthan',
            'greaterthanequal',
            'lessthan',
            'lessthanequal',
            'between',
        );
    }

    /**
     * Get TO_CHAR() equivalent for the current database.
     * Each database driver should override this method to perform vendor
     * specific conversion.
     *
     * @param string $fieldname The field to generate the to_char for.
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

        return "DATE_FORMAT($fieldname, '" . $this->vendorDateFormat($format) . "')";
    }

    /**
     * Convert a php date() format specifier to a mysql specific format
     * specifier.
     *
     * Note that currently, only the common specifiers Y, m, d, H, h, i and
     * s are supported.
     *
     * @param string $format Format specifier. The format is compatible with
     *                       php's date() function (http://www.php.net/date)
     *
     * @return string Mysql specific format specifier.
     */
    public function vendorDateFormat($format)
    {
        $php_fmt = array('Y', 'm', 'd', 'H', 'h', 'i', 's');
        $db_fmt = array('%Y', '%m', '%d', '%H', '%h', '%i', '%s');

        return str_replace($php_fmt, $db_fmt, $format);
    }

    /**
     * Returns the table type.
     *
     * @param string $table table name
     *
     * @return string table type
     */
    public function _getTableType($table)
    {
        $this->connect();
        $id = $this->_query("SHOW TABLE STATUS LIKE '" . $table . "'", true);
        $status = @mysqli_fetch_array($id, MYSQLI_ASSOC);
        $result = $status != null && isset($status['Engine']) ? $status['Engine'] : null;
        Tools::atkdebug("Table type? $table => $result");

        return $result;
    }

    /**
     * Return the meta data of a certain table.
     *
     * @param string $table the table name
     * @param bool $full all meta data or not
     *
     * @return array with meta data
     */
    public function metadata($table, $full = false)
    {
        /* first connect */
        if ($this->connect() == self::DB_SUCCESS) {
            $ddl = Ddl::create('MySqli');

            /* list fields */
            Tools::atkdebug("Retrieving metadata for $table");

            /* The tablename may also contain a schema. If so we check for it. */
            if (strpos($table, '.') !== false) {
                list($dbname, $tablename) = explode('.', $table);

                /* get meta data */
                $id = @$this->_query("SELECT * FROM `{$dbname}`.`{$tablename}` LIMIT 0", true);
            } else {
                /* get meta data */
                $id = $this->_query("SELECT * FROM `{$table}` LIMIT 0", true);
            }

            // table type
            $tableType = $this->_getTableType(isset($tablename) ? $tablename : $table);

            if (!$id) {
                Tools::atkdebug('Metadata query failed.');

                return [];
            }
            $i = 0;
            $result = [];

            while ($finfo = mysqli_fetch_field($id)) {
                $result[$i]['table'] = $finfo->table;
                $result[$i]['table_type'] = $tableType;
                $result[$i]['name'] = $finfo->name;
                $result[$i]['type'] = $finfo->type;
                $result[$i]['gentype'] = $ddl->getGenericType($finfo->type);
                $result[$i]['len'] = $finfo->length;
                $result[$i]['flags'] = 0;

                // if the connection character set is UTF8 MySQL returns the length multiplied
                // by 3, probably because the max length of an UTF8 character is 3 bytes, we need
                // the real size in characters, so we divide the length by 3
                if (strtoupper($this->m_charset) == 'UTF8' && ($result[$i]['gentype'] == 'string' || $result[$i]['gentype'] == 'text')) {
                    $result[$i]['len'] /= 3;
                } else {
                    if ($result[$i]['gentype'] == 'decimal') {
                        // for a mysql type DECIMAL, the length is returned as M+2 (signed) or M+1 (unsigned)
                        $offset = ($finfo->flags & MYSQLI_UNSIGNED_FLAG) ? 1 : 2;
                        $result[$i]['len'] -= ($offset + $finfo->decimals);
                        $result[$i]['len'] .= ',' . $finfo->decimals;
                        // TODO we should also save the "unsigned" flag in $result[$i]["flags"]
                    }
                }
                if ($finfo->flags & MYSQLI_PRI_KEY_FLAG) {
                    $result[$i]['flags'] |= Db::MF_PRIMARY;
                }
                if ($finfo->flags & MYSQLI_UNIQUE_KEY_FLAG) {
                    $result[$i]['flags'] |= Db::MF_UNIQUE;
                }
                if ($finfo->flags & MYSQLI_NOT_NULL_FLAG) {
                    $result[$i]['flags'] |= Db::MF_NOT_NULL;
                }
                if ($finfo->flags & MYSQLI_AUTO_INCREMENT_FLAG) {
                    $result[$i]['flags'] |= Db::MF_AUTO_INCREMENT;
                }

                if ($full) {
                    $result['meta'][$result[$i]['name']] = $i;
                }
                ++$i;
            }

            if ($full) {
                $result['num_fields'] = $i;
            }

            mysqli_free_result($id);

            Tools::atkdebug("Metadata for $table complete");

            return $result;
        }

        return [];
    }

    /**
     * Return the available table names.
     *
     * @param bool $includeViews Include views?
     *
     * @return array with table names etc.
     */
    public function table_names($includeViews = true)
    {
        // query
        $this->query('SHOW ' . (!$includeViews ? 'FULL' : '') . ' TABLES');

        // get table names
        $result = [];
        for ($i = 0; $info = mysqli_fetch_row($this->m_query_id); ++$i) {
            // ignore views?
            if (!$includeViews && strtoupper($info[1]) == 'VIEW') {
                continue;
            }

            $result[$i]['table_name'] = $info[0];
            $result[$i]['tablespace_name'] = $this->m_database;
            $result[$i]['database'] = $this->m_database;
        }

        // return result
        return $result;
    }

    /**
     * Commit the current transaction.
     *
     * @return bool true
     */
    public function commit()
    {
        if ($this->m_link_id) {
            Tools::atkdebug('Commit');
            mysqli_commit($this->m_link_id);
        }

        return true;
    }

    /**
     * Set savepoint with the given name.
     *
     * @param string $name savepoint name
     */
    public function savepoint($name)
    {
        Tools::atkdebug(get_class($this) . "::savepoint $name");
        $this->query('SAVEPOINT ' . $name);
    }

    /**
     * Rollback the the current transaction.
     *
     * @param string $savepoint The savepoint to rollback to
     *
     * @return bool
     */
    public function rollback($savepoint = '')
    {
        if ($this->m_link_id) {
            if (!empty($savepoint)) {
                Tools::atkdebug(get_class($this) . "::rollback (rollback to savepoint $savepoint)");
                $this->query('ROLLBACK TO SAVEPOINT ' . $savepoint);
            } else {
                Tools::atkdebug('Rollback');
                mysqli_rollback($this->m_link_id);
            }
        }

        return true;
    }

    /**
     * Enable/disable all foreign key constraints.
     *
     * @param bool $enable enable/disable foreign keys?
     */
    public function toggleForeignKeys($enable)
    {
        $this->query('SET FOREIGN_KEY_CHECKS = ' . ($enable ? 1 : 0));
    }

    /**
     * Returns the last inserted auto increment value.
     *
     * @return int auto increment value of latest insert query
     */
    public function getInsertId()
    {
        return $this->m_insert_id;
    }
}
