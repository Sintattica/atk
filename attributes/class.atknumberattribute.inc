<?php
  /**
   * The atkNumberAttribute class represents an attribute of a node
   * that can have a numeric value.
   * @author Ivo Jansch (ivo@achievo.com)
   * @version 0.5
   *
   * $Id$
   * $Log$
   * Revision 1.1  2001/04/23 10:17:09  ivo
   * Initial revision
   *
   * Revision 1.2  2001/02/22 22:46:18  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:57:58  sandy
   * Achievo Tool Kit
   *
   */
  class atkNumberAttribute extends atkAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function atkNumberAttribute($name, $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
    }

    /**
     * Checks if value is numeric
     * @param $value values that has to be checked
     * @return boolean true or false
     */
    function validate($value)
    {
	    if (!is_numeric($value)) return text('error_notnumeric');
      else return "";
    }
  }
?>