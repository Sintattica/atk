<?php

  /**
   * Class to handle specific PostgreSQL commands
   *
   * @author Peter Verhage (peter@ibuildings.nl)
   * @version $Revision$
   *
   * $Id$
   *
   */
  class atkpgsqlquery extends atkquery
  {
    /**
     * Makes a join SQL query for PostgreSQL
     * @param $table the table name
     * @param $alias alias for the table
     * @param $condition join condition
     */ 
    function addJoin($table, $alias, $condition, $outer)
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
     * @param $distinct distinct rows?
     *
     * @result a SQL Select COUNT(*) Query
     */
    function buildCount($distinct = FALSE)
    {
      $query = "SELECT COUNT(*) AS count FROM (".$this->buildSelect($distinct).") x";
      return $query;
    }
  }
?>
