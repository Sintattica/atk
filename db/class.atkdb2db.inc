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
   * @copyright (c)2000-2007 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5.38 $
   * $Id: class.atkdb2db.inc,v 5.38 2007/04/05 13:47:58 harrie Exp $
   */

  /**
   * Driver for IBM DB2 databases.
   *
   * @author Harrie Verveer <harrie@ibuildings.nl>
   * @package atk
   * @subpackage db
   */
  class atkdb2db extends atkDb
  {

    /* sequence table */
    var $m_seq_table = "db_sequence";
    // the field in the seq_table that contains the counter..
    var $m_seq_field = "nextid";
    // the field in the seq_table that countains the name of the sequence..
    var $m_seq_namefield = "seq_name";

    var $m_type = "db2";


    /**
     * Base constructor
     */
    function atkdb2db()
    {
      // set the user error's
      $this->m_type = "db2";
      $this->m_vendor = "db2";
      $this->m_user_error=array();
      
      $this->m_seq_table = atkconfig("db_sequence_table");
      $this->m_seq_field = atkconfig("db_sequence_field");
      $this->m_seq_namefield = atkconfig("db_sequence_namefield");
    }

    /**
     * Connect to the database
     *
     * @param string $host Hostname
     * @param string $user Username
     * @param string $password Password
     * @return mixed Connection status
     */
    function doConnect($host,$user,$password,$database,$port,$charset)
    {
      /* establish connection */
      if (empty($this->m_link_id))
      {
        if (empty($port))
          $port = 50000;

        $options = array("DB2_ATTR_CASE"=>DB2_CASE_LOWER);
        $dbconfig = atkconfig("db");
        $dbconfig = $dbconfig[$this->m_connection];
        if (array_key_exists('i5_lib',$dbconfig) && $dbconfig['i5_lib'] != "")
          $options['i5_lib'] = $dbconfig['i5_lib'];
          
        $this->m_link_id = db2_connect($database,$user,$password,$options);

        if (!$this->m_link_id)
        {
          $this->halt($this->getErrorMsg());
        }
      }

      return DB_SUCCESS;
    }

    /**
     * Translates known database errors to developer-friendly messages
     *
     * @todo Make DB2
     * @return int Flag of the error
     */
    function _translateError()
    {
      $this->_setErrorVariables();
      switch ($this->m_errno)
      {
        case    0: return DB_SUCCESS;
        case 1044: return DB_ACCESSDENIED_DB;
        case 1045: return DB_ACCESSDENIED_USER;
        case 1049: return DB_UNKNOWNDATABASE;
        case 2004:
        case 2005: return DB_UNKNOWNHOST;
        default:
          atkdebug("mysqldb::translateError -> MySQL Error: ".
                    $this->m_errno." -> ".$this->m_error);
          return DB_UNKNOWNERROR;
      }
    }

    /**
     * Store MySQL errors in internal variables
     * @access private
     */
    function _setErrorVariables()
    {
      if (!empty($this->m_link_id))
      {
        
        $this->m_errno = db2_conn_errormsg($this->m_link_id);
        $this->m_error = db2_conn_error($this->m_link_id);
      }
      else
      {
        $this->m_errno = db2_conn_errormsg();
        $this->m_error = db2_conn_error();
      }
    }

    /**
     * Disconnect from database
     */
    function disconnect()
    {
      if ($this->m_link_id)
      {
        atkdebug("Disconnecting from database...");
        @db2_close($this->m_link_id);
        $this->m_link_id = 0;
      }
    }

    /**
     * Performs a query
     * @param $query the query
     * @param $offset offset in record list
     * @param $limit maximum number of records
     * 
     * @todo Convert to DB2
     */
    function query($query, $offset=-1, $limit=-1)
    {
      atkimport("atk.utils.atkdebugger");
      atkdebug("In query dinges");
      atkdebug("Query:".$query);
      atkdebug("Query added to debugger...");

      atkdebug("atkdb2db::query -> connect with mode = $mode");

      /* connect to database */
      if ($this->connect($mode)==DB_SUCCESS)
      {
        /* free old results */
        if ($this->m_query_id)
        {
          if (is_resource($this->m_query_id))
            db2_free_result($this->m_query_id);
          $this->m_query_id = 0;
        }

        $this->m_affected_rows = 0;

        /* query database */
        $this->m_query_id = @db2_exec($this->m_link_id,$query,array("cursor"=>DB2_SCROLLABLE));

        /* invalid query */
        if (!$this->m_query_id)
        {
          $this->_setErrorVariables();
          $this->halt("Invalid SQL: $query.");
          return false;
        }
        
        $this->m_affected_rows = db2_num_rows($this->m_query_id);
        $this->m_row = 0;

        /* return query id */
        return true;
      }
      return false;
    }
    
        /**
     * Goto the next record in the result set
     * @return result of going to the next record
     */
    function next_record($row_number = null)
    {
      /* goto next record */
      $this->m_record = @db2_fetch_assoc($this->m_query_id,$row_number);
      
      if (isset($this->m_record) && is_array($this->m_record))
      {
        foreach($this->m_record as $key => $value)
        {
          if (is_string($value))
            $this->m_record[$key] = trim($value);
        }
      }
      
      $this->m_row++;
      $this->m_errno = db2_conn_error($this->m_link_id);
      $this->m_error = db2_conn_errormsg($this->m_link_id);

      /* are we there? */
      $result = is_array($this->m_record);
      if (!$result && $this->m_auto_free)
      {
        @db2_free_result($this->m_query_id);
        $this->m_query_id = 0;
      }
      
      /* return result */
      return $result;
    }

    /**
     * Goto a certain position in result set.
     * Not specifying a position will set the pointer
     * at the beginning of the result set.
     * @param $position the position
     */
    function seek($position=0)
    {
      atkerror("Seek not (yet) implemented for db2 driver");
    }

    /**
     * Lock a certain table in the database
     * @param $table the table name
     * @param $mode the type of locking
     * @return result of locking
     */
    function lock($table, $mode="write")
    {
      // we don't lock/unlock
      return true;
      
      
      /* connect first */
      if ($this->connect("w")==DB_SUCCESS)
      {
        $query = "LOCK TABLE $table IN " . ($mode == "write" ? "EXCLUSIVE" : "SHARE") . " MODE";

        atkimport("atk.utils.atkdebugger");
        atkDebugger::addQuery($query);

        /* lock */
        $result = @db2_exec($this->m_link_id,$query);
        if (!$result) $this->halt("$mode lock on $table failed.");

        /* return result */
        return $result;
      }
      return 0;
    }

    /**
     * Unlock table(s) in the database
     * @return result of unlocking
     */
    function unlock()
    {
      // we don't lock/unlock
      return true;
      
      
      /* connect first */
      if ($this->connect("w")==DB_SUCCESS)
      {
        // In DB2, the table will remain locked until the next commit or rollback. As
        // we never use rollback in ATK, lets just commit() to release all locks
        $result = db2_exec($this->m_link_id,"COMMIT");
        if (!$result) $this->halt("unlock tables failed.");

        /* return result */
        return $result;
      }
      return 0;
    }

    /**
     * Evaluate the result; which rows were
     * affected by the query.
     * @return affected rows
     */
    function affected_rows()
    {
      return @db2_num_rows($this->m_query_id);
    }

    /**
     * Evaluate the result; how many rows
     * were affected by the query.
     * @return number of affected rows
     */
    function num_rows()
    {
      return @db2_num_rows($this->m_query_id);
    }

    /**
     * Evaluatie the result; how many fields
     * where affected by the query.
     * @return number of affected fields
     */
    function num_fields()
    {
      return @db2_num_fields($this->m_query_id);
    }

    /**
     * Get the next sequence number
     * of a certain sequence.
     * @param $sequence the sequence name
     * @return the next sequence id
     */
    function nextid($sequence)
    {
      /* first connect */
      if ($this->connect("w")==DB_SUCCESS)
      {
        /* lock sequence table */
        if ($this->lock($this->m_seq_table))
        {
          /* get sequence number (locked) and increment */
          $query = "SELECT ".$this->m_seq_field." FROM ".$this->m_seq_table." WHERE ".$this->m_seq_namefield." = '$sequence'";

          $id = @db2_exec($this->m_link_id,$query);
          $result = @db2_fetch_assoc($id);
          
          /* no current value, make one */
          if (!is_array($result))
          {
            $query = "INSERT INTO ".$this->m_seq_table." (".$this->m_seq_namefield.",".$this->m_seq_field.") VALUES('$sequence', 1)";
            $id = @db2_exec($this->m_link_id,$query);
            $this->unlock();
            return 1;
          }

          /* enter next value */
          else
          {
            $nextid = $result[$this->m_seq_field] + 1;
            $query = "UPDATE ".$this->m_seq_table." SET ".$this->m_seq_field." = '$nextid' WHERE ".$this->m_seq_namefield." = '$sequence'";
            $id = @db2_exec($this->m_link_id,$query);
            $this->unlock();
            return $nextid;
          }
        }
        return 0;
      }

      /* cannot lock */
      else
      {
        $this->halt("cannot lock ".$this->m_seq_table." - has it been created?");
      }
    }

    /**
     * Returns the table type.
     *
     * @param string $table table name
     * @return string table type
     */
    function _getTableType($table)
    {
      atkerror("_getTableType not implemented yet for db2 database");
    }

    /**
     * Return the meta data of a certain table
     * @param String $table the table name (optionally in 'database.tablename' format)
     * @param boolean $full all meta data or not
     * @return array with meta data
     */
    function metadata($table, $full=false)
    {
      $tbl = strtoupper($table);
      $result = array();
      
      if ($this->connect()==DB_SUCCESS)
      {
        $ddl = &atkDDL::create("db2");
        $query = "SELECT * FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = '$tbl'";
        $dbconfig = atkconfig("db");
        $dbconfig = $dbconfig[$this->m_connection];
        if (array_key_exists('i5_lib',$dbconfig) && $dbconfig['i5_lib'] != "")
          $query .= " AND TABLE_SCHEMA = '{$dbconfig['i5_lib']}'";
        $result_metadata = $this->getRows($query);
        
        for ($i=0,$_i=count($result_metadata);$i<$_i;$i++)
        {
          $result[$i]["table"]      = $result_metadata[$i]['table_schema'] . "." . $result_metadata[$i]['table_name'];
          $result[$i]["table_type"] = NULL;
          $result[$i]["name"]       = strtolower($result_metadata[$i]['column_name']);
          $result[$i]["type"]       = $result_metadata[$i]['data_type'];
          $result[$i]["gentype"]    = $ddl->getGenericType($result_metadata[$i]['data_type']);
          $result[$i]["len"]        = $result_metadata[$i]['character_maximum_length'];
          $result[$i]["flags"]      = $this->metadataToFlags($result_metadata[$i]);

          $result[$i]["flags"] =
            (in_array('primary_key',    $result[$i]["flags"]) ? MF_PRIMARY        : 0) |
            (in_array('unique_key',     $result[$i]["flags"]) ? MF_UNIQUE         : 0) |
            (in_array('not_null',       $result[$i]["flags"]) ? MF_NOT_NULL       : 0) |
            (in_array('auto_increment', $result[$i]["flags"]) ? MF_AUTO_INCREMENT : 0);

          if ($full)
            $result["meta"][$result[$i]["name"]] = $i;
        }
      }
      
      return $result;
    }
    
    /**
     * We need to reconstruct the flags array ourselves based on the fields
     * we obtained from INFORMATION_SCHEMA.COLUMNS. Get properties from the
     * row provided and return an array based on these properties.
     *
     * @param array $metadata The row as obtained from INFORMATION_SCHEMA.COLUMNS
     * @return array Flags to use in the metadata we return
     */
    function metadataToFlags($metadata)
    {
      $ret = array();
      $ret[] = $metadata['is_nullable'] == "NO" ? "not_null" : "null";
      
      // todo: auto_increment
      // todo: primary_key
      // todo: unique_key
      return $ret;
    }

    /**
     * Return the available table names
     * @return array with table names etc.
     */
    function table_names()
    {
      atkerror("table_names for atkdb2db not implemented yet!");
      /* query */
      $this->query("SHOW TABLES"); // not sure if this is the right syntax for db2?

      /* get table names */
      $result = array();
      for ($i=0; $info = db2_fetch_row($this->m_query_id); $i++)
      {
        $result[$i]["table_name"]      = $info[0];
        $result[$i]["tablespace_name"] = $this->m_database;
        $result[$i]["database"]        = $this->m_database;
      }

      /* return result */
      return $result;
    }

    /**
     * This function checks the database for a table with
     * the provide name
     *
     * @param String $tableName the table to find
     * @return boolean true if found, false if not found
     */
    function tableExists($tablename)
    {
      $query = "SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = '$tablename'";
      $dbconfig = atkconfig("db");
      $dbconfig = $dbconfig[$this->m_connection];
      if (array_key_exists('i5_lib',$dbconfig) && $dbconfig['i5_lib'] != "")
        $query .= " AND TABLE_SCHEMA = '{$dbconfig['i5_lib']}'";
          
      $res = $this->getrows($query);
      return (count($res)==0 ? false : true);
    }

    /**
     * This function indicates what searchmodes the database supports.
     * @return array with search modes
     */
    function getSearchModes()
    {
      return array("exact","substring","wildcard","regexp","soundex","greaterthan","greaterthanequal","lessthan","lessthanequal","between");
    }

     /**
     * Get TO_CHAR() equivalent for the current database.
     * Each database driver should override this method to perform vendor
     * specific conversion.
     *
     * @param String $fieldname The field to generate the to_char for.
     * @param String $format Format specifier. The format is compatible with
     *                       php's date() function (http://www.php.net/date)
     *                       The default is what's specified by
     *                       $config_date_to_char, or "Y-m-d" if not
     *                       set in the configuration.
     * @return String Piece of sql query that converts a date field to char
     *                for the current database
     * 
     * @todo make me db2
     */
    function func_datetochar($fieldname, $format="")
    {
      return "TO_CHAR($fieldname, 'YYYY-MM-DD')";
    }

    /**
     * Convert a php date() format specifier to a mysql specific format
     * specifier.
     *
     * Note that currently, only the common specifiers Y, m, d, H, h, i and
     * s are supported.
     * @param String $format Format specifier. The format is compatible with
     *                       php's date() function (http://www.php.net/date)
     * @return String Mysql specific format specifier.
     * 
     * @todo make me db2
     */
    function vendorDateFormat($format)
    {
      $php_fmt = array("Y", "m", "d", "H", "h", "i", "s");
      $db_fmt  = array("%Y", "%m", "%d", "%H", "%h", "%i", "%s");
      return str_replace($php_fmt, $db_fmt, $format);
    }


    /**
     * Get TO_CHAR() equivalent for the current database.
     *
     * TODO/FIXME: add format paramater. Current format is always yyyy-mm-dd hh:mi.
     * 
     * @todo make me db2
     */
    function func_datetimetochar($fieldname)
    {
      return "TO_CHAR($fieldname, 'YYYY-MM-DD HH24:MI:SS')";
    }

    /**
     * Set database sequence value.
     *
     * @param string $seqname sequence name
     * @param int $value sequence value
     */
    function setSequenceValue($seqname, $value)
    {
      $query = "UPDATE ".$this->m_seq_table." SET ".$this->m_seq_field." = '$value' WHERE ".$this->m_seq_namefield." = '$seqname'";
      $this->query($query);
    }
    
   /**
     * Get all rows that are the result
     * of a certain specified query
     *
     * Note: This is not an efficient way to retrieve
     * records, as this will load all records into one
     * array into memory. If you retrieve a lot of records,
     * you might hit the memory_limit and your script will die.
     *
     * @param $query the query
     * @return array with rows
     */
    function getrows($query, $offset=-1, $limit=-1)
    {
      $result = array();

      // IBM DB2 doesn't support LIMIT syntax as MySQL and PostgreSQL do.
      // We have to seek through the results to go to a certain offset 
      // and retrieve only the number of rows specified in the limit. 
      // In class.atkdb2query.inc we add a fake LIMIT clause which we
      // remove here and transform to values for the offset and limit
      // parameters.
      if ($offset == -1 && $limit == -1)
      {
        $lines = explode("\n", $query);
        if (count($lines) > 0 && strpos($lines[count($lines) -1], 'LIMIT') === 0)
        {
          $last = array_pop($lines);
          $query = implode("\n", $lines);
          if (preg_match('/LIMIT ([0-9]+) OFFSET ([0-9]+)/', $last, $matches))
          {
            $limit = $matches[1];
            $offset = $matches[2];
          }
        }
      }
      
      $replaceEbcdic = $this->getEbcdicFields($query);
      $this->query($query);

      if ($limit > 0)
      {
        for ($i = 1; $i <= $limit; $i++)
        {
          if (!$this->next_record($offset+$i)) break;
          $result[] = $this->m_record;
        }
      }
      else
      {
        while ($this->next_record())
          $result[] = $this->m_record;
      }

      if (is_array($result) && count($result) > 0 && is_array($replaceEbcdic) && count($replaceEbcdic) > 0)
      {
        for($i=0,$_i=count($result);$i<$_i;$i++)
        {
          foreach($replaceEbcdic as $colname)
            $result[$i][$colname] = ebcdic2ascii($result[$i][$colname]);
        }
      }
      
      return $result;
    }
    
    /**
     * Sometimes, when certain database functions are used, DB2 returns EBCDIC
     * instead of ASCII. Figure out what fields need to get a special treatment.
     *
     * @param string $query The query we want to execute
     * @return Array An array with fieldnames that will be returned in EBCDIC
     */
    function getEbcdicFields($query)
    {
      // functions that cause DB2 to return EBCDIC (escaped for regex usage)
      $functions = array("to_char","concat");
      $matches = array();
      $regex = "/(".implode("|",$functions).")\(.*\) as (al_.+)/Uis";
      preg_match_all($regex,$query,$matches);
      if (!is_array($matches) || count($matches) < 3)
        return array();
      else 
        return $matches[2];
    }
  }
  
  /**
   * Workaround until the official PHP-version of ebcdic2ascii is released. For
   * release noted see: http://www.php.net/manual/en/function.ebcdic2ascii.php
   */
  if (!function_exists("ebcdic2ascii"))
  {
    function ebcdic2ascii($input)
    {
      static $translate_mapping = array(
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
      0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
      0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
      0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
      0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
      0x2E, 0x2E, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
      0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x2E, 0x3F,
      0x20, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
      0x2E, 0x2E, 0x2E, 0x3C, 0x28, 0x2B, 0x7C,
      0x26, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
      0x2E, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
      0x2D, 0x2F, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
      0x2E, 0x7C, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
      0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
      0x2E, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
      0x2E, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
      0x69, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
      0x2E, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
      0x72, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
      0x2E, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
      0x7A, 0x2E, 0x2E, 0x2E, 0x5B, 0x2E, 0x2E,
      0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
      0x2E, 0x2E, 0x2E, 0x2E, 0x5D, 0x2E, 0x2E,
      0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
      0x49, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
      0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51,
      0x52, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
      0x5C, 0x2E, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
      0x5A, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
      0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
      0x39, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E);
  
      $output = "";     
      for($i=0,$_i=strlen($input);$i<$_i;$i++)
        $output .= chr($translate_mapping[ord($input[$i])]);
      
      return $output;
    }
  }
  
?>