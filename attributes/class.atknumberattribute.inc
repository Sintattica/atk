<?php
  /**
   * The atkNumberAttribute can be used for numeric values.
   *
   * @author Ivo Jansch (ivo@achievo.com)
   * @version $Revision$
   *
   * $Id$   
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
     * Validates if value is numeric
     * @param &$record Record that contains value to be validated.
     *                 Errors are saved in this record
     * @param $mode can be either "add" or "update"
     * @return $record
     */    
    function validate($record, $mode)
    {
      if (!is_numeric($record[$this->fieldName()])) 
        triggerError($record, $this->fieldName(), 'error_notnumeric');
    }
    
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array("exact","greaterthan","greaterthanequal","lessthan","lessthanequal");
    }
  
    
  }
?>