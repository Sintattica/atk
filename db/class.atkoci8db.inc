<?php

  require_once(atkconfig("atkroot")."atk/db/class.atkdb.inc");

  /**
   * Handles database connectivity and database interaction
   * with the Oracle database server. Portions of this class are
   * based on the db_oci8.inc file of PHPLib.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$   
   *
   */
  class atkoci8db extends atkdb
  {    
    /**
     * Base constructor
     */
    function atkoci8db()
    {
      /* do nothing */
    }

    /**
     * Connect to the database
     * @return connection status
     */
    function connect()
    {      
      /* establish connection to database */
      if (empty($this->m_link_id))
      {
        $this->m_link_id = OCIplogon($this->m_user, $this->m_password, $this->m_database);
        if (!$this->m_link_id)
        {
          $this->halt("connect using ".$this->m_user." / ***** on ".$this->m_database." failed.");
          $err = OCIError(); // can't pass link_id because it is null if connect failed.
          return $this->_translateError($err['code']);
        }

        /* set default date format for Oracle */ 
        $nls = OCIParse($this->m_link_id, "alter session set nls_date_format='YYYY-MM-DD'");
        $myresult = OCIExecute($nls);

      }
      return DB_SUCCESS;
    }
    
    /**
     * TODO FIXME: I don't know what errormessges oracle gives,
     * so this function only returns DB_UNKNOWNERROR for now.
     */
    function _translateError($error)
    {
      return DB_UNKNOWNERROR;
    }

    /**
     * Disconnect from database
     */
    function disconnect()
    {
      atkdebug("Disconnecting from database...");
      OCILogoff($this->m_link_id);
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
        $this->m_query_id = @OCIParse($this->m_link_id, $query);
        if ($this->m_query_id) @OCIExecute($this->m_query_id,OCI_COMMIT_ON_SUCCESS);
        $this->m_row = 0;

        /* error in query */
        if (!$this->m_query_id)
        {
          $error = OCIError($this->m_link_id);
          $this->m_errno = $error["code"];
          $this->m_error = $error["message"];
          $this->halt("Invalid SQL: $query");
        }

        /* return query id */
        return $this->m_query_id;
      }
      return 0;
    }

    /**
     * Goto the next record in the result set
     * @return result of going to the next record
     */
    function next_record()
    {
      /* goto next record */
      $this->m_record = array();

      /* get record data */
      if (@OCIFetchInto($this->m_query_id, $result, OCI_ASSOC|OCI_RETURN_NULLS))
      {
        for ($i = 1; $i <= OCINumcols($this->m_query_id); $i++)
        {
          $column = strtoupper(OCIColumnname($this->m_query_id, $i));
          $column_return = strtolower($column);
          $this->m_record[$column_return] = $result[$column];
        }
        $m_row++;

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
     * Goto a certain position in result set.
     * Not specifying a position will set the pointer
     * at the beginning of the result set.
     * @param $position the position
     */
    function seek($position=0)
    {
      if ($this->num_rows() > $position) $this->m_row = $position;
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
     * Unlock table(s) in the database
     * @return result of unlocking
     */
    function unlock()
    {
      return $this->query("commit");
    }

    /**
     * Evaluate the result; which rows were
     * affected by the query.
     * @return affected rows
     */
    function affected_rows()
    {
      return $this->num_rows();
    }

    /**
     * Evaluate the result; how many rows
     * were affected by the query.
     * @return number of affected rows
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
        $query = "SELECT seq_".$sequence.".nextval AS nextid FROM dual";
  
        /* execute query */
        $m_query_id = OCIParse($this->m_link_id, $query);
        $id = @OCIExecute($m_query_id,OCI_COMMIT_ON_SUCCESS);
  
        /* error? */
        if (empty($id))
        {
          /* create sequence */
          $query = "CREATE SEQUENCE seq_".$sequence." INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER";
          $m_query_id = OCIParse($this->m_link_id, $query);
          $id = @OCIExecute($m_query_id,OCI_COMMIT_ON_SUCCESS) or $this->halt("cannot create 'seq_$sequence'");
          
          /* try again */
          $query = "SELECT seq_".$sequence.".nextval AS nextid FROM dual";
          $m_query_id = OCIParse($this->m_link_id, $query);
          $id = @OCIExecute($m_query_id) or $this->halt("cannot get nextval from sequence 'seq_$sequence'");
          
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
     * Return the meta data of a certain table
     * @param $table the table name
     * @param $full all meta data or not
     * @return array with meta data
     */
    function metadata($table, $full=false)
    {
      /* query for data */
      $this->query("SELECT 
                      T.table_name,T.column_name,T.data_type,
                      T.data_length,T.data_precision,T.data_scale,T.nullable,
                      T.char_col_decl_length,I.index_name
                   FROM ALL_TAB_COLUMNS T,ALL_IND_COLUMNS I
                   WHERE 
                     T.column_name=I.column_name (+)
                     AND T.table_name=I.table_name
                     AND T.table_name=UPPER('$table') 
                   ORDER BY T.column_id");

      /* result */
      $result = array();
      for ($i = 0; $this->next_record(); $i++)
      {
        $result[$i]["table"]  = $this->m_record["table_name"];
        $result[$i]["name"]   = strtolower($this->m_record["column_name"]);
        $result[$i]["type"]   = strtolower($this->m_record["data_type"]);
        $result[$i]["len"]    = $this->m_record["data_length"];

        if ($this->m_record["index_name"]) $result[$i]["flags"] = "INDEX ";
        if ($this->m_record["nullable"] == "N") $result[$i]["flags"] .= "NOT NULL";
        $result[$i]["format"] = (int)$this->m_record["data_precision"].",".
                                (int)$this->m_record["data_scale"];
        if ($result[$i]["format"] == "0,0") $result[$i]["format"]="";
        $result[$i]["index"]  = $this->m_record["index_name"];
        $result[$i]["chars"]  = $this->m_record["char_col_decl_length"];
        if ($full)
        {
          $result["meta"][$result[$i]["name"]] = $i;
          $result["num_fields"] = $i;
        } 
      }

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
      $this->query("SELECT table_name, tablespace_name FROM user_tables");

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
     * This function checks the database for a table with 
     * the provide name
     *
     * @params $tableName the table to find
     * @params returns true if found, false if not found
     */  
    function tableExists($table)
    {      
      global $g_db;
      
      $found = $g_db->getrows("SELECT count(*) as found FROM cat WHERE table_name='".strtoupper($table)."'");      
      return ($found[0]['found']==0 ? false : true);
    }    
    
    function func_now()
    {
      return "SYSDATE";
    }  
    
    /**
      * Returns the maximum length an identifier (tablename, columnname, etc) may have
      *      
      */
    function maxIdentifierLength()
    {
      return 30;
    }
    
  }

?>
