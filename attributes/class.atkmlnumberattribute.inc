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
   * Revision 4.3  2001/09/26 12:05:52  maurice
   * Code optimalisations for error handling
   *
   * Revision 4.2  2001/09/26 07:05:13  maurice
   * New error handling. Errors are saved in $record variable
   *
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
     * Validates if value is numeric
     * @param &$record Record that contains value to be validated.
     *                 Errors are saved in this record
     * @param $mode can be either "add" or "update"
     * @return $record
     */
    function validate($record, $mode)
    {     
      global $config_language;
      
      $value = $record[$this->fieldName()];

      reset($config_language);
      while (list($i, $language) = each($config_language))
        if (!is_numeric($value[$language]))
          triggerError($record, $this->fieldName(), 'error_notnumeric');
    }
  }
?>
