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
   * @version $Revision: 6323 $
   * $Id$
   */

  /**
   * Base include
   */
  atkimport("atk.db.atkquery");

  /**
   * SQL query builder for PostgreSQL.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */
  class atkPgsqlQuery extends atkQuery
  {
    /**
     * Makes a join SQL query for PostgreSQL
     * @param string $table the table name
     * @param string $alias alias for the table
     * @param string $condition join condition
     * @param bool $outer Wether to use an outer (left) join or an inner join
     * @return atkQuery The query object (for fluent usage)
     */
    function &addJoin($table, $alias, $condition, $outer)
    {
      if ($outer)
      {
        $join="LEFT JOIN ";
      }
      else
      {
        $join="JOIN ";
      }
      $this->m_joins[]= " ".$join.$table." ".$alias." ON ".$condition." ";
    }

    /**
     * Add limiting clauses to the query.
     * Default implementation: no limit supported. Derived classes should implement this.
     * 
     * @param string $query The query to add the limiter to
     */
    function _addLimiter(&$query)
    {
      if ($this->m_offset >= 0 && $this->m_limit > 0)
      {
        $query .= " LIMIT ".$this->m_limit." OFFSET ".$this->m_offset;
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
      $query = "SELECT COUNT(*) AS count FROM (".$this->buildSelect($distinct).") x";
      return $query;
    }
  }
?>