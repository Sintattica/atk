<?php

  /**
   * This class builds queries. Sometimes databases require specific, non
   * ansi compliant SQL. The code for this should go into classes
   * that extend this class.
   * @author Ivo Jansch (ivo@achievo.com)
   * @version 0.5
   *
   * @changes:
   *
   * 0.5        - fixed a bug in the quotes feature from 0.4 (Ivo)
   * 0.4        - addField has a new parameter that indicates whether to use
   *              quotes ('') in insert/update queries (Ivo)
   * 0.3        - Oracle support (atkoci8query) (Sandy)
   * 0.2        - short aliases for fieldnames in query (oracle can't handle
   *              long ones) (Ivo)
   * $Id$
   * $Log$
   * Revision 4.5  2001/07/15 16:37:19  ivo
   * New atk.inc includefile in skel.
   * New feature: extended search.
   * Fixed a bug in session management; style.php and other files that get
   * loaded between two dispatches could corrupt a session.
   *
   * Revision 4.4  2001/07/10 09:17:01  ivo
   * Fixed a bug with non-left joins in mysql.
   *
   * Revision 4.3  2001/06/12 12:25:35  ivo
   * New feature: 'group by' for queries.
   *
   * Revision 4.2  2001/05/07 15:13:49  ivo
   * Put config_atkroot in all files.
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
   * Revision 1.4  2001/04/05 11:02:50  ivo
   * Readded some of the removed code from previous revision, because
   * the code was necessary when using where clauses on joined tables.
   *
   * Revision 1.3  2001/04/05 10:46:42  ivo
   * Optimized the record count feature. (Removed buildCount and a lot of
   * duplicate code)
   *
   * Revision 1.2  2001/04/05 10:19:07  peter
   * new way of navigation through the record list
   *
   * Revision 1.1.1.1  2001/01/10 13:57:58  sandy
   * Achievo Tool Kit
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

    /**
    * Array with aliases
    */
    var $m_aliases;

    /**
    * Array with field aliases
    */
    var $m_fieldaliases;

    /**
    * Array with Joins
    */
    var $m_joins;
    
    /**
     * Array with group by statements
     */
    var $m_groupbys;
    
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
      $this->m_fields = Array();
      $this->m_tables = Array();
      $this->m_conditions = Array();
      $this->m_aliases = Array();
      $this->m_values = Array();
      $this->m_fieldaliases = Array();
      $this->m_joins = Array();
      
      // start at 'A'.
      $this->m_generatedAlias = "A";
      
      $this->m_aliasLookup = Array();
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

    /**
    * Add condition to condition Array
    * @param $condition Condition
    */
    function addCondition($condition)
    {
      if ($condition!="") $this->m_conditions[] = $condition;
    }        

    /**
    * Builds the SQL Select query
    * @result a SQL Select Query
    */
    function buildSelect()
    {
      $result = "SELECT ";
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
      
      if (count($this->m_groupbys)>0)
      {
        $result.= " GROUP BY ".implode(", ",$this->m_groupbys);
      }
      return $result;
    }
    
    /**
     * Builds the SQL Select COUNT(*) query. This is different from select,
     * because we do joins, like in a select, but we don't really select the
     * fields.
     *
     * @result a SQL Select COUNT(*) Query
     */
     function buildCount()
     {
       $result = "SELECT COUNT(*) as count FROM ";

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
    * Search Alias in alias array
    * @param $record Array with fields
    * @result $record with correct value
    */
    function deAlias(&$record)
    {
      while (list($name,$value) = each($record))
      {
        if(substr($name,0,3)=="al_")
	      {
          $name = substr($name,0,strlen($name)-1).strtoupper(substr($name,strlen($name)-1,strlen($name)));
	      }
        if ($this->m_aliasLookup[$name]!="") $record[$this->m_aliasLookup[$name]]=$value;
      }     
    }
    
    /**
     * Generate a searchcondition that checks whether $value matches $field exactly.
     */ 
    function exactCondition($field, $value)
    {
      return "UPPER(".$field.")=UPPER('".$value."')";
    }
    
    /**
     * Generate a searchcondition that checks whether $field contains $value .
     */ 
    function substringCondition($field, $value)
    {
      return "UPPER(".$field.") LIKE UPPER('%".$value."%')";
    }
    
    /**
     * Generate a searchcondition that accepts '*' as wildcard character.
     */ 
    function wildcardCondition($field, $value)
    {
      return "UPPER(".$field.") LIKE UPPER('".str_replace("*","%",$value)."')";
    }
    
  }

  $name = $config_atkroot."atk/db/class.atk".$GLOBALS["config_database"]."query.inc";
  include_once($name);

?>
