<?php

  require_once(atkconfig("atkroot")."atk/db/class.atkoci8db.inc");

  /**
   * Handles database connectivity and database interaction
   * with the Oracle 9i database server. 
   *
   * This class is based on oci8db, but makes use of some
   * featurss that where introduced in Oracle 9i
   *
   * @author Maurice Maas <maurice@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$   
   *
   */
  class atkoci9db extends atkoci8db
  {        
    var $m_bindresult = array();

    var $m_type = "oci9";
    
    /**
     * Base constructor
     */
    function atkoci9db()
    {
      return $this->atkoci8db();
    }

    /**
     * Connect to the database
     * @param $database the database name
     * @param $host database hostname
     * @param $user database username
     * @param $password database password
     * @return link identifier
     */
    function connect()
    {              
      /* establish connection to database */
      if (empty($this->m_link_id))
      {
        $connectfunc = (atkconfig("databasepersistent")?"OCIPlogon":"OCILogon");
        atkdebug("Connectiontype: ".$connectfunc);
        $this->m_link_id = $connectfunc($this->m_user, $this->m_password, $this->m_database);
        if (!$this->m_link_id) 
        {
          $err = OCIError();
          $this->halt($this->_translateError($err));
        }
  
        /* set default date format for Oracle */ 
        $nls = OCIParse($this->m_link_id, "alter session set nls_date_format='YYYY-MM-DD'");
        $myresult = OCIExecute($nls);
        $error = OCIError($this->m_link_id);
        if ($err["code"]!="")
        {
          $this->halt("Invalid SQL: $query"); 
          return $this->_translateError($err);
        }
        
        /* set default timestamp format for Oracle */ 
        // This should work, because it works in sqlplus
        // Is this a PHP bug
        // Workarround: see class.atkoci9query.inc and class.atkdatetimeattribute.inc
        $nls = OCIParse($this->m_link_id, "alter session set nls_timestamp_format='YYYY-MM-DD HH24:MI:SS'");
        $myresult = OCIExecute($nls);
        $err = OCIError($this->m_link_id);
        if ($err["code"]!="")
        {
          $this->halt("Invalid SQL: $query"); 
          return $this->_translateError($err);
        }
      }
      return DB_SUCCESS;
    }  

    /**
     * Performs a query
     * @param $query the query
     * @param $offset offset in record list
     * @param $limit maximum number of records
     */
    function query($query, $offset=-1, $limit=-1, $bindLobs=array(), $bindVars=array())
    {    
      /* limit? */
      if ($offset >= 0 && $limit > 0)
      {
        /* row id's start at 1! */
	      $query="SELECT * FROM (SELECT rownum AS rid, XX.* FROM (".$query.
                ") XX) YY  WHERE YY.rid >= ".($offset+1)." AND YY.rid <= ".($offset + $limit);
      }	
      
      /* query */
      atkdebug("atkoci9db.query(): ".$query);   
      
      /* connect to database */
      if ($this->connect()==DB_SUCCESS)
      {
        /* free old results */
        if ($this->m_query_id)
        {
          @OCIFreeStatement($this->m_query_id);
          $this->m_query_id = 0;    
          $this->m_bindresult = array();
        }      

        /* query database */     
        if (count($bindLobs))
        {      
          $i=0;
          foreach($bindLobs as $bind => $value)
          {          
            $clobVar = $bind.$i;
            $$clobVar = OCINewDescriptor($this->m_link_id, OCI_D_LOB);                  
            $i++;          
            atkdebug("NewDescriptor for ".$bind);
          }
          $fieldNames = array();
          $bindNames = array();
          $fieldNames = array_keys($bindLobs);
          foreach($fieldNames as $name) $bindNames[] = ":".$name;        
          $query .= " RETURNING ".implode(",",$fieldNames)." INTO ".implode(",",$bindNames);        
        }      
      
        $this->m_query_id = OCIParse($this->m_link_id, $query);
        if ($this->m_query_id) 
        {              
          if (count($bindLobs))
          {        
            $i=0;
            foreach($bindLobs as $bind => $value)
            {          
              $clobVar = $bind.$i;
              OCIBindByName($this->m_query_id, $bind, &$$clobVar, -1, OCI_B_CLOB);
              $i++;          
              atkdebug("Binding CLOB ".$bind);
            }          
          }    
        
          for ($i=0, $_i=count($bindVars);$i<$_i;$i++)   
          {          
            OCIBindByName($this->m_query_id, $bindVars[$i], $this->m_bindresult[$bindVars[$i]], 32);
          }    
        
          if (!OCIExecute($this->m_query_id,OCI_DEFAULT))
          {
            /* error in query */
            $error = OCIError($this->m_query_id);
            $this->halt("Invalid SQL: $query"); 
            $this->halt($this->_translateError($error));
            return false;
          }
          else
          {        
            if (count($bindLobs))
            {
              $i=0;
              foreach($bindLobs as $bind => $value)
              {      
                $clobVar = $bind.$i;                      
                if (!@$$clobVar->save($value))
                  atkerror("Error bind save: empty value");
                $i++;
              }
            }          
          }
          if (count($bindLobs))
          {
            $i=0;
            foreach($bindLobs as $bind => $value)
            {   
              $clobVar = $bind.$i;                                
              OCIFreeDesc($$clobVar);
              $i++;
            }

          }          
          $this->m_row = 0;
          return true;
        }
        else
        {
          /* error in query */
          $error = OCIError($this->m_query_id);
          $this->halt("Invalid SQL: $query"); 
          $this->halt($this->_translateError($error));
          return false;
        }          
      
      }
      return false;
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
                    FROM 
                      ALL_IND_COLUMNS I
                      JOIN ALL_TAB_COLUMNS T ON
                        T.table_name=I.table_name
                        AND T.table_name=UPPER('$table')
                    ORDER BY T.column_id");

      /* result */
      $result = array();
      for ($i = 0; $this->next_record(); $i++)
      {
        $result[$i]["table"]  = $this->m_record["table_name"];
        $result[$i]["name"]   = strtolower($this->m_record["column_name"]);
        $result[$i]["type"]   = $this->getGenericType($this->m_record["data_type"]);
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
        
  }

?>