<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage handlers
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal include baseclass.
   */
  useattrib('atkDateTimeAttribute');
  
  /**
   * Attribute for keeping track of last-modification times.
   *
   * The atkUpdateStampAttribute class can be used to automatically store the
   * date and time of the last modification of a record.
   * To use this attribute, add a DATETIME field to your table and add this
   * attribute to your node. No params are necessary, no initial_values need 
   * to be set. The timestamps are generated automatically.
   * This attribute is automatically set to readonly, and to af_hide_add 
   * (because we only have the first timestamp AFTER a record is added).
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkUpdateStampAttribute extends atkDateTimeAttribute
  {
    /**
     * Var is need for save without close. In this case the record is not
     * updated therefor we have to save the value.
     * @access private
     * @var String
     */
    var $m_stamp = "";

    /**
     * Constructor
     *
     * @param String $name Name of the attribute (unique within a node, and
     *                     corresponds to the name of the datetime field 
     *                     in the database where the stamp is stored.
     * @param int $flags Flags for the attribute.
     */
    function atkUpdateStampAttribute($name, $flags=0)
    {    
      $this->atkDateTimeAttribute($name, date("Y-m-d"), date("H:i:s"), $flags|AF_READONLY|AF_HIDE_ADD);
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * In the case of atkUpdateStampAttribute, this method is used to
     * calculate the timestamp of last modification.
     * 
     * @param array $rec The record that is being stored.
     * @return String The last-modified timestamp in sql-compatible format.
     */
    function value2db($rec)
    {
      $stamp = date("Y-m-d H:i:s");
      $this->m_stamp = $stamp;
      return $stamp;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * In this case, the timestamp is returned in human readable format.
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list" 
     *                     for displaying in recordlists). The regular
     *                     atkAttribute does not use this parameter, but
     *                     derived attributes may use it to distinguish
     *                     between the two display modes.
     * @return String HTML String 
     */
    function display($rec, $mode="")
    {
      $stamp = ($this->m_stamp != "" ? $this->m_stamp : parent::value2db($rec));
      return $stamp;
    }

    /**
     * This function is called by the framework to determine if the attribute
     * needs to be saved to the database in an updateDb call.
     *
     * In this case, true is returned in all cases, since we always want to
     * store the last-modification stamp.
     *
     * @param array $record The record that is going to be saved.
     * @return boolean true
     */
    function needsUpdate($rec)
    {
      // no matter what, we always save a new value.
      return true;
    }

    /**
     * This function is called by the framework to determine if the attribute
     * needs to be saved to the database in an addDb call.
     *
     * In this case, true is returned in all cases, since we always want to
     * store the last-modification stamp.
     *
     * @param array $record The record that is going to be saved.
     * @return boolean true
     */
    function needsInsert($rec)
    {
      // no matter what, we always save a new value.
      return true;
    }


    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">)
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode with hidden form elements that post
     *                This attribute's value without showing it.
     */
    function hide($record="", $fieldprefix)
    {
      $field = $record[$this->fieldName()];

      if (is_array($field))
        foreach ($field as $key => $value)
          $result .= '<input type="hidden" name="'.$fieldprefix.$this->formName().'['.$key.']" '.'value="'.$value.'">';

      else
        $result = '<input type="hidden" name="'.$fieldprefix.$this->formName().'" value="'.$field.'">';

      return $result;
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute. 
     *
     * @return array Array containing "exact" as the only supported 
     *               searchmode.
     */
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array("exact");
    }

  }

?>