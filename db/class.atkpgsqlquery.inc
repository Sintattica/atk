<?php

  /**
   * Class to handle specific PostgreSQL commands
   * @author Peter Verhage (peter@ibuildings.nl)
   * @version 0.1
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
  }

?>