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
   * @version $Revision$
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
    * @param $table Table name
    * @param $alias Alias for the table
    * @param $condition Condition for the join
    * @return atkQuery The query object (for fluent usage).
    */
    function &addJoin($table, $alias, $condition, $outer)
    {
      if ($outer) $condition.=" (+)";
      $this->addtable($table,$alias);
      $this->addcondition($condition);
      return $this;
    }

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
  }

?>
