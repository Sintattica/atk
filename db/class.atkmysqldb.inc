<?php

  require_once(atkconfig("atkroot")."atk/db/class.atkdb.inc");

  /**
   * Handles database connectivity and database interaction
   * with the MySQL database server. Portions of this class are
   * based on the db_mysql.inc file of PHPLib.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$   
   *
   */ 
  class atkmysqldb extends atkdb
  {    
     
    /**
     * Base constructor 
     */
    function atkmysqldb()
    {
      /* do nothing */
    }
  
    /**
     * Connect to the database
     * @param $database the database name
     * @param $host database hostname
     * @param $user database username
     * @param $password database password
     * @return link identifier
     */
    function connect($database="", $host="", $user="", $password="")
    {
      /* defaults ? */
      if (empty($database)) $database = $this->m_database;
      if (empty($host)) $host = $this->m_host;
      if (empty($user)) $user = $this->m_user;
      if (empty($password)) $password = $this->m_password;      
      
      /* establish connection */
      if (empty($this->m_link_id))
      {
        $this->m_link_id = mysql_pconnect($host, $user, $password);
        if (!$this->m_link_id) $this->halt("connect using $host, $user, ***** failed.");
      }

      /* select database */
      if (!@mysql_select_db($database, $this->m_link_id))
        $this->halt("cannot use database $database.");
        
      /* return link identifier */
      return $this->m_link_id;
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
      /* limit? */
      if ($offset >= 0 && $limit > 0)
        $query .= " LIMIT $offset, $limit";  
      atkdebug("atkmysqldb.query(): ".$query);

      /* connect to database */
      $this->connect();

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
      $this->m_errno = mysql_errno();
      $this->m_error = mysql_error();
      $this->m_affected_rows = mysql_affected_rows();
    
      /* invalid query */
      if (!$this->m_query_id)
        $this->halt("Invalid SQL: $query");
        
      /* return query id */
      return $this->m_query_id;
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
      $this->connect();
    
      /* lock */
      $result = @mysql_query("lock tables $table $mode", $this->m_link_id);
      if (!$result) $this->halt("$mode lock on $table failed.");
      
      /* return result */
      return $result;
    }
  
    /**
     * Unlock table(s) in the database 
     * @return result of unlocking
     */
    function unlock()
    {
      /* connect first */
      $this->connect();

      /* unlock */
      $result = @mysql_query("unlock tables");
      if (!$result) $this->halt("unlock tables failed.");
      
      /* return result */
      return $result;
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
      $this->connect();
    
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
      $this->connect();
      
      /* list fields */
      atkdebug("Retrieving metadata for $table");
      $id = @mysql_list_fields($this->m_database, $table);
      if (!$id) atkdebug("Metadata query failed.");

      /* count fields */
      $count = @mysql_num_fields($id);

      /* get meta data */
      $result = array();      
      if ($full) $result["num_fields"] = $count;
      for ($i=0; $i<$count; $i++)
      {
        $result[$i]["table"] = @mysql_field_table($id, $i);
        $result[$i]["name"]  = @mysql_field_name($id, $i);
        $result[$i]["type"]  = @mysql_field_type($id, $i);
        $result[$i]["len"]   = @mysql_field_len($id, $i);
        $result[$i]["flags"] = @mysql_field_flags($id, $i);
        if ($full) $result["meta"][$result[$i]["name"]] = $i;
      }
      /* free result */
      @mysql_free_result($id);
      atkdebug("Metadata for $table complete");
      return $result;
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
       * @params $tableName the table to find
       * @params returns true if found, false if not found
       */  
    function tableExists($table)
    {
      global $g_db;
      
      $res = $g_db->getrows("SHOW TABLES");    
      $found = FALSE;
      for($i=0; ($i<count($res)) && (!$found); $i++)
      {
        if ($res[$i][0] == $table) $found = TRUE;
      }    
      return ($found);    
    }    
  
    /**
     * This function indicates what searchmodes the database supports.
     * @return array with search modes
     */
    function getSearchModes()
    {      
      return array("exact","substring","wildcard","regexp","soundex","greaterthan","greaterthanequal","lessthan","lessthanequal");
    }
    
  }  
  
?>
