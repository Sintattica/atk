<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage filters
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */     

  /**
   * Abstract base class for atkFilters.
   * 
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @author Kees van Dieren <kees@ibuildings.nl>
   * @package atk
   * @subpackage filters
   * @abstract
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