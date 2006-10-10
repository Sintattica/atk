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
 require_once(atkconfig("atkroot")."atk/db/class.atkmysqldb.inc");

 /**
  * Driver for MySQL databases > 4.1.3
  *
  * @author Eldad Ran <eldad@tele-concept.com>
  * @package atk
  * @subpackage db
  */
 class atkMysql41Db extends atkMysqlDb
 {

   /**
    * Base constructor
    */
   function atkmysql41db()
   {
     // set type
     parent::atkmysqldb();
     $this->m_type = "mysql41";
   }

   /**
    * Connect to the database
    * @return connection status
    */
   function doConnect($host,$user,$password,$charset)
   {
     /* establish connection */
     if (empty($this->m_link_id))
     {
       $this->m_link_id = @mysqli_connect($host, $user, $password);
       if (!$this->m_link_id)
       {
         $this->halt($this->getErrorMsg());
       }
       
       /* set character set */
       if (!empty($charset))
       {
         atkdebug("Set database character set to: {$charset}");
         @mysqli_query($this->m_link_id, "SET NAMES '{$charset}'");
       }        
       
       /* set autoCommit to off */
       mysqli_autocommit($this->m_link_id, FALSE);
  
       /* select database */
       if (!@mysqli_select_db( $this->m_link_id, $this->m_database))
       {
         $this->halt($this->getErrorMsg());
         return $this->_translateError();
       }       
     }


     /* return link identifier */
     return DB_SUCCESS;
   }

   function _translateError()
   {
     if($this->m_link_id!="")
     {
       $this->m_errno = mysqli_errno($this->m_link_id);
       $this->m_error = mysqli_error($this->m_link_id);
     }
     else
     {
       $this->m_errno = mysqli_connect_errno();
       $this->m_error = mysqli_connect_error();
     }
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
     if($this->m_link_id)
     {
       mysqli_close($this->m_link_id);
       $this->m_link_id = 0;
     }
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

     if (atkconfig("debug")>0)
     {
       atkimport("atk.utils.atkdebugger");
       atkDebugger::addQuery($query);
     }

     /* connect to database */
     $mode = $this->getQueryMode($query);
     if ($this->connect($mode)==DB_SUCCESS)
     {
       /* free old results */
       if ($this->m_query_id)
       {
         @mysqli_free_result($this->m_query_id);
         $this->m_query_id = 0;
       }

       $this->m_affected_rows = 0;

       /* query database */
       $this->m_query_id = @mysqli_query($this->m_link_id, $query);

       $this->m_row = 0;

       $this->m_affected_rows = mysqli_affected_rows($this->m_link_id);

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
     $this->m_record = @mysqli_fetch_array($this->m_query_id, MYSQLI_ASSOC|atkconfig("mysqlfetchmode"));
     $this->m_row++;
     $this->m_errno = mysqli_errno($this->m_link_id);
     $this->m_error = mysqli_error($this->m_link_id);

     /* are we there? */
     $result = is_array($this->m_record);
     if (!$result && $this->m_auto_free)
     {
       @mysqli_free_result($this->m_query_id);
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
     $result = @mysqli_data_seek($this->m_query_id, $position);
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
     if ($this->connect('w')==DB_SUCCESS)
     {
       /* lock */
       $result = @mysqli_query($this->m_link_id, "lock tables $table $mode");
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
     if ($this->connect('w')==DB_SUCCESS)
     {
       /* unlock */
       $result = @mysqli_query($this->m_link_id, "unlock tables");
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
     return @mysqli_affected_rows($this->m_link_id);
   }

   /**
    * Evaluate the result; how many rows
    * were affected by the query.
    * @return number of affected rows
    */
   function num_rows()
   {
     return @mysqli_num_rows($this->m_query_id);
   }

   /**
    * Evaluatie the result; how many fields
    * where affected by the query.
    * @return number of affected fields
    */
   function num_fields()
   {
     return @mysqli_num_fields($this->m_query_idD);
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
     if ($this->connect('w')==DB_SUCCESS)
     {
       /* lock sequence table */
       if ($this->lock($this->m_seq_table))
       {
         /* get sequence number (locked) and increment */
         $query = "SELECT ".$this->m_seq_field." FROM ".$this->m_seq_table." WHERE ".$this->m_seq_namefield." = '$sequence'";

         $id = @mysqli_query($this->m_link_id, $query);
         $result = @mysqli_fetch_array($id);

         /* no current value, make one */
         if (!is_array($result))
         {
           $query = "INSERT INTO ".$this->m_seq_table." VALUES('$sequence', 1)";
           $id = @mysqli_query( $this->m_link_id, $query);
           $this->unlock();
           return 1;
         }

         /* enter next value */
         else
         {
           $nextid = $result[$this->m_seq_field] + 1;
           $query = "UPDATE ".$this->m_seq_table." SET ".$this->m_seq_field." = '$nextid' WHERE ".$this->m_seq_namefield." = '$sequence'";

           $id = @mysqli_query( $this->m_link_id, $query);
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
     if ($this->connect('r')==DB_SUCCESS)
     {
       atkimport("atk.db.atkddl");
       $ddl = &atkDDL::create("mysql41");

       /* list fields */
       atkdebug("Retrieving metadata for $table");

       /* get meta data */
       $id = @mysqli_query( $this->m_link_id, "SELECT * FROM {$table} LIMIT 1");
       if (!$id)
       {
         	atkdebug("Metadata query failed.");
         	return array();
       }
       $i  = 0;
       $result = array();

       while ($finfo = mysqli_fetch_field($id))
       {
		      $result[$i]["table"]   = $finfo->table;
	        $result[$i]["name"]    = $finfo->name;
	        $result[$i]["type"]    = $finfo->type;
	        $result[$i]["gentype"] = $ddl->getGenericType($finfo->type);
	        $result[$i]["len"]     = $finfo->length;

	        if($finfo->flags & MYSQLI_PRI_KEY_FLAG) $result[$i]["flags"]|= MF_PRIMARY;
	        if($finfo->flags & MYSQLI_UNIQUE_KEY_FLAG) $result[$i]["flags"]|= MF_UNIQUE;
	        if($finfo->flags & MYSQLI_NOT_NULL_FLAG ) $result[$i]["flags"]|= MF_NOT_NULL;
	        if($finfo->flags & MYSQLI_AUTO_INCREMENT_FLAG) $result[$i]["flags"]|= MF_AUTO_INCREMENT;

         	if ($full)
           		$result["meta"][$result[$i]["name"]] = $i;
	      	$i++;
      	}
        $result["num_fields"] = $i;
      	mysqli_free_result($id);

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
     for ($i=0; $info = mysqli_fetch_row($this->m_query_id); $i++)
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
       mysqli_commit($this->m_link_id);
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
       mysqli_rollback($this->m_link_id);
     }
     return true;
   }

 }
?>
