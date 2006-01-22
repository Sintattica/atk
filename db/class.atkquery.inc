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
   * @copyright (c)2000-2006 Ibuildings.nl BV
   * @copyright (c)2000-2006 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Abstract baseclass for SQL query builder drivers.
   *
   * All db drivers should derive a class from this baseclass and implement
   * the necessary methods.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage db
   * @abstract
   */
  class atkQuery
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
     * Do we need to perform a DISTINCT query?
     */
    var $m_distinct=false;

    /**
      * Do we need to fetch only a specific set of records?
      */
    var $m_offset=0;
    var $m_limit=0;

    /**
    * Array with generated aliasses
    * Oracle has a problem when aliases are too long
    */
    var $m_generatedAlias;

    /**
     * The database that this query does it's thing on
     */
    var $m_db;

    /**
     * Quote to put around fields
     * @var String
     * @access private
     */
    var $m_fieldquote;

    var $m_quotedfields=array();

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

      // start at 'a'.
      $this->m_generatedAlias = "a";

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
    * @param $quotefield
    */
    function addField($name, $value="", $table="", $fieldaliasprefix="", $quote=true, $quotefield=false)
    {
      if ($table!="") $fieldname = $table.".".$name;
      else $fieldname = $name;
      $this->m_fields[] = $fieldname;
      if ($quotefield) $this->m_quotedfields[] = $fieldname;

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
        $jointype="LEFT JOIN ";
        $join = " ".$jointype.$table." ".$alias." ON ".$condition." ";

        if (!in_array($join, $this->m_joins)) $this->m_joins[]= $join;
      }
      else
      {
        $jointype="JOIN ";
        $join = " ".$jointype.$table." ".$alias;
        if (!in_array($join, $this->m_joins))
        {
          $this->m_joins[]= $join;
          $this->addCondition($condition);
        }
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
     * Set the 'distinct' mode for the query.
     * If set to true, a 'SELECT DISTINCT' will be performed. If set to false,
     * a regular 'SELECT' will be performed.
     */
    function setDistinct($distinct)
    {
      $this->m_distinct = $distinct;
    }

    function setLimit($offset, $limit)
    {
      $this->m_offset = $offset;
      $this->m_limit = $limit;
    }

    /**
     * Builds the SQL Select query
     * @param $distinct distinct records?
     * @return String a SQL Select Query
     */
    function buildSelect($distinct = FALSE)
    {
      $result = "SELECT ".($distinct||$this->m_distinct ? "DISTINCT ": "");
      for ($i=0;$i<count($this->m_fields);$i++)
      {
        $result.= $this->quoteField($this->m_fields[$i]);
        $fieldalias = (isset($this->m_fieldaliases[$this->m_fields[$i]])?$this->m_fieldaliases[$this->m_fields[$i]]:"");
        if ($fieldalias!="") $result.=" AS ".$fieldalias;
        if ($i < count($this->m_fields)-1) $result.=",";
      }
      $result.= " FROM ";
      for ($i=0;$i<count($this->m_tables);$i++)
      {
        $result.= $this->quoteField($this->m_tables[$i]);
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
        $this->_addOrderBy($result);
      }

      if ($this->m_limit>0)
      {
        $this->_addLimiter($result);
      }

      return $result;
    }

    /**
     * Wrapper function to execute a select query.
     * @param boolean $distinct Set to true to perform a distinct select,
     *                          false for a regular select.
     * @return array The set of records returned by the database.
     */
    function executeSelect($distinct = FALSE)
    {
      if (!isset($this->m_db)) $this->m_db = &atkGetDb();
      $query = $this->buildSelect($distinct);
      return $this->m_db->getrows($query);
    }

    /**
      * Add limiting clauses to the query.
      * Default implementation: no limit supported. Derived classes should implement this.
      */
    function _addLimiter(&$query)
    {
      // not supported..
    }

    function _addOrderBy(&$query)
    {
      if (count($this->m_orderbys)>0)
      {
        $query.= " ORDER BY ".implode(", ",$this->m_orderbys);
      }
    }

    /**
     * Builds the SQL Select COUNT(*) query. This is different from select,
     * because we do joins, like in a select, but we don't really select the
     * fields.
     *
     * @param boolean $distinct distinct rows?
     *
     * @return String a SQL Select COUNT(*) Query
     */
     function buildCount($distinct = FALSE)
     {
       if ($distinct && count($this->m_fields) > 0)
       {
        $result = "SELECT COUNT(DISTINCT ";
        $result.=implode($this->quoteFields($this->m_fields),",");
        $result.=") as count FROM ";
       }
       else $result = "SELECT COUNT(*) as count FROM ";

       for ($i=0;$i<count($this->m_tables);$i++)
       {
         $result.= $this->quoteField($this->m_tables[$i]);
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
     * @return String a SQL Update Query
     */
    function buildUpdate()
    {
      $result = "UPDATE ".$this->quoteField($this->m_tables[0])." SET ";

      for ($i=0;$i<count($this->m_fields);$i++)
      {
        $result.= $this->quoteField($this->m_fields[$i])."=".$this->m_values[$this->m_fields[$i]];
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
      if (!isset($this->m_db)) $this->m_db = &atkGetDb();
      $query = $this->buildUpdate();
      return $this->m_db->query($query);
    }

    /**
     * Wrapper function to execute an insert query
     */
    function executeInsert()
    {
      if (!isset($this->m_db)) $this->m_db = &atkGetDb();
      $query = $this->buildInsert(true);
      return $this->m_db->query($query);
    }

    /**
     * Builds the SQL Insert query
     * @return String a SQL Insert Query
     */
    function buildInsert()
    {
      $result = "INSERT INTO ".$this->quoteField($this->m_tables[0])." (";

      for ($i=0;$i<count($this->m_fields);$i++)
      {
        $result.= $this->quoteField($this->m_fields[$i]);
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
     * @return String a SQL Delete Query
     */
    function buildDelete()
    {
      $result = "DELETE FROM ".$this->quoteField($this->m_tables[0]);

      if (count($this->m_conditions)>0)
      {
        $result.= " WHERE ".implode(" AND ",$this->m_conditions);
      }

      return $result;
    }

    /**
     * Wrapper function to execute a delete query
     */
    function executeDelete()
    {
      if (!isset($this->m_db)) $this->m_db = &atkGetDb();
      $query = $this->buildDelete();
      return $this->m_db->query($query);
    }

    /**
     * Search Alias in alias array
     * @param array $record Array with fields
     */
    function deAlias(&$record)
    {
      foreach ($record as $name => $value)
      {
        if (isset($this->m_aliasLookup[$name]))
        {
          $record[$this->m_aliasLookup[strtolower($name)]]=$value;
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

    function betweenCondition($field, $value1, $value2)
    {
      return $field." BETWEEN '".$value1."' AND '".$value2."'";
    }

    /**
     * Static factory method. This method returns a new instance of a query
     * object for the current database.
     */
    function &create()
    {
      $dbconfig = atkconfig("db");
      $name = "atk" . $dbconfig["default"]["driver"] . "query";
      return new $name();
    }

    /**
     * If we set a m_fieldquote you can pass a field to this function and it will
     * quote all the identifiers (db, table, column, etc...) in the field.
     *
     * @param String $field The field to add quotes too
     * @return The quoted field, if we have a fieldquote
     */
    function quoteField($field)
    {
      if ((in_array($field,$this->m_quotedfields) || in_array($field, $this->m_tables)) && preg_match('/(^[\w\.]+)/',$field)."'")
      {
        $exploded = explode('.',$field);
        foreach ($exploded as $identifier)
        {
          $quoted[] = $this->m_fieldquote.$identifier.$this->m_fieldquote;
        }
        $field = implode('.',$quoted);
      }
      return $field;
    }

    /**
     * Quote an array of fields if m_fieldquote is set.
     * Uses $this->quoteField($field)
     *
     * @param Array $fields The fields to add quotes to
     * @return Array The quoted fields
     */
    function quoteFields($fields)
    {
      if ($this->m_fieldquote)
      {
        foreach ($fields as $key => $field)
        {
          $quoted[$key] = $this->quoteField($field);
        }
        $fields = $quoted;
      }
      return $fields;
    }
  }

?>
