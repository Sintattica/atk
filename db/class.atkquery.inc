<?php

  /**
   * This class builds queries. Sometimes databases require specific, non
   * ansi compliant SQL. The code for this should go into classes
   * that extend this class.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
   *
   */

  class atkquery
  {
    /**
    * Array with Fieldnames
    */
    var $m_fields;

    /**
    * Array with tables
    */
    var $m_tables;

    /**
    * Array with conditions
    */
    var $m_conditions;
    var $m_searchconditions;

    /**
    * Var with AND or OR method
    */
    var $m_searchmethod;

    /**
    * Array with aliases
    */
    var $m_aliases;

    /**
    * Array with field aliases
    */
    var $m_fieldaliases;

    /**
    * Array with aliases from joins
    */
    var $m_joinaliases;


    /**
    * Array with Joins
    */
    var $m_joins;

    /**
     * Array with group by statements
     */
    var $m_groupbys;

    /**
     * Array with order by statements
     */
    var $m_orderbys;

    /**
    * Array with generated aliasses
    * Oracle has a problem when aliases are too long
    */
    var $m_generatedAlias;

    /**
    * Initialize all variables
    */
    function atkquery()
    {
      $this->m_fields = array();
      $this->m_tables = array();
      $this->m_conditions = array();
      $this->m_searchconditions = array();
      $this->m_aliases = array();
      $this->m_values = array();
      $this->m_fieldaliases = array();
      $this->m_joinaliases = array();
      $this->m_joins = array();
      $this->m_orderbys = array();
      $this->m_groupbys = array();
      $this->m_searchmethod = "";

      // start at 'A'.
      $this->m_generatedAlias = "A";

      $this->m_aliasLookup = array();
    }

    /**
    * Add's a field to the query
    * @param $name Field name
    * @param $value Field value
    * @param $table Table name
    * @param $fieldaliasprefix Field alias prefix
    * @param $quote If this parameter is true, stuff is inserted into the db
    *               using quotes, e.g. SET name = 'piet'. If it is false, it's
    *               done without quotes, e.d. SET number = 4.
    */
    function addField($name, $value="", $table="", $fieldaliasprefix="", $quote=true)
    {
      //$this->m_fields[] = strtr($name,"_",".");
      if ($table!="") $fieldname = $table.".".$name;
      else $fieldname = $name;
      $this->m_fields[] = $fieldname;

      if ($quote||$value=="") $value = "'".$value."'";

      $this->m_values[$fieldname] = $value;

      if ($fieldaliasprefix!="")
      {
        $this->m_aliasLookup["al_".$this->m_generatedAlias] = $fieldaliasprefix.$name;
        $this->m_fieldaliases[$fieldname] = "al_".$this->m_generatedAlias;

        $this->m_generatedAlias++;
      }
    }

    /**
    * Add table to Tables array
    * @param $name Table name
    * @param $alias Alias of table
    */
    function addTable($name, $alias = "")
    {
      $this->m_tables[] = $name;
      $this->m_aliases[count($this->m_tables)-1] = $alias;
    }

    /**
    * Add join to Join Array
    * @param $table Table name
    * @param $alias Alias of table
    * @param $condition Condition for the Join
    */
    function addJoin($table, $alias, $condition, $outer)
    {
    
      if ($outer)
      {
        $join="LEFT JOIN ";
        $this->m_joins[]= " ".$join.$table." ".$alias." ON ".$condition." ";
      }
      else
      {
        $join="JOIN ";
        $this->m_joins[]= " ".$join.$table." ".$alias;
        $this->addCondition($condition);
      }

    }

    function addGroupBy($element)
    {
      $this->m_groupbys[] = $element;
    }

    function addOrderBy($element)
    {
      $this->m_orderbys[] = $element;
    }

    /**
    * Add condition to condition Array
    * @param $condition Condition
    */
    function addCondition($condition)
    {
      if ($condition!="") $this->m_conditions[] = $condition;
    }

  /**
    * Add search condition to search condition Array
    * @param $condition Condition
    */
    function addSearchCondition($condition)
    {
      if ($condition!="") $this->m_searchconditions[] = $condition;
    }

    /**
    * Builds the SQL Select query
    * @result a SQL Select Query
    * @param $distinct distinct records?
    */
    function buildSelect($distinct = FALSE)
    {
      $result = "SELECT ".($distinct ? "DISTINCT ": "");
      for ($i=0;$i<count($this->m_fields);$i++)
      {
        $result.= $this->m_fields[$i];
        $fieldalias = $this->m_fieldaliases[$this->m_fields[$i]];
        if ($fieldalias!="") $result.=" AS ".$fieldalias;
        if ($i < count($this->m_fields)-1) $result.=",";
      }
      $result.= " FROM ";
      for ($i=0;$i<count($this->m_tables);$i++)
      {
        $result.= $this->m_tables[$i];
        if ($this->m_aliases[$i]!="") $result.=" ".$this->m_aliases[$i];
        if ($i < count($this->m_tables)-1) $result.=",";
      }

      for ($i=0;$i<count($this->m_joins);$i++)
      {
        $result.=$this->m_joins[$i];
      }

      if (count($this->m_conditions)>0)
      {
          $result.= " WHERE ".implode(" AND ",$this->m_conditions);
      }

      if (count($this->m_searchconditions)>0)
      {
        $prefix=" ";
        if(count($this->m_conditions)==0) { $prefix=" WHERE "; } else { $prefix=" AND "; }
        if($this->m_searchmethod==""||$this->m_searchmethod=="AND")
        {
          $result.= $prefix."(".implode(" AND ",$this->m_searchconditions).")";
        }
        else
        {
          $result.= $prefix."(".implode(" OR ",$this->m_searchconditions).")";
        }
      }

      if (count($this->m_groupbys)>0)
      {
        $result.= " GROUP BY ".implode(", ",$this->m_groupbys);
      }

      if (count($this->m_orderbys)>0)
      {
        $result.= " ORDER BY ".implode(", ",$this->m_orderbys);
      }          
      
      return $result;
    }

    /**
     * Builds the SQL Select COUNT(*) query. This is different from select,
     * because we do joins, like in a select, but we don't really select the
     * fields.
     *
     * @param $distinct distinct rows?
     *
     * @result a SQL Select COUNT(*) Query
     */
     function buildCount($distinct = FALSE)
     {
       if ($distinct && count($this->m_fields) > 0)
        $result = "SELECT COUNT(DISTINCT ".implode($this->m_fields, ",").") as count FROM ";
       else $result = "SELECT COUNT(*) as count FROM ";

       for ($i=0;$i<count($this->m_tables);$i++)
       {
         $result.= $this->m_tables[$i];
         if ($this->m_aliases[$i]!="") $result.=" ".$this->m_aliases[$i];
         if ($i < count($this->m_tables)-1) $result.=",";
       }

       for ($i=0;$i<count($this->m_joins);$i++)
       {
         $result.=$this->m_joins[$i];
       }

      if (count($this->m_conditions)>0)
      {
        $result.= " WHERE ".implode(" AND ",$this->m_conditions);
      }

      if (count($this->m_searchconditions)>0)
      {
        $prefix=" ";
        if(count($this->m_conditions)==0) { $prefix=" WHERE "; } else { $prefix=" AND "; };
        if($this->m_searchmethod==""||$this->m_searchmethod=="AND")
        {
          $result.= $prefix."(".implode(" AND ",$this->m_searchconditions).")";
        }
        else
        {
          $result.= $prefix."(".implode(" OR ",$this->m_searchconditions).")";
        }
      }

       if (count($this->m_groupbys)>0)
       {
         $result.= " GROUP BY ".implode(", ",$this->m_groupbys);
       }
       return $result;
     }


 /**
    * Builds the SQL Update query
    * @result a SQL Update Query
    */
    function buildUpdate()
    {
      $result = "UPDATE ".$this->m_tables[0]." SET ";

      for ($i=0;$i<count($this->m_fields);$i++)
      {
        $result.= $this->m_fields[$i]."=".$this->m_values[$this->m_fields[$i]];
        if ($i < count($this->m_fields)-1) $result.=",";
      }
      if (count($this->m_conditions)>0)
      {
        $result.= " WHERE ".implode(" AND ",$this->m_conditions);
      }
      return $result;
    }

 /**
    * Wrapper function to execute an update query 
    */
    function executeUpdate()
    {
      global $g_db;
      
      $query = $this->buildUpdate();
      $g_db->query($query);      
    }

 /**
    * Wrapper function to execute an insert query 
    */
    function executeInsert()
    {
      global $g_db;
      
      $query = $this->buildInsert();
      $g_db->query($query);      
    }

    /**
    * Builds the SQL Insert query
    * @result a SQL Insert Query
    */
    function buildInsert()
    {
      $result = "INSERT INTO ".$this->m_tables[0]." (";

      for ($i=0;$i<count($this->m_fields);$i++)
      {
        $result.= $this->m_fields[$i];
        if ($i < count($this->m_fields)-1) $result.=",";
      }

      $result.=") VALUES (";

      for ($i=0;$i<count($this->m_fields);$i++)
      {
        $result.= $this->m_values[$this->m_fields[$i]];
        if ($i < count($this->m_fields)-1) $result.=",";
      }

      $result.=")";

      return $result;
    }

    /**
     * Builds the SQL Delete query
     * @result a SQL Delete Query
     */
    function buildDelete()
    {
      $result = "DELETE FROM ".$this->m_tables[0];

      if (count($this->m_conditions)>0)
      {
        $result.= " WHERE ".implode(" AND ",$this->m_conditions);
      }

      return $result;
    }

 /**
    * Search Alias in alias array
    * @param $record Array with fields
    * @result $record with correct value
    */
    function deAlias(&$record)
    {
      foreach ($record as $name => $value)
      {
        if(substr($name,0,3)=="al_")
        {
          $name = substr($name,0,3).strtoupper(substr($name,3));
        }
        if ($this->m_aliasLookup[$name]!="")
        {
          $record[$this->m_aliasLookup[$name]]=$value;
          unset($record[strtolower($name)]);
        }
      }
    }

  /**
     * Generate a searchcondition that checks whether $value matches $field exactly.
     */
    function exactCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return "UPPER(".$field.")!=UPPER('".substr($value,1,strlen($value))."')";
      }
      else
      {
        return "UPPER(".$field.")=UPPER('".$value."')";
      }
    }

    /**
     * Generate a searchcondition that checks whether $field contains $value .
     */
    function substringCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return "UPPER(".$field.") NOT LIKE UPPER('%".substr($value,1,strlen($value))."%')";
      }
      else
      {
        return "UPPER(".$field.") LIKE UPPER('%".$value."%')";
      }
    }

    /**
     * Generate a searchcondition that accepts '*' as wildcard character.
     */
    function wildcardCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return "UPPER(".$field.") NOT LIKE UPPER('".str_replace("*","%",substr($value,1,strlen($value)))."')";
      }
      else
      {
        return "UPPER(".$field.") LIKE UPPER('".str_replace("*","%",$value)."')";
      }
    }

    /**
        *  generate searchcondition with greater than
        */
    function greaterthanCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return $field." < '".substr($value,1,strlen($value))."'";
      }
      else
      {
        return $field." > '".$value."'";
      }
    }

   /**
        *  generate searchcondition with greater than
        */
    function greaterthanequalCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return $field." < '".substr($value,1,strlen($value))."'";
      }
      else
      {
        return $field." >= '".$value."'";
      }
    }

    /**
        *  generate searchcondition with less than
        */
    function lessthanCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return $field." > '".substr($value,1,strlen($value))."'";
      }
      else
      {
        return $field." < '".$value."'";
      }
    }

    /**
        *  generate searchcondition with less than
        */
    function lessthanequalCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return $field." > '".substr($value,1,strlen($value))."'";
      }
      else
      {
        return $field." <= '".$value."'";
      }
    }
  }

  $name = $config_atkroot."atk/db/class.atk".$GLOBALS["config_database"]."query.inc";
  include_once($name);

?>
