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
   * @version $Revision: 6309 $
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
      $this->setForceInsert(true);
      $this->setForceUpdate(true);
    }
    
    /**
     * Adds this attribute to database queries.
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
    {
      if ($mode == 'add' || $mode == 'update')
      {
        atkAttribute::addToQuery($query, $tablename, $fieldaliasprefix, $rec, $level, $mode);
      }
      else
      {
        parent::addToQuery($query, $tablename, $fieldaliasprefix, $rec, $level, $mode);
      }
    }
    
    /**
     * This method is overriden to make sure that when a form is posted ('save' button), the
     * current record is refreshed so the output on screen is accurate.
     * 
     * @return array Array with userinfo, or "" if no user is logged in.
     */    
    function initialValue()
    {
      $fakeRecord = array($this->fieldName() => getUser());
      $this->populate($fakeRecord);
      return $fakeRecord[$this->fieldName()];
    }
    
    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * @param array $record The record that holds this attribute's value.
     * @return String The database compatible value
     */
    function value2db($record)
    {
      $record[$this->fieldName()] = $this->initialValue();
      return parent::value2db($record);
    }
  }
?>