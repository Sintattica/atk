<?php

namespace Sintattica\Atk\Db;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\Debugger;

/**
 * Database driver for PostgreSQL.
 *
 * @author Peter Verhage <peter@ibuildings.nl>
 */
class PgSqlDb extends Db
{
    public $m_type = 'PgSql';
    public $m_vendor = 'postgresql';

    /**
     * Base constructor.
     */
    public function __construct()
    {
        parent::__construct();

        // force case-insensitive searching and ordering
        $this->m_force_ci = true;
    }

    public function doConnect($host, $user, $password, $database, $port, $charset): int
    {
        if (empty($this->m_link_id)) {
            $conn = [];

            if (!empty($host)) {
                $conn[] = 'host=' . $host;
            }

            if (!empty($user)) {
                $conn[] = 'user=' . $user;
            }
            if (!empty($password)) {
                $conn[] = 'password=' . $password;
            }

            if (!empty($database)) {
                $conn[] = 'dbname=' . $database;
            }

            if (!empty($port)) {
                $conn[] = 'port=' . $port;
            }

            if (!empty($charset)) {
                Tools::atkdebug("Set database character set to: {$charset}");
                $conn[] = "options='--client_encoding=$charset'";
            }

            $connection_str = implode(' ', $conn);


            /* establish connection */
            $this->m_link_id = pg_connect($connection_str);

            if ($this->m_link_id === false) {
                $this->halt("connect using ** $connection_str ** failed");

                // We can't use pg_result_error, since we need a resource
                // for that function, and if pg_connect fails, we don't even have
                // a resource yet.
                return $this->_translateError(@pg_last_error());
            }

            /* set autoCommit to off */
            $this->_query("BEGIN", true);
        }

        return self::DB_SUCCESS;
    }


    /**
     * TODO FIXME: I don't know what errormessges postgresql gives,
     *  so this function only returns self::DB_UNKNOWNERROR for now.
     *
     * @param mixed $error
     *
     * @return int The ATK error code
     */
    public function _translateError($error = null)
    {
        return parent::_translateError($error);
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

        return @pg_query($this->m_link_id, $query);
    }

    /**
     * Disconnect from database, we use a persistent
     * link, so this won't be necessary!
     */
    public function disconnect()
    {
    }

