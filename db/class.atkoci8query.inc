<?php

/**
* $Id$
* $Log$
* Revision 4.0  2001/04/23 10:20:39  ivo
* revision tags.
*
* Revision 1.1.1.1  2001/04/23 10:17:11  ivo
* Added atk4 to repository.
*
* Revision 1.1.1.1  2001/01/10 13:57:58  sandy
* Achievo Tool Kit
*
*/


// Multiple Inclusion Protection
if (!defined("CLASS_ATKOCI8QUERY_INC"))
{
define("CLASS_ATKOCI8QUERY_INC",1);
// Multiple Inclusion Protection


  /**
   *  Class to handle specific Oracle command's
   *  @author Sandy Pleyte (sandy@ibuildings.net)
   *  @version 0.2
   *
   * <b>Changes</b>:
   *
   * 0.2    - Outer join feature (adding (+) to joins)  (Ivo)
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




} // endif CLASS_ATKOCI8QUERY_INC;

?>
