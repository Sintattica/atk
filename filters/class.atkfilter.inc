<?php

 /**
  * filter base class
  * 
  * @authors Ivo Jansch      <ivo@ibuildings.nl>
             Kees van Dieren <kees@ibuildings.nl>
  * @version $Revision$
  * $Id$
  * @copyright Ibuildings.nl BV
  * @package atk
  * @subpackage filters
  */
  class atkFilter extends atkAttribute
  {	
    function atkFilter($name, $flags=0)
    {	
      $this->atkAttribute($name, $flags|AF_HIDE|AF_FORCE_LOAD);
    }

    /**
     * Add's fields to query
     * @param $query The SQL query
     * @param $tablename Tablename
     * @param $fieldaliasprefix Field alias prefix
     * @param $level ?
     * @param $mode Indicates what kind of query is being processing:
     *              This can be any action performed on a node (edit, add, etc)
     *              Mind you that "add" and "update" are the actions that store
     *              something in the database, whereas the rest are probably
     *              select queries.
     * @return New SQL Query
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
    }
  }
?>