    /**
     * Goto the next record in the result set.
     *
     * @return bool result of going to the next record
     */
    public function next_record()
    {
        /* goto next record */
        $this->m_record = @pg_fetch_array($this->m_query_id, null, PGSQL_ASSOC);
        ++$this->m_row;

        /* are we there? */
        $result = is_array($this->m_record);
        if (!$result && $this->m_auto_free) {
            @pg_free_result($this->m_query_id);
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
        $result = pg_result_seek($this->m_query_id, $position);
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
     * @return mixed result of locking
     */
    public function lock($table, $mode = 'write')
    {
        /* connect first */
        if ($this->connect() == self::DB_SUCCESS) {
            /* lock */
            if ($mode == 'write') {
                $result = @pg_query($this->m_link_id, "LOCK TABLE $table") or $this->halt("cannot lock table $table");
            } else {
                $result = 1;
            }

            /* return result */

            return $result;
        }

        return 0;
    }

    /**
     * Unlock table(s) in the database.
     *
     * @return mixed result of unlocking
     */
    public function unlock()
    {
        /* connect first */
        if ($this->connect() == self::DB_SUCCESS) {
            /* unlock */
            $result = @pg_query($this->m_link_id, 'COMMIT') or $this->halt('cannot unlock tables');

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
        return @pg_affected_rows($this->m_query_id);
    }

    /**
     * Evaluate the result; how many rows
     * were affected by the query.
     *
     * @return int The number of affected rows
     */
    public function num_rows()
    {
        return @pg_num_rows($this->m_query_id);
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
        /* connect first */
        if ($this->connect() == self::DB_SUCCESS) {
            $sequencename = Config::getGlobal('database_sequenceprefix') . $sequence . Config::getGlobal('database_sequencesuffix');
            /* get sequence number and increment */
            $query = "SELECT nextval('$sequencename') AS nextid";

            /* execute query */
            $id = @pg_query($this->m_link_id, $query);

            // maybe the name should not have the default seq_ prefix.
            // this is here for backwardscompatibility. $config_database_sequenceprefix
            // should be used to specify the sequence prefix.
            if (empty($id)) {
                /* get sequence number and increment */
                $query = "SELECT nextval('" . $sequence . "') AS nextid";

                /* execute query */
                $id = @pg_query($this->m_link_id, $query);
            }

            /* error? */
            if (empty($id)) {
                /* create sequence */
                $query = 'CREATE SEQUENCE ' . $sequencename;
                @pg_query($this->m_link_id, $query);

                /* try again */
                $query = "SELECT nextval('" . $sequencename . "') AS nextid";

                $id = @pg_query($this->m_link_id, $query) or $this->halt("cannot get nextval() of sequence '$sequencename'");

                if (empty($id)) {
                    return 0;
                }
            }

            return @pg_fetch_result($id, 0, 'nextid');
        } else {
            $this->halt('cannot connect to ' . $this->m_host);
        }

        return 0;
    }

    /**
     * Return the metadata of a certain table.
     *
     * @param string $table the table name
     * @param bool $full all metadata or not
     *
     * @return array with meta data
     */
    public function metadata($table, $full = false)
    {
        $ddl = Ddl::create('PgSql');

        if (strpos($table, '.') != false) {
            // there is a period in the table, so we split out the schema name.
            $schema = substr($table, 0, strpos($table, '.'));
            $table = substr($table, strpos($table, '.') + 1);
            $schema_condition = "AND n.nspname = '$schema' ";
            $schema_join = ' LEFT JOIN pg_namespace n ON (n.oid = c.relnamespace)';
        } else {
            // no period in the name, so there is no schema
            $schema_condition = '';
            $schema_join = '';
        }

        // Get metadata from system tables.
        // See developer manual (www.postgresql.org)
        // for system table specification.
        $sql = "SELECT
                a.attnum AS i,
                a.attname AS name,
                t.typname AS type,
                (CASE
                    WHEN LOWER(t.typname) = 'varchar' AND a.attlen = -1 THEN a.atttypmod - 4
                    WHEN a.atttypid = 21 /*int2*/ THEN 5
                    WHEN a.atttypid = 23 /*int4*/ THEN 10
                    WHEN a.atttypid = 20 /*int8*/ THEN 19
                    WHEN a.atttypid = 1700 /*numeric*/ THEN
                        CASE WHEN a.atttypmod = -1 THEN null
                        ELSE ((atttypmod - 4) >> 16) & 65535
                        END
                    ELSE a.attlen END
                ) AS length,
                (CASE WHEN a.attnotnull THEN 1 ELSE 0 END) AS is_not_null,
                (
                    SELECT COUNT(1)
                    FROM pg_index i
                    WHERE i.indrelid = c.oid
                    AND i.indisprimary = true
                    AND a.attnum IN (
                    i.indkey[0], i.indkey[1], i.indkey[2],
                    i.indkey[3], i.indkey[4], i.indkey[5],
                    i.indkey[6], i.indkey[7], i.indkey[8]
                    )
                    LIMIT 1
                ) AS is_primary,
                (
                    SELECT COUNT(1)
                    FROM pg_index i
                    WHERE i.indrelid = c.oid
                    AND i.indisunique = true
                    AND i.indnatts = 1
                    AND i.indkey[0] = a.attnum
                    LIMIT 1
                ) AS is_unique,          
                (CASE WHEN a.attidentity = 'd' AND a.attnotnull = true THEN 1 ELSE 0 END) AS is_auto_inc,
                '' AS default
        FROM pg_class c
        JOIN pg_attribute a ON (a.attrelid = c.oid AND a.attnum > 0)
        JOIN pg_type t ON (t.oid = a.atttypid)
        LEFT JOIN pg_attrdef ad ON (ad.adrelid = c.oid AND ad.adnum = a.attnum)
        $schema_join
        WHERE c.relname = '$table'
        $schema_condition
        ORDER BY a.attnum";

        // TODO: sembra che non si riesca più a ricavare questi valori...
        /*(CASE WHEN ad.adsrc LIKE 'nextval(%::text)' THEN 1 ELSE 0 END) AS is_auto_inc,
          (CASE WHEN ad.adsrc LIKE 'nextval(%::text)' THEN SUBSTRING(ad.adsrc, '''(.*?)''') END) AS sequence,
          (CASE WHEN t.typname = 'varchar' THEN SUBSTRING(ad.adsrc FROM '^''(.*)''.*$') ELSE ad.adsrc END) AS default*/

        $meta = [];
        $rows = $this->getRows($sql);

        foreach ($rows as $i => $row) {
            $meta[$i]['table'] = $table;
            $meta[$i]['type'] = $row['type'];
            $meta[$i]['gentype'] = $ddl->getGenericType($row['type']);
            $meta[$i]['name'] = $row['name'];
            $meta[$i]['len'] = $row['length'];
            $meta[$i]['flags'] = ($row['is_primary'] == 1 ? Db::MF_PRIMARY : 0) | ($row['is_unique'] == 1 ? Db::MF_UNIQUE : 0) | ($row['is_not_null'] == 1 ? Db::MF_NOT_NULL : 0) | ($row['is_auto_inc'] == 1 ? Db::MF_AUTO_INCREMENT : 0);

            if ($row['is_auto_inc'] == 1) {
                $meta[$i]['sequence'] = $row['sequence'] ?? null; // TODO
            } else {
                if (Tools::atk_strlen($row['default']) > 0) {
                    // date/time/datetime
                    if (strtolower($row['default']) == 'now' && in_array($meta[$i]['gentype'], ['date', 'time', 'datetime'])) {
                        $meta[$i]['default'] = 'NOW';
                    } // numbers
                    else {
                        if (in_array($meta[$i]['gentype'], ['number', 'decimal'])) {
                            $meta[$i]['default'] = $row['default'];
                        } // strings
                        else {
                            if (in_array($meta[$i]['gentype'], ['string', 'text'])) {
                                $meta[$i]['default'] = $row['default'];
                            } // boolean
                            else {
                                if ($meta[$i]['gentype'] == 'boolean') {
                                    $meta[$i]['default'] = strtolower($row['default']) == 't' ? 1 : 0;
                                }
                            }
                        }
                    }
                }
            }

            if ($full) {
                $meta['meta'][$row['name']] = &$meta[$i];
            }
        }

        if ($full) {
            $meta['num_fields'] = Tools::count($rows);
        }

        return $meta;
    }

    /**
     * Return the available table names.
     *
     * @param $includeViews bool
     *
     * @return array with table names etc.
     */
    public function table_names($includeViews = true)
    {
        /* query */
        $this->query("SELECT relname FROM pg_class WHERE relkind = 'r' AND NOT relname LIKE 'pg_%' AND NOT relname LIKE 'sql_%'");

        $result = [];
        for ($i = 0; $info = @pg_fetch_row($this->m_query_id, $i); ++$i) {
            $result[$i]['table_name'] = $info[0];
            $result[$i]['tablespace_name'] = $this->m_database;
            $result[$i]['database'] = $this->m_database;
        }

        /* return result */

        return $result;
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
        if ($offset >= 0 && $limit >= 0) {
            $query .= " LIMIT $limit OFFSET $offset";
        }
        Tools::atkdebug('atkpgsqldb.query(): ' . $query);

        /* connect to database */
        if ($this->connect() == self::DB_SUCCESS) {
            /* free old results */
            if (!empty($this->m_query_id)) {
                @pg_free_result($this->m_query_id);
                $this->m_query_id = 0;
            }

            /* query database */
            $error = false;
            $this->m_query_id = @pg_query($this->m_link_id, $query) or $error = true;

            $this->m_row = 0;

            $this->m_error = pg_last_error();
            if ($error) {
                $this->halt("Invalid SQL query: $query");
            }

            if ($this->m_query_id) {
                /* return query id */
                return true;
            }
        }

        return false;
    }

    /**
     * Check if table exists.
     *
     * @param string $table the table to find
     *
     * @return bool true if found, false if not found
     */
    public function tableExists($table)
    {
        $res = $this->getRows("SELECT relname FROM pg_class WHERE relkind = 'r' AND UPPER(relname) = UPPER('" . $table . "')");

        return Tools::count($res) == 0 ? false : true;
    }

    public function commit()
    {
        if ($this->m_link_id) {
            Tools::atkdebug('Commit');
            $this->_query('COMMIT', true);
        }
    }

    public function savepoint($name)
    {
        Tools::atkdebug(get_class($this) . "::savepoint $name");
        $this->_query('SAVEPOINT ' . $name, true);
    }

    public function rollback($savepoint = '')
    {
        if ($this->m_link_id) {
            if (!empty($savepoint)) {
                Tools::atkdebug(get_class($this) . "::rollback (rollback to savepoint $savepoint)");
                $this->_query('ROLLBACK TO SAVEPOINT ' . $savepoint, true);
            } else {
                $this->_query('ROLLBACK', true);
            }
        }
    }


    /**
     * This function indicates what searchmodes the database supports.
     *
     * @return array with search modes
     */
    public function getSearchModes()
    {
        return [
            'exact',
            'substring',
            'wildcard',
            'regexp',
            'greaterthan',
            'greaterthanequal',
            'lessthan',
            'lessthanequal',
            'between'
        ];
    }
}
