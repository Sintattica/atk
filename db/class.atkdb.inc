<?php

  /**
   * Handles database connectivity and database interaction.
   *
   * $Id$
   * $Log$
   * Revision 4.3  2001/05/18 07:49:28  ivo
   * Made nextid() feature of mysqldb a bit more flexible.
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
   * Revision 1.1.1.1  2001/04/23 10:17:10  ivo
   * Added atk4 to repository.
   *
   * Revision 1.3  2001/04/05 10:19:07  peter
   * new way of navigation through the record list
   *
   * Revision 1.2  2001/03/30 10:05:56  ivo
   * Added a function for retrieving error messages.
   *
   * Revision 1.1  2001/03/21 11:00:20  peter
   * new database management classes
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

    /* identification */
    var $m_type     = "db";
    var $m_revision = '$Revision$';

    /* link and query handles */
    var $m_link_id  = 0;
    var $m_query_id = 0;

    /* sequence table */
    var $m_seq_table = "db_sequence";  
    // the field in the seq_table that contains the counter..
    var $m_seq_field = "nextid";
    // the field in the seq_table that countains the name of the sequence..
    var $m_seq_namefield = "seq_name";
    
    /* set to 1 for automatic freeing of results */    
    var $m_auto_free = 0;
    
    /* set to 1 to set debugging on */    
    var $m_debug = 0; 

    /**
     * Get the current link id
     * @return integer link id
     */
    function link_id()
    {
      return $this->m_link_id;
    }
    
    /**
     * Get the last error message
     */
    function getError()
    {      
      if ($this->m_error!="" || $this->m_errno!=0)
      {
        return $this->m_error." (errno: ".$this->m_errno.")"; 
      }
      return "";
    }
    
    function halt($message)
    {
      $dbmsg = $this->getError();
      if ($dbmsg!="") atkerror($dbmsg);
      halt($message);
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
     * @param $database the database name
     * @param $host database hostname
     * @param $user database username
     * @param $password database password
     * @return connection identifier
     */
    function connect($database="", $host="", $user="", $password="")
    {
      return $this->m_link_id;
    }

    /**
     * Disconnect from database
     */
    function disconnect()
    {
    }

    /**
     * Performs a query
     * @param $query the query
     * @param $offset offset in record list
     * @param $limit maximum number of records
     * @return the query identifier
     */
    function query($query, $offset=-1, $limit=-1)
    {
      return $this->m_query_id;    
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
     * Goto a certain position in result set.
     * Not specifying a position will set the pointer
     * at the beginning of the result set.
     * @param $position the position
     * @return result of seek
     */
    function seek($position=0)
    {
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
     * Evaluate the result; how many rows
     * were affected by the query.
     * @return number of affected rows
     */
    function num_rows()
    {
      return 0;
    }

    /**
     * Evaluatie the result; how many fields
     * where affected by the query.
     * @return number of affected fields
     */
    function num_fields()
    {
      return 0;
    }

    /**
     * Same effect as num_rows, but
     * a bit of a shorthand notation.
     * @return number of affected rows
     */
    function nf()
    {
      return $this->num_rows();
    }

    /**
     * Same effect as num_rows, but print
     * the result instead of returning it.
     */
    function np()
    {
      print $this->num_rows();
    }

    /**
     * Get the value of a certain field of
     * the currently selected record.
     * @param $name the field name
     * @return the field value
     */
    function f($name)
    {
      return $this->m_record[$name];
    }

    /**
     * Print the value of a certain field of
     * the currently selected record.
     * @param $name the field name
     * @return the field value
     */
    function p($name)
    {
      print $this->m_record[$name];
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
     * @param $table the table name
     * @param $full all meta data or not
     * @return array with meta data
     */
    function metadata($table, $full=false)
    {
      /*
        depending on $full, metadata returns the following values:
        full is false (default):
        $result[]:
          [0]["table"]  table name
          [0]["name"]   field name
          [0]["type"]   field type
          [0]["len"]    field length
          [0]["flags"]  field flags
     
        full is true:
        $result[]:
          ["num_fields"] number of metadata records
          [0]["table"]  table name
          [0]["name"]   field name
          [0]["type"]   field type
          [0]["len"]    field length
          [0]["flags"]  field flags
          ["meta"][field name] index of field named "field name"
          The last one is used, if you have a field name, but no index.
      */
    
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
     * Get all rows that are the result
     * of a certain specified query
     * @param $query the query
     * @return array with rows
     */
    function getrows($query)
    {
      $result = array();

      $this->query($query);    
      while ($this->next_record())
        $result[] = $this->m_record;
      
      return $result;
    }
  }  

?>