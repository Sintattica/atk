<?php

/**
 * Oracle 8 specific SQL functions.
 * 
 * @author Sandy Pleyte (sandy@ibuildings.net)
 * @version $Revision$
 *
 * $Id$
 *
 */

  class atkoci8query extends atkquery
  {

    /**
    * Makes a Join SQL query for Oracle 
    * 
    * @param $table Table name
    * @param $alias Alias for the table
    * @param $condition Condition for the join
    * @since ATK 0.5.2 
    */ 
    
    function addJoin($table, $alias, $condition, $outer)
    {
      // $this->m_joins[]= ", ".$table." ".$alias." WHERE ".$condition." ";
      if ($outer) $condition.=" (+)";
      $this->addtable($table,$alias);
      $this->addcondition($condition);
    }

  }

?>