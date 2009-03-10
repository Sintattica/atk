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
     * Array with expressions.
     */
    var $m_expressions;

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
     * The quote char to put around fields, for example `
     * @var String
     * @access private
     */
    var $m_fieldquote;

    /**
     * Wether or not a field should be quoted in a query
     *
     * @var Array
     * @access private
     */
    var $m_quotedfields=array();

    /**
     * Names reserved by the database,
     * if any of these are used ATK MUST quote the fieldname
     * or the database engine will not be able to make any sense of the queries.
     *
     * @var Array
     * @access private
     */
    var $m_reservedNames=array('from','select','order','group','release');

    /**
    * Initialize all variables
    */
    function atkquery()
    {
      $this->m_fields = array();
      $this->m_expressions = array();
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
     * @param string $name Field name
     * @param string $value Field value
     * @param string $table Table name
     * @param string $fieldaliasprefix Field alias prefix
     * @param bool $quote If this parameter is true, stuff is inserted into the db
     *               using quotes, e.g. SET name = 'piet'. If it is false, it's
     *               done without quotes, e.d. SET number = 4.
     * @param bool $quotefield Wether or not to quote the fieldname
     * @return atkQuery The query object itself (for fluent usage)
     */
    function &addField($name, $value="", $table="", $fieldaliasprefix="", $quote=true, $quotefield=false)
    {
      if ($table!="") $fieldname = $table.".".$name;
      else $fieldname = $name;
      $this->m_fields[] = $fieldname;
      if ($quotefield) $this->m_quotedfields[] = $fieldname;

      if ($quote) $value = "'".$value."'";
      elseif ($value === NULL || $value === '') $value = 'NULL';

      $this->m_values[$fieldname] = $value;

      if ($fieldaliasprefix!="")
      {
        $this->m_aliasLookup["al_".$this->m_generatedAlias] = $fieldaliasprefix.$name;
        $this->m_fieldaliases[$fieldname] = "al_".$this->m_generatedAlias;

        $this->m_generatedAlias++;
      }

      return $this;
    }


    /**
     * Add multiple fields at once
     *
     * @param array $fields array with field value pairs
     * @param $table String Table name
     * @param $fieldaliasprefix String Field alias prefix
     * @param $quote bool If this parameter is true, stuff is inserted into the db
     *               using quotes, e.g. SET name = 'piet'. If it is false, it's
     *               done without quotes, e.d. SET number = 4.
     * @param $quotefield bool Wether or not to quote the fieldname
     * @return atkQuery The query object itself (for fluent usage)
     */
    function addFields(array $fields, $table="", $fieldaliasprefix="", $quote=true, $quotefield=false)
    {
      foreach ($fields as $name => $value)
      {
        $this->addField($name,$value,$table,$fieldaliasprefix,$quote,$quotefield);
      }
      return $this;
    }

    /**
     * Add's an expression to the select query
     *
     * @param $fieldName        string expression field name
     * @param $expression       string expression value
     * @param $table            string table name
     * @param $fieldAliasPrefix string field alias prefix
     * @param $quoteFieldName   bool   wether or not to quote the expression field name
     * @return atkQuery The query object itself (for fluent usage)
     */
    function &addExpression($fieldName, $expression, $fieldAliasPrefix="", $quoteFieldName=false)
    {
      if ($quoteFieldName)
        $this->m_quotedfields[] = $fieldName;

      $this->m_expressions[] = array('name' => $fieldAliasPrefix.$fieldName, 'expression' => $expression);

      if (!empty($fieldAliasPrefix))
      {
        $this->m_aliasLookup["al_".$this->m_generatedAlias] = $fieldAliasPrefix.$fieldName;
        $this->m_fieldaliases[$fieldAliasPrefix.$fieldName] = "al_".$this->m_generatedAlias;
        $this->m_generatedAlias++;
      }

      return $this;
    }

    /**
    * Add table to Tables array
    * @param $name Table name
    * @param $alias Alias of table
    * @return atkQuery The query object itself (for fluent usage)
    */
    function &addTable($name, $alias = "")
    {
      $this->m_tables[] = $name;
      $this->m_aliases[count($this->m_tables)-1] = $alias;
      return $this;
    }

    /**
    * Add join to Join Array
    * @param string $table Table name
    * @param string $alias Alias of table
    * @param string $condition Condition for the Join
    * @param bool $outer Wether to use an outer (left) join or an inner join
    * @return atkQuery The query object itself (for fluent usage)
    */
    function &addJoin($table, $alias, $condition, $outer=false)
    {
      $join = " ".($outer ? "LEFT JOIN " : "JOIN ").$this->quoteField($table)." ".$this->quoteField($alias)." ON (".$condition.") ";
      if (!in_array($join, $this->m_joins)) $this->m_joins[] = $join;
      return $this;
    }

    /**
     * Add a group-by statement
     *
     * @param String $element Group by expression
     * @return atkQuery The query object itself (for fluent usage)
     */
    function &addGroupBy($element)
    {
      $this->m_groupbys[] = $element;
      return $this;
    }

    /**
     * Add order-by statement
     *
     * @param String $element Order by expression
     * @return atkQuery The query object itself (for fluent usage)
     */
    function &addOrderBy($element)
    {
      $this->m_orderbys[] = $element;
      return $this;
    }

    /**
     * Add a query condition (conditions are where-expressions that are AND-ed)
     * @param $condition Condition
     * @return atkQuery The query object itself (for fluent usage)
     */
    function &addCondition($condition)
    {
      if ($condition!="" && !in_array($condition,$this->m_conditions)) $this->m_conditions[] = $condition;
      return $this;
    }


   /**
    * Add search condition to the query. Basically similar to addCondition, but
    * searchconditions make use of the searchmode setting to determine whether the
    * different searchconditions should be and'ed or or'ed.
    * @param $condition Condition
    * @return atkQuery The query object itself (for fluent usage)
    */
    function &addSearchCondition($condition)
    {
      if ($condition!="") $this->m_searchconditions[] = $condition;
      return $this;
    }

    /**
     * Set the 'distinct' mode for the query.
     * If set to true, a 'SELECT DISTINCT' will be performed. If set to false,
     * a regular 'SELECT' will be performed.
     * 
     * @param Bool $distinct Set to true to perform a distinct select,
     *                          false for a regular select.
     * @return atkQuery The query object itself (for fluent usage)
     */
    function &setDistinct($distinct)
    {
      $this->m_distinct = $distinct;
      return $this;
    }

    /**
     * Set a limit to the number of results.
     *
     * @param int $offset Retrieve records starting with record ...
     * @param int $limit Retrieve only this many records.
     * @return atkQuery The query object itself (for fluent usage)
     */
    function &setLimit($offset, $limit)
    {
      $this->m_offset = $offset;
      $this->m_limit = $limit;
      return $this;
    }

    /**
     * Builds the SQL Select query
     * @param $distinct distinct records?
     * @return String a SQL Select Query
     */
    function buildSelect($distinct = FALSE)
    {
      if (count($this->m_fields)<1 && count($this->m_expressions)<1) return false;
      $result = "SELECT ".($distinct||$this->m_distinct ? "DISTINCT ": "");
      for ($i=0;$i<count($this->m_fields);$i++)
      {
        $result.= $this->quoteField($this->m_fields[$i]);
        $fieldalias = (isset($this->m_fieldaliases[$this->m_fields[$i]])?$this->m_fieldaliases[$this->m_fields[$i]]:"");
        if ($fieldalias!="") $result.=" AS ".$fieldalias;
        if ($i < count($this->m_fields)-1) $result.=", ";
      }

      foreach ($this->m_expressions as $i => $entry)
      {
        if (count($this->m_fields) > 0 || $i > 0) $result .= ", ";
        $fieldName = $entry['name'];
        $expression = $entry['expression'];
        $fieldAlias = isset($this->m_fieldaliases[$fieldName]) ? $this->m_fieldaliases[$fieldName] : $this->quoteField($fieldName);
        $result .= "($expression) AS $fieldAlias";
        $first = false;
      }

      $this->_addFrom($result);

      for ($i=0;$i<count($this->m_joins);$i++)
      {
        $result.=$this->m_joins[$i];
      }

      if (count($this->m_conditions)>0)
      {
          $result.= " WHERE (".implode(") AND (",$this->m_conditions).")";
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
     * Add FROM clause to query.
     * 
     * @param String $query The query
     */
    function _addFrom(&$query)
    {
      $query.= " FROM ";
      for ($i=0;$i<count($this->m_tables);$i++)
      {
        $query.= $this->quoteField($this->m_tables[$i]);
        if ($this->m_aliases[$i]!="") $query.=" ".$this->m_aliases[$i];
        if ($i < count($this->m_tables)-1) $query.=", ";
      }
      $query.= " ";
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
     * 
     * @param string $query The query to add the limiter to
     */
    function _addLimiter(&$query)
    {
      // not supported..
    }

    /**
     * Add the ORDER BY clause
     *
     * @param String $query The query
     */
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
     * @param bool $distinct distinct rows?
     *
     * @return String a SQL Select COUNT(*) Query
     */
     function buildCount($distinct = FALSE)
     {
       if (($distinct||$this->m_distinct) && count($this->m_fields) > 0)
       {
        $result = "SELECT COUNT(DISTINCT ";
        $result.=implode($this->quoteFields($this->m_fields),", ");
        $result.=") as count FROM ";
       }
       else $result = "SELECT COUNT(*) as count FROM ";

       for ($i=0;$i<count($this->m_tables);$i++)
       {
         $result.= $this->quoteField($this->m_tables[$i]);
         if ($this->m_aliases[$i]!="") $result.=" ".$this->m_aliases[$i];
         if ($i < count($this->m_tables)-1) $result.=", ";
       }

       for ($i=0;$i<count($this->m_joins);$i++)
       {
         $result.=$this->m_joins[$i];
       }

      if (count($this->m_conditions)>0)
      {
        $result.= " WHERE (".implode(") AND (",$this->m_conditions).")";
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
     * Generate a searchcondition that checks if the field is null.
     * 
     * @param String $field
     * @param Bool $emptyStringIsNull
     */
    function nullCondition($field, $emptyStringIsNull=false)
    {
      $result = "$field IS NULL";
      if ($emptyStringIsNull)
        $result = "($result OR $field = '')";
      return $result;
    }

    /**
     * Generate a searchcondition that checks if the field is not null.
     * 
     * @param String $field
     * @param Bool $emptyStringIsNull
     */
    function notNullCondition($field, $emptyStringIsNull=false)
    {
      $result = "$field IS NOT NULL";
      if ($emptyStringIsNull)
        $result = "($result AND $field <> '')";
      return $result;
    }

    /**
     * Generate a searchcondition that checks whether $value matches $field exactly.
     * @param string $field full qualified table column
     * @param mixed $value string/number/decimal expected column value
     * @param string $dbFieldType help determine exact search method
     * @return string piece of where clause to use in your SQL statement
     */
    function exactCondition($field, $value, $dbFieldType=null)
    {
      if(in_array($dbFieldType,array("decimal","number")))
        return self::exactNumberCondition($field,$value);

      if($value[0]=='!')
      {
        return "UPPER(".$field.")!=UPPER('".substr($value,1,atk_strlen($value))."')";
      }
      else
      {
        return "UPPER(".$field.")=UPPER('".$value."')";
      }
    }

    /**
     * Generate a searchcondition that check number/decimal literal values
     * @param string $field full qualified table column
     * @param mixed $value integer/float/double etc.
     * @return string piece of where clause to use in your SQL statement
     */
    public static function exactNumberCondition($field, $value)
    {
      return "$field = $value";
    }

    /**
     * Generate a searchcondition that checks whether $field contains $value .
     * 
     * @param String $field The field
     * @param String $value The value
     * @return String The substring condition
     */
    function substringCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return "UPPER(".$field.") NOT LIKE UPPER('%".substr($value,1,atk_strlen($value))."%')";
      }
      else
      {
        return "UPPER(".$field.") LIKE UPPER('%".$value."%')";
      }
    }

    /**
     * Generate a searchcondition that accepts '*' as wildcard character.
     * 
     * @param String $field
     * @param String $value
     */
    function wildcardCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return "UPPER(".$field.") NOT LIKE UPPER('".str_replace("*","%",substr($value,1,atk_strlen($value)))."')";
      }
      else
      {
        return "UPPER(".$field.") LIKE UPPER('".str_replace("*","%",$value)."')";
      }
    }

    /**
     * Generate searchcondition with greater than
     * 
     * @param String $field The database field
     * @param String $value The value
     */
    function greaterthanCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return $field." < '".substr($value,1,atk_strlen($value))."'";
      }
      else
      {
        return $field." > '".$value."'";
      }
    }

    /**
     * Generate searchcondition with greater than
     * 
     * @param String $field The database field
     * @param String $value The value
     */
    function greaterthanequalCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return $field." < '".substr($value,1,atk_strlen($value))."'";
      }
      else
      {
        return $field." >= '".$value."'";
      }
    }

    /**
     * Generate searchcondition with less than
     * 
     * @param String $field The database field
     * @param String $value The value
     */
    function lessthanCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return $field." > '".substr($value,1,atk_strlen($value))."'";
      }
      else
      {
        return $field." < '".$value."'";
      }
    }

    /**
     * Generate searchcondition with less than
     * 
     * @param String $field The database field
     * @param String $value The value
     */
    function lessthanequalCondition($field, $value)
    {
      if($value[0]=='!')
      {
        return $field." > '".substr($value,1,atk_strlen($value))."'";
      }
      else
      {
        return $field." <= '".$value."'";
      }
    }

    /**
     * Get the between condition
     *
     * @param String $field The database field
     * @param Mixed $value1 The first value
     * @param Mixed $value2 The second value
     * @param Bool $quote Add quotes?
     * @return unknown
     */
    function betweenCondition($field, $value1, $value2, $quote=true)
    {
      if($quote)
        return $field." BETWEEN '".$value1."' AND '".$value2."'";
      else
        return $field." BETWEEN ".$value1." AND ".$value2;
    }

    /**
     * Static factory method. This method returns a new instance of a query
     * object for the current database.
     * @param $basepath The basepath for the database object (defaults to 'atk.db')
     * @return atkQuery A Query object for the appropriate database
     */
    function &create($basepath="atk.db.")
    {
      $dbconfig = atkconfig("db");
      $name = "atk" . $dbconfig["default"]["driver"] . "query";
      return atknew(($basepath?$basepath.'.':'').$name);
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
      $quotefield=false;
      if ((in_array($field,$this->m_quotedfields) ||
           in_array($field, $this->m_tables)) &&
           preg_match('/(^[\w\.]+)/',$field)."'")
      {
        $quotefield=true;
      }

      $exploded = explode('.',$field);
      foreach ($exploded as $identifier)
      {
        if ($quotefield || in_array($identifier,$this->m_reservedNames))
          $identifiers[] = $this->m_fieldquote.$identifier.$this->m_fieldquote;
        else
          $identifiers[] = $identifier;
      }
      $field = implode('.',$identifiers);
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
