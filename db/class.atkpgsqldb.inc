<?php
/* Multiple inclusion protection */
if (!defined("CLASS_ATKPGSQLDB_INC"))
{
  define("CLASS_ATKPGSQLDB_INC", 1);

  /* include */
  require("atk/db/class.atkdb.inc");

  /**
   * Handles database connectivity and database interaction
   * with the PostgreSQL database server. Portions of this class are
   * based on the db_pgql.inc file of PHPLib.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   *
   * $Id$
   * $Log$
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:11  ivo
   * Added atk4 to repository.
   *
   * Revision 1.1  2001/04/06 11:27:47  peter
   * Added PostgreSQL support (really this time!), please note this version
   * is not ready for use in a production environment because of missing
   * support for outer joins. Support will follow as soon as version
   * 7.1 or higher of the PostgreSQL database has been released!
   *
   */ 
  class atkpgsqldb extends atkdb
  {
    /* identification */
    var $m_type = "pgsql";
    var $m_revision = '$Revision$';
  
    /**
     * Base constructor 
     */
    function atkpgsqldb()
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
      if (empty($host))     $host = $this->m_host;
      if (empty($user))     $user = $this->m_user;
      if (empty($password)) $password = $this->m_password;      
      
      /* connection string */
                             $connection_str  = "dbname=$database";
      if (!empty($host))     $connection_str .= " host=$host";
      if (!empty($user))     $connection_str .= " user=$user";
      if (!empty($password)) $connection_str .= " password=$password";
      
      /* establish connection */
      if (empty($this->m_link_id))
      {
        $this->m_link_id = @pg_pconnect($connection_str) or $this->halt("connect using $database, $host, $user, ***** failed.");
      }

      /* return link identifier */
      return $this->m_link_id;
    }

    /**
     * Disconnect from database, we use a persistent
     * link, so this won't be necessary!
     */
    function disconnect()
    {}

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
  
      /* connect to database */
      $this->connect();

      /* free old results */
      if (!empty($this->m_query_id))
      {
        @pg_freeresult($this->m_query_id);
        $this->m_query_id = 0;
      }
        
      /* query database */
      $this->m_query_id = @pg_exec($this->m_link_id, $query) or $this->halt("Invalid SQL query: $query");
      $this->m_row = 0;

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
      $this->connect();
  
      /* lock */
      if ($mode == "write")
        $result = @pg_exec($this->m_link_id, "lock table $table") or $this->halt("cannot lock table $table");
      else $result = 1;

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
      $result = @pg_exec($this->m_link_id, "commit") or $this->halt("cannot unlock tables");
      
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
      $this->connect();

      /* get sequence number and increment */
      $query = "SELECT nextval('seq_$sequence') AS nextid";
    
      /* execute query */
      $id = @pg_exec($this->m_link_id, $query);
    
      /* error? */
      if (empty($id))
      {
        /* create sequence */
        $query = "CREATE SEQUENCE 'seq_$sequence'";
        $id = @pg_exec($this->m_link_id, $query) or $this->halt("cannot get nextval() of sequence 'seq_$sequence'");        
        
        /* try again */
        $query = "SELECT nextval('seq_$sequence') AS nextid";
        $id = @pg_exec($this->m_link_id, $query) or $this->halt("cannot get nextval() of sequence 'seq_$sequence'");                
        
        /* empty? */
        if (empty($id)) return 0;
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
      /* first connect */
      $this->connect();

      /* try to get metadata */
      $id = @pg_exec($this->m_link_id, "SELECT * FROM $table") or $this->halt("metadata query for table $table failed");
      if (empty($id)) return array();
      
      /* number of fields / result array */
      $count = pg_numfields($id);
      $result = array();
    
      /* get meta data */
      for ($i=0; $i<$count; $i++)
      {
        $result[$i]["table"] = $table;
        $result[$i]["name"]  = @pg_fieldname($id, $i); 
        $result[$i]["type"]  = @pg_fieldtype($id, $i);
        $result[$i]["len"]   = @pg_fieldsize($id, $i);
        $result[$i]["flags"] = "";
        if ($full) $result["meta"][$result[$i]["name"]] = $i;        
      }
      
      /* free result */
      @pg_freeresult($id);
      
      /* return */
      return $result;
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
     * error handling
     */
    function halt($message)
    {
      /* no reporting */
      if ($this->m_halt_on_error = "no") return;

      /* print error */
      echo "<b>Database error:</b> $message<br>\n";
      echo "<b>PostgreSQL Error</b>: ".$this->m_errno." (".$this->m_error.")<br>\n";

      /* not only report but also quit? */
      if ($this->m_halt_on_error != "report") die();
    }
  }  
} /* endif CLASS_ATKPGSQLDB_INC; */