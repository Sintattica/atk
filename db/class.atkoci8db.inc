<?php

  require_once("atk/db/class.atkdb.inc");

  /**
   * Handles database connectivity and database interaction
   * with the Oracle database server. Portions of this class are
   * based on the db_oci8.inc file of PHPLib.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   *
   * $Id$
   * $Log$
   * Revision 4.4  2001/06/27 15:25:43  peter
   * A lot of fixes to the oci8 support, including better outer joins, and
   * limitting queries! Fields now have an asterix behind them if they are
   * required. Some other small enhancements.
   *
   * Revision 4.3  2001/06/07 12:36:26  ivo
   * Atknode now reads records atomic.
   * Removed some debug echo's from the db classes.
   *
   * Revision 4.2  2001/05/14 10:20:19  ivo
   * Improved error reporting when database queries fail.
   * New feature: atk can now send a detailed error report by email.
   *
   * Revision 4.1  2001/05/01 09:49:49  ivo
   * Replaced all require() and include() calls by require_once() and
   * include_once() calls. The if(!DEFINED)... inclusion protection in files
   * is now obsolete.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:11  ivo
   * Added atk4 to repository.
   *
   * Revision 1.1  2001/03/21 11:00:20  peter
   * new database management classes
   *
   */
  class atkoci8db extends atkdb
  {
    /* identification */
    var $m_type     = "oci8";
    var $m_revision = '$Revision$';

    /* oci8 specific parameters */
    var $m_ora_home = "/usr/local/oracle/8i/u01/app/oracle/product/8.1.5";
    var $m_ora_sid  = "CDB";

    /**
     * Base constructor
     */
    function atkoci8db()
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

      /* establish connection to database */
      if (empty($this->m_link_id))
      {
        $this->m_link_id = OCIplogon($user, $password, $database);
        if (!$this->m_link_id) $this->halt("connect using $user / ***** on $database failed.");
      }

      /* set default date format for Oracle */ 
      $nls = OCIParse($this->m_link_id, "alter session set nls_date_format='YYYY-MM-DD'");
      $myresult = OCIExecute($nls);

      /* return link identifier */
      return $this->m_link_id;
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
      $this->connect();

      /* free old results */
      if ($this->m_query_id)
      {
        @OCIFreeStatement($this->m_query_id);
        $this->m_query_id = 0;
      }      

      /* query database */
      $this->m_query_id = OCIParse($this->m_link_id, $query);
      if ($this->m_query_id) @OCIExecute($this->m_query_id);
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
      $this->connect();

      /* lock */
      if ($mode == "write")
      {
        $m_query_id = OCIParse($this->m_link_id, "lock table $table in row exclusive mode");
        $result = @OCIExecute($m_query_id);
        if (!$result) $this->halt("$mode lock on $table failed.");
      }

      /* return result */
      return 1;
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
      $this->connect();

      /* get sequence number and increment */
      $query = "SELECT seq_".$sequence.".nextval AS nextid FROM dual";

      /* execute query */
      $m_query_id = OCIParse($this->m_link_id, $query);
      $id = @OCIExecute($m_query_id);

      /* error? */
      if (empty($id))
      {
        /* create sequence */
        $query = "CREATE SEQUENCE seq_$sequence INCREMENT BY 1 START WITH 1 NOCYCLE NOORDER";
        $m_query_id = OCIParse($this->m_link_id, $query);
        $id = @OCIExecute($m_query_id) or $this->halt("cannot create 'seq_$sequence'");
        
        /* try again */
        $query = "SELECT seq_".$sequence.".nextval AS nextid FROM dual";
        $m_query_id = OCIParse($this->m_link_id, $query);
        $id = @OCIExecute($m_query_id) or $this->halt("cannot get nextval from sequence 'seq_$sequence'");
        
        /* empty? */
        if (empty($id)) return 0;
      }

      /* get nextid */
      OCIFetchInto($m_query_id, &$result);

      /* return id */
      return $result[0];

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
      $this->query("SELECT T.table_name,T.column_name,T.data_type,".
                   "T.data_length,T.data_precision,T.data_scale,T.nullable,".
                   "T.char_col_decl_length,I.index_name".
                   " FROM ALL_TAB_COLUMNS T,ALL_IND_COLUMNS I".
                   " WHERE T.column_name=I.column_name (+)".
                   " AND T.table_name=I.table_name (+)".
                   " AND T.table_name=UPPER('$table') ORDER BY T.column_id");

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
    
  }

?>
