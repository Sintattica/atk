<?php

  /**
   * Some defines used for connection statusses, generic error messages, etc.
   */
  define("DB_SUCCESS"           , 0);
  define("DB_UNKNOWNERROR"      , 1);
  define("DB_UNKNOWNHOST"       , 2);
  define("DB_UNKNOWNDATABASE"   , 3);  
  define("DB_ACCESSDENIED_USER" , 4);
  define("DB_ACCESSDENIED_DB"   , 5);


  /**
   * Handles database connectivity and database interaction.
   *
   * @author Peter Verhage <peter@achievo.org>
   * @version $Revision$
   *
   * $Id$
   *
   */ 
  class atkdb
  {
    /* connection parameters */
    var $m_host     = "";
    var $m_database = "";
    var $m_user     = "";
    var $m_password = "";

    /* result array and current row number */
    var $m_record = array();
    var $m_row = 0;

    /*  current error number and error text */
    var $m_errno = 0;
    var $m_error = "";
    var $m_haltonerror = true;
    
    /* Driver name. Must be overwritten by each driver. */
    var $m_type = "";
    
    /** 
     * number of affected rows after an update/delete/insert query 
     * NOTE: this is currently only supported for mysql.
     */
    var $m_affected_rows = 0;      
    
    // array to store meta-information about tables.
    var $m_tableMeta = array();

    /* link and query handles */
    var $m_link_id  = 0;
    var $m_query_id = 0;

    /* set to 1 for automatic freeing of results */    
    var $m_auto_free = 1;
    
    /* set to 1 to set debugging on */    
    var $m_debug = 0; 
    
    var $m_user_error = array();
    var $m_critical_error = array();

    /**
     * Get the current link id
     * @return integer link id
     */
    function link_id()
    {
      return $this->m_link_id;
    }
    
    function getErrorType()
    {
      if(in_array($this->m_errno,$this->m_user_error))
      {
        return "user";   
      }
      return "system";
    }
    
    /*
          * Get atk database error
          */
    function getAtkDbErrno()
    {
      return $this->_translateError();
    }
    
    /*
          * Get real database error number
          */
    function getDbErrno()
    {
      return $this->m_errno;
    }

    /*
          * Get real database error 
          */
    function getDbError()
    {
      return $this->m_error;
    }
    
    /*
          * Set user error number for your own applications.
          * When the user gets this error, it will show the feedback screen
          */
    function setUserError($errno)
    {
      atkdebug("atkdb::setUserError() -> ".$errno);
      $this->m_user_error[]=$errno;
    }
    
    /*
          * Get the translated error message
          */
    function getErrorMsg()
    {
      $errno = $this->getAtkDbErrno(); 
      if($errno==DB_UNKNOWNERROR)
      {
        $errstr = 'db_'.$this->getDbErrno();
        if($GLOBALS[$errstr]=="") { $this->m_error = text('unknown_error').": ".$this->getDbErrno()." (".$this->getDbError().")"; } else { $this->m_error=text($errstr); }
      //  atkerror($this->m_error);

        return $this->m_error;
      }
      else
      {
        $tmp_error='';
        switch ($errno)
        {
          case DB_ACCESSDENIED_DB: $tmp_error = sprintf(text("db_access_denied_database"),$this->m_user,$this->m_database);
                                break;
          case DB_ACCESSDENIED_USER: $tmp_error = sprintf(text("db_access_denied_user"),$this->m_user,$this->m_database);
                                     break;
          case DB_UNKNOWNDATABASE: $tmp_error = sprintf(text("db_unknown_database"),$this->m_database);
                                   break;
          case DB_UNKNOWNHOST: $tmp_error = sprintf(text("db_unknown_host"),$this->m_host);
                               break;
        }
        $this->m_error = $tmp_error;
        return $this->m_error;
      }
    }
    
    function halt()
    {
      if ($this->m_haltonerror)
      {
        if($this->getErrorType()=="system")
        {
          atkdebug("atkdb::halt() on system error");
          if(in_array($this->m_errno,$this->m_critical_error)) $level='critical';
          halt($this->getErrorMsg(),$level);
        }
        else
        {
          atkdebug("atkdb::halt() on user error (not halting)");
        }
      }
    }

    /**
     * Get the current query id
     * @return integer query id
     */
    function query_id()
    {
      return $this->m_query_id;
    }
    
    /**
     * Connect to the database     
     * @return connection identifier
     */
    function connect()
    {
      return $this->m_link_id;
    }

    
    /**
     * If possible, derived db-classes should implement this 
     * function, which can be used to translate database
     * specific errornumbers/errormessages to standardized
     * ATK db error statusses.
     *
     * If a class does not override this function, the default
     * implementation will return DB_UNKNOWNERROR for any 
     * error that occurs. 
     */
    function _translateError($errno)
    {      
      return DB_UNKNOWNERROR;     
    }

    /**
     * Disconnect from database
     */
    function disconnect()
    {
    }

    /**
     * Commit
     */
    function commit()
    {
    }

    /**
     * Rollback
     */
    function rollback()
    {
    }

    /**
     * Performs a query
     * @param $query the query
     * @param $offset offset in record list
     * @param $limit maximum number of records
     * @return true if successful
     */
    function query($query, $offset=-1, $limit=-1)
    {
      return true;    
    }

    /**
     * Goto the next record in the result set
     * @return result of going to the next record
     */
    function next_record()
    {
      return 0;
    }
 
    /**
     * Lock a certain table in the database 
     * @param $table the table name
     * @param $mode the type of locking
     * @return result of locking
     */
    function lock($table, $mode="write")
    {
      return 0;
    }
  
    /**
     * Unlock table(s) in the database 
     * @return result of unlocking
     */
    function unlock()
    {
      return 0;
    }

    /**
     * Evaluate the result; which rows were
     * affected by the query.
     * @return affected rows
     */  
    function affected_rows()
    {
      return array();
    }    

    /**
     * Get the next sequence number
     * of a certain sequence.
     * @param $sequence the sequence name
     * @return the next sequence id
     */
    function nextid($sequence)
    {
    }

    /**
     * Return the meta data of a certain table
     *
     * depending on $full, metadata returns the following values:
     *  -full is false (default):
     *   $result[]:
     *     [0]["table"]  table name
     *     [0]["name"]   field name
     *     [0]["type"]   field type
     *     [0]["len"]    field length
     *     [0]["flags"]  field flags
     * 
     *  -full is true:
     *   $result[]:
     *     ["num_fields"] number of metadata records
     *     [0]["table"]  table name
     *     [0]["name"]   field name
     *     [0]["type"]   field type
     *     [0]["len"]    field length
     *     [0]["flags"]  field flags
     *     ["meta"][field name] index of field named "field name"
     *     The last one is used, if you have a field name, but no index.
     *
     * @param $table the table name
     * @param $full all meta data or not
     * @return array with meta data    
     */
    function metadata($table, $full=false)
    {            
    
      return array();
    }   

    /**
     * Return the available table names
     * @return array with table names etc.
     */
    function table_names()
    {
      return array();
    }
  
   /**
     * This function checks the database for a table with 
     * the provide name
     *
     * @params $tableName the table to find
     * @params returns true if found, false if not found
     */  
    function tableExists($tableName)
    {
      return false;
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

      $this->query($query, $offset, $limit);    
      while ($this->next_record())
        $result[] = $this->m_record;
      
      return $result;
    }
    
    /**
     * This function indicates what searchmodes the database supports.
     * @return array with search modes
     */
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      return array("exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal","between");
    }
    
    function tableMeta($table)
    {
      if (!is_array($this->m_tableMeta[$table]))
      {
        $tmparr = $this->metadata($table);
        for ($i=0, $_i=count($tmparr); $i<$_i; $i++)
        {
          $this->m_tableMeta[$table][$tmparr[$i]['name']]['type'] = $tmparr[$i]['type'];
          $this->m_tableMeta[$table][$tmparr[$i]['name']]['len'] = $tmparr[$i]['len'];
          $this->m_tableMeta[$table][$tmparr[$i]['name']]['flags'] = $tmparr[$i]['flags'];          
        }      
      }
      return $this->m_tableMeta[$table];
    }
    
    /**
      * get NOW() or SYSDATE() equivalent for the current database
      *
      * Every database has it's own implementation to get the current date
      *
      */
    function func_now()
    {
      return "NOW()";
    }
    
    /**
      * get SUBSTRING() equivalent for the current database.
      */
    function func_substring($fieldname, $startat=0, $length=0)
    {
      return "SUBSTRING($fieldname, $startat".($length!=0?", $length":"").")";
    }
    
    /** 
     * Get TO_CHAR() equivalent for the current database.
     *
     * TODO/FIXME: add format paramater. Current format is always yyyy-mm-dd.
     */
    function func_datetochar($fieldname)
    {
      return "TO_CHAR($fieldname, 'YYYY-MM-DD')";
    }
      
      
    /** 
     * Get TO_CHAR() equivalent for the current database.
     *
     * TODO/FIXME: add format paramater. Current format is always yyyy-mm-dd hh:mi.
     */
    function func_datetimetochar($fieldname)
    {
      return "TO_CHAR($fieldname, 'YYYY-MM-DD hh:mi')";
    }      
    
    /**
      * Returns the maximum length an identifier (tablename, columnname, etc) may have
      *      
      */
    function maxIdentifierLength()
    {
      return 64;
    }
       
    /**
     * escapes quotes for use in SQL: ' -> '' (and sometimes % -> %%)
     */
    function escapeSQL($string, $wildcard=false)
    {    
      $result = str_replace("'","''",$string);
      $result = str_replace("\\","\\\\",$result);
      if ($wildcard == true) $result = str_replace("%","%%",$result);
      return $result;
    }

    function &createQuery()
    {      
      $query = &atknew("atk.db.atk".$this->m_type."query");
      return $query;
    }

  }  

?>
