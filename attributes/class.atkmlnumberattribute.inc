<?php
  /**
   * The atkMlNumberAttribute class represents an multilanguage
   * attribute of a node that can have a numeric value.
   *
   * Based on atkNumberAttribute.
   *
   * @author Peter Verhage (peter@ibuildings.nl)
   * @see atkNumberAttribute
   *
   * $Id$
   * $Log$
   * Revision 1.1  2001/04/23 10:17:09  ivo
   * Initial revision
   *
   * Revision 1.1  2001/02/22 22:43:14  peter
   * initial release of the multilanguage attributes
   *
   */
  class atkMlNumberAttribute extends atkMlAttribute
  {
    /**
     * Constructor
     * @param $name name of the attribute
     * @param $flags flags for this attribute
     */
    function atkMlNumberAttribute($name, $flags=0)
    {
	    $this->atkMlAttribute($name, $flags); // base class constructor
    }

    /**
     * Checks if value is numeric
     * @param $value values that has to be checked
     * @return boolean true or false
     */
    function validate($value)
    {
      global $config_language;

      reset($config_language);
      while (list($i, $language) = each($config_language))
        if (!is_numeric($value[$language])) return text('error_notnumeric');;

	    return "";
    }
  }
?>
