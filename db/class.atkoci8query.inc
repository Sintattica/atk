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
   * @version $Revision: 6311 $
   * $Id$
   */

  /**
   * Base include
   */
  atkimport("atk.db.atkquery");

 /**
   * Query build for Oracle 8i databases.
   *
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */
  class atkOci8Query extends atkQuery
  {

    /**
    * Makes a Join SQL query for Oracle
    *
    * @param string $table Table name
    * @param string $alias Alias for the table
    * @param string $condition Condition for the join
    * @param bool $outer Wether to use an outer (left) join or an inner join
    * @return atkQuery The query object (for fluent usage).
    */
    function &addJoin($table, $alias, $condition, $outer)
    {
      if ($outer) $condition.=" (+)";
      $this->addtable($table,$alias);
      $this->addcondition($condition);
      return $this;
    }

    /**
     * Add limiting clauses to the query.
     * Default implementation: no limit supported. Derived classes should implement this.
     *
     * @param String $query The query
     */
    function _addLimiter(&$query)
    {
      /* limit? */
      if ($this->m_offset >= 0 && $this->m_limit > 0)
      {
        /* row id's start at 1! */
	      $query="SELECT * FROM (SELECT rownum AS rid, XX.* FROM (".
            $query.") XX) YY  WHERE YY.rid >= ".($this->m_offset+1)." AND YY.rid <= ".($this->m_offset + $this->m_limit);
      }
    }
    

   /**
    * Generate a searchcondition that checks whether $field contains $value .
    * 
    * This override adds special support for comparisons using a subquery instead
    * of a table field. Oracle doesn't allow direct UPPER(...) calls on the subquery
    * result so we need to wrap it inside a select query from dual. To prevent 
    * perform loss we try to detect if the comparison field is a subquery or not.  
    * 
    * @param String $field The field
    * @param String $value The value
    * @return String The substring condition
    */
   function substringCondition($field, $value)
   {
     if (substr(ltrim(strtoupper($field), ' ('), 0, 7) != "SELECT ")
     {
       return parent::substringCondition($field, $value);
     }
     
     if($value[0]=='!')
     {
       return "(SELECT UPPER((".$field.")) FROM dual) NOT LIKE '%".strtoupper(substr($value,1,atk_strlen($value)))."%'";
     }
     else
     {
       return "(SELECT UPPER((".$field.")) FROM dual) LIKE '%".strtoupper($value)."%'";
     }
   }     
 }
?>