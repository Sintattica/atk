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
      if ($outer) $condition.=" (+)";
      $this->addtable($table,$alias);
      $this->addcondition($condition);
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