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
        /* Unfortunately PostgreSQL does not support left joins
           at the moment. This means ATK can't support outer joins
           for PostgreSQL at the moment. Fortunately PostgreSQL
           will support outer joins as of version 7.1. Until that
           time has come, ATK can better not be used in conjunction
           with PostgreSQL. Until that time has arrived we just
           use normal joins. This will break some of the relation 
           stuff, so we don't recommend using it in a production
           environment!
        */
        $join="JOIN ";
      }
      else 
      {
        $join="JOIN ";
      }
      $this->m_joins[]= " ".$join.$table." ".$alias." ON ".$condition." ";
    }
  }

?>