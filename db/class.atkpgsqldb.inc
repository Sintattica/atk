<?php
  
  require_once(atkconfig("atkroot")."atk/db/class.atkdb.inc");
  //PostgreSQL error's includen
  include_once(atkconfig("atkroot")."atk/db/languages/postgresql_".atkconfig('language').'.lng');

  /**
   * Handles database connectivity and database interaction
   * with the PostgreSQL database server. Portions of this class are
   * based on the db_pgql.inc file of PHPLib.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$   
   *
   */ 
  class atkpgsqldb extends atkdb
  {
    /* identification */
    var $m_type = "pgsql";  
  
    /**
     * Base constructor 
     */
    function atkpgsqldb()
    {
      /* do nothing */
    }
  
    /**
     * Connect to the database     
     * @return connection identifier
     */    
    function connect()
    { 
      if (empty($this->m_link_id))
      {
        /* connection string */
        $connection_str  = "dbname=".$this->m_database;
        if (!empty($this->m_host))     $connection_str .= " host=".$this->m_host;
        if (!empty($this->m_user))     $connection_str .= " user=".$this->m_user;
        if (!empty($this->m_password)) $connection_str .= " password=".$this->m_password;
      
        /* establish connection */              
        $this->m_link_id = @pg_pconnect($connection_str);
        if (!$this->m_link_id)
        {
          $this->halt("connect using ".$this->m_database.", ".$this->m_host.", ".$this->m_user.", ***** failed.");
          
          // We can't use pg_result_error, since we need a resource
          // for that function, and if pg_pconnect fails, we don't even have
          // a resource yet.
          if (function_exists("pg_last_error")) // only available since PHP 4.2.0.
          {
            return $this->_translateError(pg_last_error());
          }
          else
          {
            return DB_UNKNOWNERROR;
          }
        }
      }
      /* return link identifier */
      return DB_SUCCESS;
    }
    
    /**
     * TODO FIXME: I don't know what errormessges postgresql gives,
     * so this function only returns DB_UNKNOWNERROR for now.
     */
    function _translateError($error)
    {
      return DB_UNKNOWNERROR;
    }

    /**
     * Disconnect from database, we use a persistent
     * link, so this won't be necessary!
     */
    function disconnect()
    {
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
        $query .= " LIMIT $limit OFFSET $offset";  
      atkdebug("atkpgsqldb.query(): ".$query);

      /* connect to database */
      if ($this->connect()==DB_SUCCESS)
      {
        /* free old results */
        if (!empty($this->m_query_id))
        {
          @pg_freeresult($this->m_query_id);
          $this->m_query_id = 0;
        }
        
        /* query database */
        $this->m_query_id = @pg_exec($this->m_link_id, $query) or $this->halt("Invalid SQL query: $query");
        $this->m_row = 0;
        $this->m_error = pg_errormessage();
        
        if ($this->m_query_id)
        {
          /* return query id */
          return true;
        }
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
      $this->m_record = @pg_fetch_array($this->m_query_id, $this->m_row);
      $this->m_row++;

      /* are we there? */
      $result = is_array($this->m_record);
      if (!$result && $this->m_auto_free)
      {
        @pg_freeresult($this->m_query_id);
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
      $this->m_row = $position;
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
        if ($mode == "write")
          $result = @pg_exec($this->m_link_id, "lock table $table") or $this->halt("cannot lock table $table");
        else $result = 1;

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
        $result = @pg_exec($this->m_link_id, "commit") or $this->halt("cannot unlock tables");
      
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
      return @pg_cmdtuples($this->m_link_id);
    }

    /**
     * Evaluate the result; how many rows
     * were affected by the query.
     * @return number of affected rows
     */
    function num_rows()
    {
      return @pg_numrows($this->m_query_id);
    }

    /**
     * Evaluatie the result; how many fields
     * where affected by the query.
     * @return number of affected fields
     */
    function num_fields()
    {
      return @pg_numfields($this->m_query_id);
    }

    /**
     * Get the next sequence number
     * of a certain sequence.
     * @param $sequence the sequence name
     * @return the next sequence id
     */
    function nextid($sequence)
    {
      /* connect first */
      if ($this->connect()==DB_SUCCESS)
      {
        /* get sequence number and increment */
        $query = "SELECT nextval('seq_".$sequence."') AS nextid";
    
        /* execute query */
        $id = @pg_exec($this->m_link_id, $query) or $this->halt("cannot get nextval() of sequence 'seq_$sequence'");                
    
        /* error? */
        if (empty($id))
        {
          /* create sequence */
          $query = 'CREATE SEQUENCE "seq_'.$sequence.'"';
          $id = @pg_exec($this->m_link_id, $query);

          /* try again */
          $query = "SELECT nextval('seq_".$sequence."') AS nextid";
          $id = @pg_exec($this->m_link_id, $query) or $this->halt("cannot get nextval() of sequence 'seq_$sequence'");                
        
          /* empty? */
          if (empty($id)) return 0;
        }
        return 0;
      }

      /* get nextid */
      $result = @pg_result($id, 0, "nextid");      

      /* return id */
      return $result;
    }

    /**
     * Return the meta data of a certain table
     * @param $table the table name
     * @param $full all meta data or not
     * @return array with meta data
     */
    function metadata($table, $full=false)
    {  
      $meta    = array();    // Metadata array - return value

      // See PostgreSQL developer manual (www.postgresql.org) for system table spec.
      // Get catalog data from system tables.
      $sql = 'SELECT a.attnum, a.attname, t.typname, a.attlen, a.atttypmod, a.attnotnull, 
                     a.atthasdef 
              FROM pg_class as c, pg_attribute a, pg_type t 
              WHERE a.attnum > 0 and a.attrelid = c.oid and 
                    c.relname = '."'$table'".' and 
                    a.atttypid = t.oid order by a.attnum';

      $rows = $this->getrows($sql);
              
      // Store meta data
      for ($i = 0; $i < count($rows); $i++) 
      {
        // Field Name
        $meta[$i]['table'] = $table;
        $meta[$i]['name'] = $rows[$i]['attname'];
        $meta[$i]['type'] = $rows[$i]['typname'];
        $meta[$i]['len']  = $rows[$i]['attlen'];
        if ($meta[$i]['type']=='varchar'&&$meta[$i]['len']==-1)
        {          
          $meta[$i]['len'] = $rows[$i]['atttypmod']-4;
        }
        if ($full) $result["meta"][$result[$i]["name"]] = $i;
        
      }
 
      return $meta;
    }

    

    /**
     * Return the available table names
     * @return array with table names etc.
     */
    function table_names()
    {
      /* query */
      $this->query("SELECT relname FROM pg_class WHERE relkind = 'r' AND NOT relname LIKE 'pg_%'");
    
      for ($i=0; $info = pg_fetch_row($this->m_query_id, $i); $i++)    
      {
        $result[$i]["table_name"]      = $info[0];
        $result[$i]["tablespace_name"] = $this->m_database;
        $result[$i]["database"]        = $this->m_database;
      }
      
      /* return result */
      return $result;
    }
    
    /**
     * This function indicates what searchmodes the database supports.
     * @return array with search modes
     */
    function getSearchModes()
    {      
      return array("exact","substring","wildcard","regexp","greaterthan","greaterthanequal","lessthan","lessthanequal");
    }
    
  }  
?>