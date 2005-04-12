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
  
  /** @internal include baseclass */
  require_once(atkconfig("atkroot")."atk/db/class.atkdb.inc");
  //PostgreSQL error's includen
  include_once(atkconfig("atkroot")."atk/db/languages/postgresql_".atkconfig('language').'.lng');

  /**
   * Database driver for PostgreSQL.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */ 
  class atkPgsqlDb extends atkDb
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
        $this->m_link_id = pg_connect($connection_str);
        if ($this->m_link_id === FALSE)
        {
          $this->halt("connect using ".$this->m_database.", ".$this->m_host.", ".$this->m_user.", ***** failed.");
          
          // We can't use pg_result_error, since we need a resource
          // for that function, and if pg_connect fails, we don't even have
          // a resource yet.
          if (function_exists("pg_last_error")) // only available since PHP 4.2.0.
          {
            return $this->_translateError(@pg_last_error());
          }
          else
          {
            return DB_UNKNOWNERROR;
          }
        }
      }

      return DB_SUCCESS;
    }
    
    /**
     * TODO FIXME: I don't know what errormessges postgresql gives,
     * so this function only returns DB_UNKNOWNERROR for now.
     */
    function _translateError($error="")
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
      if ($offset >= 0 && $limit >= 0)
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
        $this->m_query_id = @pg_exec($this->m_link_id, $query) or $error = true;
        $this->m_row = 0;
        $this->m_error = pg_errormessage();
        if ($error) $this->halt("Invalid SQL query: $query");
        
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
        $sequencename = atkconfig("database_sequenceprefix").$sequence;
        /* get sequence number and increment */
        $query = "SELECT nextval('$sequencename') AS nextid";
    
        /* execute query */
        $id = @pg_exec($this->m_link_id, $query);
        
        // maybe the name should not have the default seq_ prefix.
        // this is here for backwardscompatibility. $config_database_sequenceprefix
        // should be used to specify the sequence prefix.
        if (empty($id))
        {
          /* get sequence number and increment */
          $query = "SELECT nextval('".$sequence."') AS nextid";
  
          /* execute query */
          $id = @pg_exec($this->m_link_id, $query);
        }
    
        /* error? */
        if (empty($id))
        {
          /* create sequence */
          $query = "CREATE SEQUENCE '".$sequencename."'";
          $id = @pg_exec($this->m_link_id, $query);

          /* try again */
          $query = "SELECT nextval('".$sequencename."') AS nextid";
          $id = @pg_exec($this->m_link_id, $query) or $this->halt("cannot get nextval() of sequence '$sequencename'");                
        
          /* empty? */
          if (empty($id)) return 0;
        }
              
        /* get nextid */
        $result = @pg_result($id, 0, "nextid");      

        /* return id */
        return $result;
      }
      return 0;
    }

    /**
     * Return the meta data of a certain table
     * @param $table the table name
     * @param $full all meta data or not
     * @return array with meta data
     */
    function metadata($table, $full=false)
    { 
      atkimport("atk.db.atkddl");
      $ddl = &atkDDL::create("pgsql");
    
      // Get meta data from system tables.
      // See developer manual (www.postgresql.org)
      // for system table specification.
      $sql = 
       "SELECT
          a.attnum AS i,
          a.attname AS name,
          t.typname AS type,
          (CASE WHEN LOWER(t.typname) = 'varchar' AND a.attlen = -1 THEN a.atttypmod - 4 ELSE a.attlen END) AS length,
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
          (CASE WHEN ad.adsrc LIKE 'nextval(%::text)' THEN 1 ELSE 0 END) AS is_auto_inc,
          (CASE WHEN ad.adsrc LIKE 'nextval(%::text)' THEN SUBSTRING(ad.adsrc, '''(.*?)''') END) AS sequence
        FROM pg_class c
        JOIN pg_attribute a ON (a.attrelid = c.oid AND a.attnum > 0)
        JOIN pg_type t ON (t.oid = a.atttypid)
        LEFT JOIN pg_attrdef ad ON (ad.adrelid = c.oid AND ad.adnum = a.attnum)
        WHERE c.relname = '$table'
        ORDER BY a.attnum";

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
          ($row['is_auto_inc'] == 1 ? MF_AUTO_INCREMENT : 0);
        
        if ($row['is_auto_inc'] == 1)
          $meta[$i]['sequence'] = $row['sequence'];
        
        if ($full)
          $meta["meta"][$row['name']] = &$meta[$i];
      }
      
      if ($full)
        $meta['num_fields'] = count($rows);

      return $meta;
    }

    /**
     * Return the available table names
     * @return array with table names etc.
     */
    function table_names()
    {
      /* query */
      $this->query("SELECT relname FROM pg_class WHERE relkind = 'r' AND NOT relname LIKE 'pg_%' AND NOT relname LIKE 'sql_%'");
 

      $result = array();   
      for ($i=0; $info = @pg_fetch_row($this->m_query_id, $i); $i++)    
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
      return array("exact","substring","wildcard","regexp","greaterthan","greaterthanequal","lessthan","lessthanequal","between");
    }
    
  }  
?>
