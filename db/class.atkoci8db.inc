<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage db
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Driver for Oracle 8i databases.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */
  class atkOci8Db extends atkDb
  {
    /**
     * @var String
     * @access private
     */
    var $m_type = "oci8";
    var $m_vendor = "oracle";

    /**
     * Default constructor
     */
    function atkOci8Db()
    {
      // set the user error's
      $this->m_user_error=array(1,1400,1407,1438,2290,2291,2292,2293);
      // Other errors are assumed to be critical errors
    }

    /**
     * Connect to the database.
     * @return int One of the generic DB_ connection statusses.
     */
    function connect()
    {
      /* establish connection to database */
      if (empty($this->m_link_id))
      {
        $connectfunc = (atkconfig("databasepersistent")?"OCIPlogon":"OCILogon");
        atkdebug("Connectiontype: ".$connectfunc);
        $this->m_link_id = $connectfunc($this->m_user, $this->m_password, $this->m_database);
        if (!$this->m_link_id)
        {
          $error = OCIError(); // can't pass link_id because it is null if connect failed.
          atkdebug("CONNECT ERROR");
          $this->halt($this->_translateError($error));
        }

        /* set default date format for Oracle */
        $nls = OCIParse($this->m_link_id, "alter session set nls_date_format='YYYY-MM-DD HH24:MI:SS'");
        $myresult = OCIExecute($nls);
        $error = OCIError($this->m_link_id);
        if ($error["code"]!="")
        {
          $this->halt("Invalid SQL: $query");
          return $this->_translateError($error);
        }
        
        /* set default timestamp format for Oracle */
        $nls = OCIParse($this->m_link_id, "alter session set nls_timestamp_format='YYYY-MM-DD HH24:MI:SS'");
        $myresult = OCIExecute($nls);
        $error = OCIError($this->m_link_id);
        if ($error["code"]!="")
        {
          $this->halt("Invalid SQL: $query");
          return $this->_translateError($error);
        }
      }
      return DB_SUCCESS;
    }

    /**
     * Get last error code.
     *
     * If an error occurred, this method can be called to retrieve
     * the generic error code.
     * @return int One of the generic DB_* statusses.
     */
    function getAtkDbErrno()
    {
      atkdebug("atkoci8db::getAtkDbErrno() -> ".$this->getDbErrno());
      return $this->_translateError(array('code'=>$this->getDbErrno(),'error'=>$this->getDbError()));
    }

    /**
     * Translate a driver specific error to a generic ATK db error code.
     * @access private
     * @return int One of the generic DB_* constants
     */
    function _translateError($err)
    {
      $this->m_errno = $err['code'];
      $this->m_error = $err['message'];
      atkdebug('atkoci8db::_translateError() -> DB Error code: '.$err['code']);
      switch ($this->m_errno)
      {
        case     0: return DB_SUCCESS;
        case  1017: return DB_ACCESSDENIED_USER;
        case 12154: return DB_UNKNOWNDATABASE;
        default:
                    return DB_UNKNOWNERROR;
      }
    }

    /**
     * Disconnect from database.
     */
    function disconnect()
    {
      if($this->m_link_id)
      {
        atkdebug("Disconnecting from database...");
        OCILogoff($this->m_link_id);
      }
    }
    
    /**
     * Enable/disable all foreign key constraints.
     * 
     * @param boolean $enable enable/disable foreign keys?
     */
    function toggleForeignKeys($enable)
    {
      $query = "SELECT fk.constraint_name, fk.table_name FROM user_constraints fk WHERE fk.constraint_type = 'R'";
      $keys = $this->getRows($query);
      
      $state = $enable ? "ENABLE" : "DISABLE";
      
      foreach ($keys as $key)
      {
        $this->query("ALTER TABLE {$key[table_name]} {$state} CONSTRAINT {$key[constraint_name]}");
      }      
    }    

    /**
     * Commit the current transaction.
     */
    function commit()
    {
      atkdebug(get_class($this)."::commit");            
      if($this->m_link_id)
      {
        OCICommit($this->m_link_id);
      }
      return true;
    }

    /**
     * Set savepoint with the given name.
     * 
     * @param $name savepoint name
     * @abstract
     */
    function savepoint($name)
    {
      atkdebug(get_class($this)."::savepoint $name");      
      $this->query('SAVEPOINT '.$name);
    }

    /*
     * Rollback the the current transaction.
     */
    function rollback($savepoint="")
    {
      atkdebug(get_class($this)."::rollback $savepoint");      
      
      if (!empty($savepoint))
      {
        $this->query('ROLLBACK TO SAVEPOINT '.$savepoint);        
      }
      else if($this->m_link_id)
      {
        OCIRollback($this->m_link_id);
      }
      return true;
    }

    /**
     * Parse and execute a query.
     *
     * If the query is a select query, the rows can be retrieved using the
     * next_record() method.
     *
     * @param String $query The SQL query to execute
     * @param int $offset Retrieve the results starting at the specified
     *                    record number. Pass -1 or 0 to start at the first
     *                    record.
     * @param int $limit Indicates how many rows to retrieve. Pass -1 to
     *                   retrieve all rows.
     */
    function query($query, $offset=-1, $limit=-1)
    {
      /* limit? */
      if ($offset >= 0 && $limit > 0)
      {
        /* row id's start at 1! */
	      $query="SELECT * FROM (SELECT rownum AS rid, XX.* FROM (".
        $query.") XX) YY  WHERE YY.rid >= ".($offset+1)." AND YY.rid <= ".($offset + $limit);
      }

      /* query */
      atkdebug("atkoci8db.query(): ".$query);

      /* connect to database */
      if ($this->connect()==DB_SUCCESS)
      {
        /* free old results */
        if ($this->m_query_id)
        {
          @OCIFreeStatement($this->m_query_id);
          $this->m_query_id = 0;
        }

        /* query database */
        $execresult = false;
        $this->m_query_id = @OCIParse($this->m_link_id, $query);

        if ($this->m_query_id)
        {
          if(!@OCIExecute($this->m_query_id,OCI_DEFAULT))
          {
            $error = OCIError($this->m_query_id);
            atkerror("Invalid SQL: $query");
            $this->halt($this->_translateError($error));
            return false;
          }
        }
        else
        {
          /* error in query */
          $error = OCIError($this->m_link_id);
          atkerror("Invalid SQL: $query");
          $this->halt($this->_translateError($error));
          return false;
        }

        $this->m_row = 0;

        /* return query id */
        return true;
      }
      return false;
    }

    /**
     * Retrieve the next record in the resultset.
     * @return mixed An array containing the record, or 0 if there are no more
     *               records to retrieve.
     */
    function next_record()
    {
      /* goto next record */
      $this->m_record = array();

      /* get record data */
      if (@OCIFetchInto($this->m_query_id, $result, OCI_ASSOC|OCI_RETURN_NULLS|OCI_RETURN_LOBS))
      {
        for ($i = 1; $i <= OCINumcols($this->m_query_id); $i++)
        {
          $column = strtoupper(OCIColumnname($this->m_query_id, $i));
          $column_return = strtolower($column);
          $this->m_record[$column_return] = $result[$column];
        }
        $this->m_row++;

        return 1;
      }

      /* error */
      else
      {
        $error = OCIError($this->m_link_id);
        $this->m_errno = $error["code"];
        $this->m_error = $error["message"];

        /* no more records, no problem */
        if(1403 == $this->m_errno)
        {
          $this->m_errno = 0;
          $this->m_error = "";
          $this->disconnect();
        }

        return 0;
      }
    }

    /**
     * Skip to a certain position in the result set for subsequent calls to
     * next_record().
     *
     * Calling seek() with no parameters places the pointer to the first
     * record in the resultset.
     *
     * @param int $position The number of the record to skip to.
     */
    function seek($position=0)
    {
      if ($this->num_rows() > $position) $this->m_row = $position;
      else $this->halt("seek($position) failed: result has ".$this->num_rows()." rows");
    }

    /**
     * Lock a table in the database.
     *
     * @param String $table The name of the table to lock.
     * @param String $mode The lock type. Note that this driver only supports
     *                     "write".
     *
     * @return boolean True if succesful, false if not.
     */
    function lock($table, $mode="write")
    {
      /* connect first */
      if ($this->connect()==DB_SUCCESS)
      {

        /* lock */
        if ($mode == "write")
        {
          $m_query_id = OCIParse($this->m_link_id, "lock table $table in row exclusive mode");
          $result = @OCIExecute($m_query_id);
          if (!$result)
          {
            $this->halt("$mode lock on $table failed.");
            return 0;
          }
        }

        /* return result */
        return 1;
      }
      return 0;
    }

    /**
     * Relieve all locks.
     *
     * The oracle 8i driver has no way to unlock other than committing the
     * current transaction. So be careful with calling this function; any
     * outstanding transaction will be committed.
     *
     * @return boolean True if succesful, false if not.
     */
    function unlock()
    {
      return $this->query("commit");
    }

    /**
     * Retrieve the number of rows affected by the last query.
     *
     * After calling query() to perform an update statement, this method will
     * return the number of rows that was updated.
     *
     * @return int The number of affected rows
     */
    function affected_rows()
    {
      return $this->num_rows();
    }

    /**
     * Retrieve the number of rows in the resultset.
     *
     * After calling query() to perform a select statement, this method will
     * return the number of rows in the resultset.
     *
     * @return int The number of rows in the resultset.
     */
    function num_rows()
    {
      return @OCIRowCount($this->m_query_id);
    }

    /**
     * Evaluatie the result; how many fields
     * where affected by the query.
     * @return number of affected fields
     */
    function num_fields()
    {
      return @OCINumCols($this->m_query_idD);
    }


    /**
     * Get the next sequence number of a certain sequence.
     *
     * If the sequence does not exist, it is created automatically. For this
     * to work, the database user should have the 'create sequence' privilege.
     * The name of the sequence in the database is prefixed with
     * the string that's configured in $config_database_sequenceprefix.
     *
     * <b>Example</b>
     * If the database contains a sequence named "seq_employees", the following
     * call will retrieve the next value from the sequence:
     * <code>
     *   $id = $db->nextid("employees");
     * </code>
     *
     * @param $sequence The sequence name
     * @return int The next sequence value
     */
    function nextid($sequence)
    {
      /* connect first */
      if ($this->connect()==DB_SUCCESS)
      {
        $sequencename = atkconfig("database_sequenceprefix").$sequence;

        /* get sequence number and increment */
        $query = "SELECT ".$sequencename.".nextval AS nextid FROM dual";

        /* execute query */
        $m_query_id = OCIParse($this->m_link_id, $query);
        $id = @OCIExecute($m_query_id,OCI_COMMIT_ON_SUCCESS);

        /* error? */
        if (empty($id))
        {
          /* create sequence */
          $query = "CREATE SEQUENCE ".$sequencename." INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER";
          $m_query_id = OCIParse($this->m_link_id, $query);
          $id = @OCIExecute($m_query_id,OCI_COMMIT_ON_SUCCESS) or $this->halt("cannot create '$sequencename'");

          /* try again */
          $query = "SELECT ".$sequencename.".nextval AS nextid FROM dual";
          $m_query_id = OCIParse($this->m_link_id, $query);
          $id = @OCIExecute($m_query_id) or $this->halt("cannot get nextval from sequence '$sequencename'");

          /* empty? */
          if (empty($id)) return 0;
        }

        /* get nextid */
        OCIFetchInto($m_query_id, $result);

        /* return id */
        return $result[0];
      }
      return 0;
    }
    
    /**
     * Set sequence value.
     *
     * @param string $sequence
     * @param int $value
     */
    function setSequenceValue($sequence, $value)
    {
      /* connect first */
      if ($this->connect()==DB_SUCCESS)
      {
        $sequence = atkconfig("database_sequenceprefix").$sequence;      
        $query = "SELECT {$sequence}.NEXTVAL AS value FROM DUAL";
        atkdebug($query);
        list($row) = $this->getRows($query);
        $diff = $value - $row['value'];
        if ($diff > 0)
        {
          $query = "ALTER SEQUENCE {$sequence} INCREMENT BY {$diff}";
          atkdebug($query);    
          $queryId = OCIParse($this->m_link_id, $query);
          @OCIExecute($queryId) or $this->halt("Cannot set sequence value for sequence '$sequence'");
          $query = "SELECT {$sequence}.NEXTVAL AS value FROM DUAL";
          atkdebug($query);          
          $queryId = OCIParse($this->m_link_id, $query);
          @OCIExecute($queryId) or $this->halt("Cannot set sequence value for sequence '$sequence'");
          $query = "ALTER SEQUENCE {$sequence} INCREMENT BY 1";
          atkdebug($query);          
          $queryId = OCIParse($this->m_link_id, $query);
          @OCIExecute($queryId) or $this->halt("Cannot set sequence value for sequence '$sequence'");
        }
      }
    }

    /**
     * Return the meta data of a certain table
     *
     * @param String $table The name of the table
     * @param boolean $full If set to true, more metadata is retrieved than
     *                      when set to false.
     * @return array The retrieved metadata. See documentation for
     *               atkDb::metadata() for information about the structure
     *               of the returned array.
     */
    function metadata($table, $full=false)
    {
      atkimport("atk.db.atkddl");
      
      $ddl = &$this->createDDL();

      // retrieve meta data
      $sql = "
        SELECT
          LOWER(c.column_name) AS name,
          c.data_type AS type,
          c.data_length AS length,
          c.data_default AS default_value,
          (CASE WHEN c.nullable = 'Y' THEN 0 ELSE 1 END) AS is_not_null,
          (
            SELECT COUNT(1)
            FROM user_constraints uc
            JOIN user_cons_columns ucc ON (ucc.constraint_name = uc.constraint_name)
            WHERE uc.table_name = c.table_name
            AND ucc.column_name = c.column_name
            AND uc.constraint_type = 'P'
            AND ROWNUM = 1
          ) AS is_primary,
          (
            SELECT (CASE WHEN COUNT(1) = 1 THEN 1 ELSE 0 END)
            FROM user_constraints uc
            JOIN user_cons_columns ucc ON (ucc.constraint_name = uc.constraint_name)
            WHERE uc.table_name = c.table_name
            AND ucc.column_name = c.column_name
            AND uc.constraint_type = 'U'
            AND NOT EXISTS (
              SELECT 1
              FROM user_cons_columns ucc1 
              WHERE ucc1.constraint_name = uc.constraint_name
              AND ucc1.column_name <> ucc.column_name
            )
          ) AS is_unique,
          (
            SELECT COUNT(1)
            FROM user_objects o
            WHERE o.object_type = 'SEQUENCE'
            AND UPPER(o.object_name) IN (UPPER('seq_' || c.table_name), UPPER(c.table_name || '_seq'))
            AND ROWNUM = 1
          ) AS is_auto_inc,
          (
            SELECT o.object_name
            FROM user_objects o
            WHERE o.object_type = 'SEQUENCE'
            AND UPPER(o.object_name) IN (UPPER('seq_' || c.table_name), UPPER(c.table_name || '_seq'))
            AND ROWNUM = 1
          ) AS sequence
         FROM user_tab_cols c
         WHERE c.table_name = UPPER('$table')
         AND c.hidden_column <> 'YES'
         ORDER BY c.column_id";

      $meta = array();
      $rows = $this->getRows($sql);

      foreach ($rows as $i => $row)
      {
        $meta[$i]['table']   = $table;
        $meta[$i]['type']    = $row['type'];
        $meta[$i]['gentype'] = $ddl->getGenericType($row['type']);
        $meta[$i]['name']    = $row['name'];
        $meta[$i]['len']     = $row['length'];
        $meta[$i]['flags']   =
          ($row['is_primary'] == 1 ? MF_PRIMARY : 0) |
          ($row['is_unique'] == 1 ? MF_UNIQUE : 0) |
          ($row['is_not_null'] == 1 ? MF_NOT_NULL : 0) |
          ($row['is_primary'] == 1 && $row['is_auto_inc'] == 1 ? MF_AUTO_INCREMENT : 0);

        if ($row['is_primary'] == 1 && $row['is_auto_inc'] == 1)
          $meta[$i]['sequence'] = $row['sequence'];
        else if ($row['default_value'] !== NULL)
        {
          if (in_array($meta[$i]['gentype'], array('string', 'text')))
            $meta[$i]['default'] = substr(substr($row['default_value'], 1), 0, -1);
          else if (in_array($meta[$i]['gentype'], array('number', 'decimal')))
            $meta[$i]['default'] = $row['default_value'];
        }

        if ($full)
          $meta["meta"][$row['name']] = &$meta[$i];
      }

      if ($full)
        $meta['num_fields'] = count($rows);

      return $meta;
    }

    /**
     * Return a list of all table names in the current connection.
     *
     * @return array The array containing the names of the tables.
     */
    function table_names()
    {
      /* query */
      $this->query("SELECT LOWER(table_name) AS table_name, LOWER(tablespace_name) AS tablespace_name FROM user_tables");

      /* get table names */
      $result = array();
      for ($i=0; $this->next_record(); $i++)
      {
        $result[$i]["table_name"]      = $this->m_record["table_name"];
        $result[$i]["tablespace_name"] = $this->m_record["tablespace_name"];
        $result[$i]["database"]        = $this->m_database;
      }

      /* return result */
      return $result;
    }

    /**
     * This function checks in the database if a table exists.
     *
     * @param String $tableName The name of the table to check.
     * @return boolean True if the table exists, false if not.
     */
    function tableExists($table)
    {
      $found = $this->getrows("SELECT count(*) as found FROM cat WHERE table_name='".strtoupper($table)."'");
      return ($found[0]['found']==0 ? false : true);
    }

    /**
     * Retrieve this drivers' specific SQL implementation of a now() function.
     * @return String
     */
    function func_now()
    {
      return "SYSDATE";
    }

    /**
      * Retrieve this drivers' specific SQL implementation of a substring
      * function.
      *
      * Note that this function does not actually perform a substring
      * operation. It just returns a piece of valid SQL code that can be used
      * in a query.
      *
      * @param String $fieldname The name of the field to perform a substring
      *                          operation on.
      * @param int $startat The character at which to start.
      * @param int $length The number of charachters to retrieve.
      */
    function func_substring($fieldname, $startat=0, $length=0)
    {
      return "SUBSTR($fieldname, $startat".($length!=0?", $length":"").")";
    }

    /**
      * Returns the maximum length an identifier (tablename, columnname, etc)
      * for this driver may have.
      * @return int
      */
    function maxIdentifierLength()
    {
      return 30;
    }

    /**
     * Escape a string for use in SQL queries.
     *
     * This driver's implementation escapes single quotes and optionally,
     * wildcards. Wildcards should be escaped if they appear as regular
     * characters inside a string. When actually searching for wildcards,
     * those wildcards should not be escaped.
     *
     * @param String $string The string to escape.
     * @param boolean $wildcard Set to true to convert wildcard chars ('%').
     *                          False will leave them unescaped.
     * @return String A SQL compatible version of the input string.

     */
    function escapeSQL($string, $wildcard=false)
    {
      $result = str_replace("'","''",$string);
      if ($wildcard == true) $result = str_replace("%","%%",$result);
      return $result;
    }

  }

?>
