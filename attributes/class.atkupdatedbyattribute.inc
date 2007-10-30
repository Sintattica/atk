<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2006 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal baseclass include
   */
  userelation("atkmanytoonerelation");

  /**
   * This attribute can be used to automatically store the user that inserted
   * or last modified a record.
   *
   * Note that this attribute relies on the config value $config_auth_usernode.
   * If you use this attribute, be sure to set it in your config.inc.php file.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkUpdatedByAttribute extends atkManyToOneRelation
  {
    /**
     * Constructor.
     *
     * @param String $name Name of the field
     * @param int $flags Flags for this attribute.
     * @return atkUpdatedByAttribute
     */
    function atkUpdatedByAttribute($name, $flags=0)
    {
      $this->atkManyToOneRelation($name, atkconfig("auth_usernode"), $flags|AF_READONLY|AF_HIDE_ADD);
    }

    /**
     * needsUpdate always returns true for this attribute.
     * @return true
     */
    function needsUpdate() { return true; }

    /**
     * needsInsert always returns true for this attribute.
     * @return true
     */
    function needsInsert() { return true; }

    /**
     * This method is overriden to make sure that upon each save, the
     * current user is retrieved.
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
    {
      if ($mode=="update"||$mode=="add")
      {
        $rec[$this->fieldName()] = getUser();
      }
      return parent::addToQuery($query, $tablename, $fieldaliasprefix, $rec, $level,  $mode);
    }

    /**
     * This method is overriden to make sure that when a form is posted ('save' button), the
     * current record is refreshed so the output on screen is accurate.
     */    
    function initialValue()
    {
      return getUser();
    }
    
    function value2db($record)
    {
      // if record not created using a form this situation can occur, so set the value here
      if (!is_array($record[$this->fieldName()]))
        $record[$this->fieldName()] = $this->initialValue();
      return parent::value2db($record);
    }

  }

?>