<?php

  /**
   * IBM i5-specific databasedriver. This driver is based on the db2 driver but
   * uses the i5_ library instead of the db2_ library. This allows us not just
   * execute queries, but also to run some commands on the i5 machine. This is
   * used to set the librarylist on-the-fly, for example.
   *
   * You can supply a librarylist in your database configuration by adding
   * librarylist to your database configuration:
   *
   * $db['default']['librarylist'] = array('MYLIBL', 'HISLIBL');
   *
   * @author Harrie Verveer <harrie@ibuildings.nl>
   */

  class atki5db extends atkdb2db 
  {
    function atki5db()
    {
      $this->atkdb2db();
      $this->m_type = "i5";
      $this->m_vendor = "i5";
    }
    
    /**
     * Connect to the i5 machine
     *
     * @param string $host Hostname
     * @param string $user Username
     * @param string $password Password
     * @return mixed Connection status
     */
    function doConnect($host,$user,$password,$database,$port,$charset)
    {
      /* establish connection */
      if (empty($this->m_link_id))
      {
        if (empty($port))
          $port = 50000;

        $options = array(I5_OPTIONS_SQLNAMING=>"SQL",I5_OPTIONS_INITLIBL=>"ALGDEVDTA SYSIBM");
        $dbconfig = atkconfig("db");
        $dbconfig = $dbconfig[$this->m_connection];
                  
        $this->m_link_id = i5_connect($host,$user,$password);

        if (!$this->m_link_id)
        {
          $this->halt($this->getErrorMsg());
        }
        if (array_key_exists('librarylist',$dbconfig))
          $this->add_to_librarylist($dbconfig['librarylist']);
      }        
      return DB_SUCCESS;
    }    
    
    function add_to_librarylist($libraries)
    {
      $curlibl = "";
      
      // get current librarylist
      if (!i5_command("rtvjoba",array(),array("usrlibl"=>"curlibl"),$this->m_link_id))
        return atkdebug("Could not retrieve current librarylist:" . i5_errormsg($this->m_link_id));
      
      atkdebug("Current librarylist is $curlibl");
        
      // add our libraries to the librarylist
      $curlibl .= " " . implode(" ",$libraries);
      
      if(!i5_command("chglibl",array("libl"=>$curlibl),array(),$this->m_link_id))
        return atkdebug("Could not change current librarylist:" . i5_errormsg($this->m_link_id));
        
      atkdebug("Changed librarylist to: $curlibl");
    }
    
    /**
     * Get errornumber and errormessage from the i5 machine and store them
     * in the errno and error member variables
     */
    function _setErrorVariables()
    {
      if (!empty($this->m_link_id))
      {
        $this->m_error = i5_error($this->m_link_id);
        $this->m_errno = i5_errormsg($this->m_link_id);
        
      }
      else
      {
        $this->m_error = i5_error();
        $this->m_errno = i5_errormsg();
      }
      
      if(is_array($this->m_error) && count($this->m_error) > 2)
      {
        $this->m_error = $this->m_error[3];
      }
    }

    /**
     * Disconnect from i5 machine
     */
    function disconnect()
    {
      if ($this->m_link_id)
      {
        atkdebug("Disconnecting from database...");
        @i5_close($this->m_link_id);
        $this->m_link_id = 0;
      }
    }    

   /**
     * Performs a query
     * @param $query the query
     * @param $offset offset in record list
     * @param $limit maximum number of records
     * @return boolean True on success, false on failure
     */
    function query($query, $offset=-1, $limit=-1)
    {
      atkimport("atk.utils.atkdebugger");
      atkdebug("Query:".$query);

      /* connect to database */
      if ($this->connect($mode)==DB_SUCCESS)
      {
        /* free old results */
        if ($this->m_query_id)
        {
          // free result here if resource
          $this->m_query_id = 0;
        }

        $this->m_affected_rows = 0;

        /* query database */
        $this->m_query_id = @i5_query($query,$this->m_link_id);

        /* invalid query */
        if (!$this->m_query_id)
        {
          $this->_setErrorVariables();
          $this->halt("Invalid SQL: $query.");
          return false;
        }

        $this->m_affected_rows = i5_affected_rows($this->m_link_id);
        $this->m_row = 0;

        /* return query id */
        atkdebug("atki5db::return true<br>");
        return true;
      }
      return false;
    }  
            
    /**
     * Goto the next record in the result set
     * @return result of going to the next record
     */
    function next_record($row_number = null)
    {
      /* goto next record */
      if (!is_null($row_number)) i5_data_seek($this->m_query_id,$row_number);
      $this->m_record = i5_fetch_assoc($this->m_query_id);
      
      if (isset($this->m_record) && is_array($this->m_record))
      {
        foreach($this->m_record as $key => $value)
        {
          // convert keys to lowercase or ATK will freak out
          // TODO: is there a way to achievo this by passing a
          // param to i5_connect (like DB2_ATTR_CASE for db2_connect?)
          if (strtolower($key) != $key)
          {
            $this->m_record[strtolower($key)] = $this->m_record[$key];
            unset($this->m_record[$key]);
          }
          
          if (is_string($value))
            $this->m_record[strtolower($key)] = trim($value);
        }
      }
      
      $this->m_row++;
      $this->m_errno = i5_error($this->m_link_id);
      $this->m_error = i5_errormsg($this->m_link_id);

      /* are we there? */
      $result = is_array($this->m_record);
      if (!$result && $this->m_auto_free)
      {
        // should say free result here (but isn't there for i5)
        $this->m_query_id = 0;
      }
      
      /* return result */
      return $result;
    }
    
    function seek($row_number=0)
    {
      i5_data_seek($this->m_query,$row_number);
    }

    /**
     * Evaluate the result; which rows were
     * affected by the query.
     * @return affected rows
     */
    function affected_rows()
    {
      return @i5_num_rows($this->m_query_id);
    }

    /**
     * Evaluate the result; how many rows
     * were affected by the query.
     * @return number of affected rows
     */
    function num_rows()
    {
      return @i5_num_rows($this->m_query_id);
    }

    /**
     * Evaluatie the result; how many fields
     * where affected by the query.
     * @return number of affected fields
     */
    function num_fields()
    {
      return @i5_num_fields($this->m_query_id);
    }    
    
    
    /**
     * Return the meta data of a certain table
     * @param String $table the table name (optionally in 'database.tablename' format)
     * @param boolean $full all meta data or not
     * @return array with meta data
     */
    function metadata($table, $full=false)
    {
      $tbl = strtoupper($table);
      $result = array();
      
      if ($this->connect()==DB_SUCCESS)
      {
        $ddl = &atkDDL::create("i5");
        $query = "SELECT * FROM \"COLUMNS\" WHERE \"TABLE_NAME\" = '$tbl'";
        
        $dbconfig = atkconfig("db");
        $dbconfig = $dbconfig[$this->m_connection];
        if (array_key_exists('i5_lib',$dbconfig) && $dbconfig['i5_lib'] != "")
          $query .= " AND TABLE_SCHEMA = '{$dbconfig['i5_lib']}'";
          
        $result_metadata = $this->getRows($query);
        
        for ($i=0,$_i=count($result_metadata);$i<$_i;$i++)
        {
          $result[$i]["table"]      = strtolower($result_metadata[$i]['table_schema']) . "." . strtolower($result_metadata[$i]['table_name']);
          $result[$i]["table_type"] = NULL;
          $result[$i]["name"]       = strtolower($result_metadata[$i]['column_name']);
          $result[$i]["type"]       = $result_metadata[$i]['data_type'];
          $result[$i]["gentype"]    = $ddl->getGenericType($result_metadata[$i]['data_type']);
          $result[$i]["len"]        = $result_metadata[$i]['character_maximum_length'];
          $result[$i]["flags"]      = $this->metadataToFlags($result_metadata[$i]);

          $result[$i]["flags"] =
            (in_array('primary_key',    $result[$i]["flags"]) ? MF_PRIMARY        : 0) |
            (in_array('unique_key',     $result[$i]["flags"]) ? MF_UNIQUE         : 0) |
            (in_array('not_null',       $result[$i]["flags"]) ? MF_NOT_NULL       : 0) |
            (in_array('auto_increment', $result[$i]["flags"]) ? MF_AUTO_INCREMENT : 0);

          if ($full)
            $result["meta"][$result[$i]["name"]] = $i;
        }
      }
      return $result;
    }
    
    function nextid($sequence)
    {
      /* first connect */
      if ($this->connect("w")==DB_SUCCESS)
      {
        /* lock sequence table */
        if ($this->lock($this->m_seq_table))
        {
          /* get sequence number (locked) and increment */
          $query = "SELECT ".$this->m_seq_field." FROM ".$this->m_seq_table." WHERE ".$this->m_seq_namefield." = '$sequence'";
          $id = @i5_query($query,$this->m_link_id);
          $result = @i5_fetch_array($id);
          atk_var_dump($result,"<h2>RESULT FROM DB_SEQUENCE TABLE:</h2>");
          
          /* no current value, make one */
          if (!is_array($result))
          {
            $query = "INSERT INTO ".$this->m_seq_table." VALUES('$sequence', 1)";
            $id = @i5_query($query,$this->m_link_id);
            atkdebug("<b>Ran query $query, result is : $id</b>");
            $this->unlock();
            return 1;
          }

          /* enter next value */
          else
          {
            $nextid = $result[strtoupper($this->m_seq_field)] + 1;
            $query = "UPDATE ".$this->m_seq_table." SET ".$this->m_seq_field." = $nextid WHERE ".$this->m_seq_namefield." = '$sequence'";

            $id = @i5_query($query,$this->m_link_id);
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
     * Unlock table(s) in the database
     * @return result of unlocking
     */
    function unlock()
    {
      // we don't lock/unlock in this version of the driver
      return true;
      
      /* connect first */
      if ($this->connect("w")==DB_SUCCESS)
      {
        // In i5, the table will remain locked until the next commit or rollback. As
        // we never use rollback in ATK, lets just commit() to release all locks
        atkdebug("Query: commit");
        $result = i5_query("COMMIT",$this->m_link_id);
        atk_var_dump($result,"<h2>RESULT:</h2>");
        if (!$result) $this->halt("commit failed:" . print_r(i5_error($this->m_link_id),true));

        /* return result */
        return $result;
      }
      return 0;
    }
  }