<?php
  /**
   * The atkUpdateStampAttribute class can be used to store 
   * the date and time of the last modification of a record.
   * To use this attribute, add a DATETIME field to your table
   * and add this attribute to your node class. No params are necessary,
   * no initial_values need to be set. The timestamps are generated automatically.
   * This attribute is automatically set to readonly, and to hide add (because we only
   * have the first timestamp AFTER a record is added).
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
   *
   */
  class atkUpdateStampAttribute extends atkAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function atkUpdateStampAttribute($name, $flags=0)
    {
      $this->atkAttribute($name,$flags|AF_READONLY|AF_HIDE_ADD); // base class constructor
    }

    function value2db($rec)
    {
      return date("Y-m-d H:i:s");
    }

    function display($rec)
    {
      $stamp = $rec[$this->fieldname()];
      return $stamp;
    }

    function needsUpdate($rec)
    {
      // no matter what, we always save a new value.
      return true;
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
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