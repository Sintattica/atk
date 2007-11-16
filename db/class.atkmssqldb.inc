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
  */

 /**
  * @internal Include parent class
  */
 require_once(atkconfig("atkroot")."atk/db/class.atkmysqldb.inc");

 /**
  * Driver for MsSQL databases
  *
  * This driver is only supporting MS SQL partially at the moment
  * 
  * Succesfully tested:
  * - SELECT, INSERT, UPDATE, DELETE, LEFT JOIN, ORDER BY, LIKE
  * - atkAttribute
  * - atkNumberAttribute
  * - atkOneToManyRelation
  * - atkManyToOneRelation
  * - atkBoolAttribute
  *
  * @todo
  * - LIMIT and OFFSET support
  * - REGEX support
  * - SOUNDEX support
  * - Support for DATE attribute
  * - Support for TIME attribute
  * - Support for DATETIME attribute
  *
  * Attributes and database functions not stated above are untested
  *
  * @author Harrie Verveer <harrie@ibuildings.nl>
  * @package atk
  * @subpackage db
  */
 class atkMsSqlDb extends atkMysqlDb 
 {

   /**
    * Class constructor
    */
   function atkMsSqlDb()
   {
     $this->m_type = "mssql";
     $this->m_vendor = "microsoft";
     $this->m_user_error=array(); 
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
       $this->m_link_id = @mssql_connect($this->m_host, $this->m_user, $this->m_password);

       if (!$this->m_link_id)
       {
         $this->halt($this->getErrorMsg());
       }
     }

     /* select database */
     if (!@mssql_select_db($this->m_database,$this->m_link_id))
     {
       $this->halt($this->getErrorMsg());
       return $this->_translateError();
     }

     return DB_SUCCESS;
   }

   function _translateError()
   {
     // The MSSQL interface can not return detailed error information
     $this->m_errno = 1;
     $this->m_error = atktext('unknown_error');
     
     return DB_UNKNOWNERROR;
   }

   /**
    * Disconnect from database
    */
   function disconnect()
   {
     atkdebug("Disconnecting from database...");
     mssql_close($this->m_link_id);
   }

   /**
    * Performs a query
    * @param $query the query
    * @param $offset offset in record list
    * @param $limit maximum number of records
    * @todo implement limit and offset support
    */
   function query($query, $offset=-1, $limit=-1)
   {
      // <- implement limit support here

     atkdebug("atkmssqldb.query(): ".$query);

     /* connect to database */
     if ($this->connect()==DB_SUCCESS)
     {
       /* free old results */
       if ($this->m_query_id)
       {
         @mssql_free_result($this->m_query_id);
         $this->m_query_id = 0;
       }

       $this->m_affected_rows = 0;

       /* query database */
       $this->m_query_id = @mssql_query($query,$this->m_link_id);       
       $this->m_row = 0;
        
       $this->m_affected_rows = mssql_rows_affected($this->m_link_id);

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
     $this->m_record = @mssql_fetch_array($this->m_query_id, MSSQL_ASSOC);
     $this->m_row++;
     
     /* are we there? */
     $result = !($this->m_record === false);
     if (!$result && $this->m_auto_free)
     {
       @mssql_free_result($this->m_query_id);
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
   function seek($position=0, $haltOnError=true)
   {
     $result = @mssql_data_seek($this->m_query_id, $position);
     if ($result) $this->m_row = $position;
     else if ($haltOnError) $this->halt("seek($position) failed: result has ".$this->num_rows()." rows");
      return $result;
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
       $result = @mssql_query("lock tables $table $mode", $this->m_link_id);
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
       $result = @mssql_query("unlock tables", $this->m_link_id);
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
     return @mssql_rows_affected($this->m_link_id);
   }

   /**
    * Evaluate the result; how many rows
    * were affected by the query.
    * @return number of affected rows
    */
   function num_rows()
   {
     return @mssql_num_rows($this->m_query_id);
   }

   /**
    * Evaluatie the result; how many fields
    * where affected by the query.
    * @return number of affected fields
    */
   function num_fields()
   {
     return @mssql_num_fields($this->m_query_id);
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

         $id = @mssql_query($query, $this->m_link_id);
         $result = @mssql_fetch_array($id);

         /* no current value, make one */
         if (!is_array($result))
         {
           $query = "INSERT INTO ".$this->m_seq_table." VALUES('$sequence', 1)";
           $id = @mssql_query($query, $this->m_link_id);
           $this->unlock();
           return 1;
         }

         /* enter next value */
         else
         {
           $nextid = $result[$this->m_seq_field] + 1;
           $query = "UPDATE ".$this->m_seq_table." SET ".$this->m_seq_field." = '$nextid' WHERE ".$this->m_seq_namefield." = '$sequence'";

           $id = @mssql_query($query,$this->m_link_id);
           $this->unlock();
           return $nextid;
         }
       }
       return 0;
     }

     /* cannot connect */
     else
     {
       $this->halt("cannot connect to  ".$this->m_host);
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
       $ddl = &atkDDL::create("mssql");

       /* list fields */
       atkdebug("Retrieving metadata for $table");
       /* get meta data */
       $id = @mssql_query("SELECT TOP 1 * FROM [$table]", $this->m_link_id);
       if (!$id)
       {
         	atkdebug("Metadata query failed.");
         	return array();
       }
       $i = 0;
       $result = array();

       while ($finfo = mssql_fetch_field($id))
       {
		      $result[$i]["table"]   = $table;
	        $result[$i]["name"]    = $finfo->name;
	        $result[$i]["type"]    = $finfo->type;
	        $result[$i]["gentype"] = $ddl->getGenericType($finfo->type);
	        $result[$i]["len"]     = $finfo->max_length;
	        $result[$i]["flags"]   = array();

	        $result[$i]["flags"] =
	           	(in_array('primary_key',    $result[$i]["flags"]) ? MF_PRIMARY        : 0) |
	           	(in_array('unique_key',     $result[$i]["flags"]) ? MF_UNIQUE         : 0) |
	           	(in_array('not_null',       $result[$i]["flags"]) ? MF_NOT_NULL       : 0) |
	           	(in_array('auto_increment', $result[$i]["flags"]) ? MF_AUTO_INCREMENT : 0);

         	if ($full)
           		$result["meta"][$result[$i]["name"]] = $i;
		      $i++;
	     }
        
	     $result["num_fields"] = $i;
	     mssql_free_result($id);

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
     for ($i=0; $info = mssql_fetch_row($this->m_query_id); $i++)
     {
       $result[$i]["table_name"]      = $info[0];
       $result[$i]["tablespace_name"] = $this->m_database;
       $result[$i]["database"]        = $this->m_database;
     }

     /* return result */
     return $result;
   }

   /**
    * Commit the current transaction.
    */
   function commit()
   {
     if($this->m_link_id)
     {
       atkdebug("Commit");
       mssql_query("COMMIT TRANSACTION;",$this->m_link_id);
     }
     return true;
   }

   /*
    * Rollback the the current transaction.
    */
   function rollback()
   {
     if($this->m_link_id)
     {
       atkdebug("Rollback");
       mssql_query("ROLLBACK TRANSACTION;",$this->m_link_id);
     }
     return true;
   }
   
   function getErrorMsg()
   {
     return atktext("db_unknown_mssql_error");
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

      // MSSQL doesn't support LIMIT syntax as MySQL and PostgreSQL do.
      // We have to seek through the results to go to a certain offset 
      // and retrieve only the number of rows specified in the limit. 
      // In class.atkmssqlquery.inc we add a fake LIMIT clause which we
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

      $this->query($query);
      if ($offset > 0)
      {
        if (!$this->seek($offset, false)) 
          return array();
      }

      if ($limit > 0)
      {
        for ($i = 0; $i < $limit; $i++)
        {
          if (!$this->next_record()) break;
          $result[] = $this->m_record;
        }
      }
      else
      {
        while ($this->next_record())
          $result[] = $this->m_record;
      }

      return $result;
    }



 }
?>
