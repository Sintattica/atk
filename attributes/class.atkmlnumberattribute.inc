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
   * Revision 4.1  2001/08/27 07:38:58  ivo
   * changed validate() api: atk now passes record to the function instead of
   * just one value. Also added a mode parameter that indicates whether we
   * are in add or update mode.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:09  ivo
   * Added atk4 to repository.
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
    function validate($record, $mode)
    {     
      global $config_language;
      
      $value = $record[$this->fieldName()];

      reset($config_language);
      while (list($i, $language) = each($config_language))
        if (!is_numeric($value[$language])) return text('error_notnumeric');;

	    return "";
    }
  }
?>
