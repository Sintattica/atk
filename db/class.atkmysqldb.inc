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
   * @internal Include baseclass
   */
  require_once(atkconfig("atkroot")."atk/db/class.atkdb.inc");
  //Mysql errors includen
  include_once(atkconfig("atkroot")."atk/db/languages/mysql_".atkconfig('language').'.lng');

  /**
   * Driver for MySQL databases.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage db
   */ 
  class atkMysqlDb extends atkDb
  {    
  
    /* sequence table */
    var $m_seq_table = "db_sequence";  
    // the field in the seq_table that contains the counter..
    var $m_seq_field = "nextid";
    // the field in the seq_table that countains the name of the sequence..
    var $m_seq_namefield = "seq_name";
    
    var $m_type = "mysql";
    
     
    /**
     * Base constructor 
     */
    function atkmysqldb()
    {
      // set the user error's
      $this->m_user_error=array();   
      // set critical error's      
      $this->m_critical_error = array(1044,1045,1049,1054,1146,1130,2002,2005);
    }
  
    /**
     * Connect to the database     
     * @return connection status
     */
    function connect()
    {          
      /* establish connection */
      if (empty($this->m_link_id))
      {
        $this->m_link_id = @mysql_pconnect($this->m_host, $this->m_user, $this->m_password);
        if (!$this->m_link_id) 
        {
          $this->halt($this->getErrorMsg());
        }
      }

      /* select database */
      if (!@mysql_select_db($this->m_database, $this->m_link_id))
      {
        $this->halt($this->getErrorMsg());
        return $this->_translateError();
      }
        
      /* return link identifier */
      return DB_SUCCESS;
    }
    
    function _translateError()
    { 
      $this->m_errno = mysql_errno();
      $this->m_error = mysql_error();  
      switch ($this->m_errno)
      {
        case    0: return DB_SUCCESS;
        case 1044: return DB_ACCESSDENIED;  // todofixme: deze komt bij mysql pas na de eerste query. 
        case 1045: return DB_ACCESSDENIED_USER;
        case 1049: return DB_UNKNOWNDATABASE;
        case 2004: 
        case 2005: return DB_UNKNOWNHOST;
        default: 
                   return DB_UNKNOWNERROR;   
      }
    }
    
    /**
     * Disconnect from database
     */
    function disconnect()
    {
      atkdebug("Disconnecting from database...");
    }

    /**
     * Performs a query
     * @param $query the query
     * @param $offset offset in record list
     * @param $limit maximum number of records
     */
    function query($query, $offset=-1, $limit=-1)
    {
//      if(strpos($query, "content_page"))
//      {
//        atk_var_dump(debug_backtrace(), "<B>Backtrace of wrong query!</B>");
//      }
      /* limit? */
      if ($offset >= 0 && $limit > 0)
        $query .= " LIMIT $offset, $limit";  
      atkdebug("atkmysqldb.query(): ".$query);

      /* connect to database */
      if ($this->connect()==DB_SUCCESS)
      {
        /* free old results */
        if ($this->m_query_id)
        {
          @mysql_free_result($this->m_query_id);
          $this->m_query_id = 0;
        }      
        
        $this->m_affected_rows = 0;
      
        /* query database */
        $this->m_query_id = @mysql_query($query, $this->m_link_id);
        $this->m_row = 0;
        //$this->m_errno = mysql_errno();
        //$this->m_error = mysql_error();
        $this->m_affected_rows = mysql_affected_rows();
    
        /* invalid query */
        if (!$this->m_query_id)
        {
          $this->halt("Invalid SQL: $query");
          $this->halt($this->_translateError());
          return false;
        }
        
        /* return query id */
        return true;
      }
      return false;
    }

    /**
     * Goto the next record in the result set
     * @return result of going to the next record
     */
    function next_record()
    {
      /* goto next record */      
     
      $this->m_record = @mysql_fetch_array($this->m_query_id, MYSQL_ASSOC|atkconfig("mysqlfetchmode"));
      $this->m_row++;
      $this->m_errno = mysql_errno();
      $this->m_error = mysql_error();

      /* are we there? */
      $result = is_array($this->m_record);
      if (!$result && $this->m_auto_free)
      {
        @mysql_free_result($this->m_query_id);
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
      $result = @mysql_data_seek($this->m_query_id, $position);
      if ($result) $this->m_row = $position;
      else $this->halt("seek($position) failed: result has ".$this->num_rows()." rows");
    }
  
    /**
     * Lock a certain table in the database 
     * @param $table the table name
     * @param $mode the type of locking
     * @return result of locking
     */
    function lock($table, $mode="write")
    {
      /* connect first */
      if ($this->connect()==DB_SUCCESS)
      {    
        /* lock */
        $result = @mysql_query("lock tables $table $mode", $this->m_link_id);
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
      /* connect first */
      if ($this->connect()==DB_SUCCESS)
      {
        /* unlock */
        $result = @mysql_query("unlock tables");
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
      return @mysql_affected_rows($this->m_link_id);
    }

    /**
     * Evaluate the result; how many rows
     * were affected by the query.
     * @return number of affected rows
     */
    function num_rows()
    {
      return @mysql_num_rows($this->m_query_id);
    }

    /**
     * Evaluatie the result; how many fields
     * where affected by the query.
     * @return number of affected fields
     */
    function num_fields()
    {
      return @mysql_num_fields($this->m_query_idD);
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
      if ($this->connect()==DB_SUCCESS)
      {    
        /* lock sequence table */
        if ($this->lock($this->m_seq_table))
        {
          /* get sequence number (locked) and increment */
          $query = "SELECT ".$this->m_seq_field." FROM ".$this->m_seq_table." WHERE ".$this->m_seq_namefield." = '$sequence'";       
        
          $id = @mysql_query($query, $this->m_link_id);
          $result = @mysql_fetch_array($id);
      
          /* no current value, make one */
          if (!is_array($result))
          {
            $query = "INSERT INTO ".$this->m_seq_table." VALUES('$sequence', 1)";       
            $id = @mysql_query($query, $this->m_link_id);
            $this->unlock();          
            return 1;
          }
        
          /* enter next value */
          else
          {
            $nextid = $result[$this->m_seq_field] + 1;
            $query = "UPDATE ".$this->m_seq_table." SET ".$this->m_seq_field." = '$nextid' WHERE ".$this->m_seq_namefield." = '$sequence'";
          
            $id = @mysql_query($query, $this->m_link_id);
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
     * Return the meta data of a certain table
     * @param $table the table name
     * @param $full all meta data or not
     * @return array with meta data
     */
    function metadata($table, $full=false)
    {
      /* first connect */
      if ($this->connect()==DB_SUCCESS)
      {      
        atkimport("atk.db.atkddl");
        $ddl = &atkDDL::create("mysql");      
      
        /* list fields */
        atkdebug("Retrieving metadata for $table");
        $id = @mysql_list_fields($this->m_database, $table);
        if (!$id) 
        {
          atkdebug("Metadata query failed.");
          return array();
        }
 
        /* count fields */
        $count = @mysql_num_fields($id);

        /* get meta data */
        $result = array();      
        
        if ($full)
          $result["num_fields"] = $count;
        
        for ($i=0; $i<$count; $i++)
        {
          $result[$i]["table"]   = @mysql_field_table($id, $i);
          $result[$i]["name"]    = @mysql_field_name($id, $i);
          $result[$i]["type"]    = @mysql_field_type($id, $i);
          $result[$i]["gentype"] = $ddl->getGenericType($result[$i]['type']);
          $result[$i]["len"]     = @mysql_field_len($id, $i);
          $result[$i]["flags"]   = explode(' ', @mysql_field_flags($id, $i));
          
          $result[$i]["flags"] =
            (in_array('primary_key',    $result[$i]["flags"]) ? MF_PRIMARY        : 0) |
            (in_array('unique_key',     $result[$i]["flags"]) ? MF_UNIQUE         : 0) |
            (in_array('not_null',       $result[$i]["flags"]) ? MF_NOT_NULL       : 0) |            
            (in_array('auto_increment', $result[$i]["flags"]) ? MF_AUTO_INCREMENT : 0);
          
          if ($full)
            $result["meta"][$result[$i]["name"]] = $i;
        }
        /* free result */
        @mysql_free_result($id);
        atkdebug("Metadata for $table complete");
        return $result;
      }
      return array();
    }

    /**
     * Return the available table names
     * @return array with table names etc.
     */
    function table_names()
    {
      /* query */
      $this->query("SHOW TABLES");
      
      /* get table names */
      $result = array();
      for ($i=0; $info = mysql_fetch_row($this->m_query_id); $i++)
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
    function tableExists($table)
    {
      global $g_db;
      $res = $g_db->getrows("SHOW TABLES LIKE '$table'");
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
     *
     * TODO/FIXME: add format paramater. Current format is always yyyy-mm-dd.
     */
    function func_datetochar($fieldname)
    {
      return "DATE_FORMAT($fieldname, '%Y-%m-%d, %Y')";
    }
      
    /** 
     * Get TO_CHAR() equivalent for the current database.
     *
     * TODO/FIXME: add format paramater. Current format is always yyyy-mm-dd hh:mi.
     */
    function func_datetimetochar($fieldname)
    {
      return "DATE_FORMAT($fieldname, '%Y-%m-%d %H:%i')";
    }  
  }  
?>
