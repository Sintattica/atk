<?php
  /**
   * The atkMlNumberAttribute class represents an multilanguage
   * attribute of a node that can have a numeric value.
   *
   * Based on atkNumberAttribute.
   *
   * @author Peter Verhage (peter@ibuildings.nl)
   * @see atkNumberAttribute
   * @version $Revision$
   *
   * $Id$   
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
