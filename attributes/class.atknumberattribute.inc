<?php
  /**
   * The atkNumberAttribute can be used for numeric values.
   *
   * TODO: If decimals are set, than input should be rounded
   *       to the correct number of decimals.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @version $Revision$
   *
   * $Id$   
   *
   */
  class atkNumberAttribute extends atkAttribute
  {
    var $m_decimals=0; // The number of decimals of the number. 
  
    /**
     * Constructor
     * @param $name  Name of the attribute
     * @param $flags Flags for this attribute
     * @param $size  The size(s) for this attribute
     * @param $decimals The number of decimals to use. 
     *               
     */
    function atkNumberAttribute($name, $flags=0, $size=0, $decimals=0)
    {
      $this->atkAttribute($name,$flags, $size); // base class constructor
      $this->m_decimals = $decimals;
    }

    /**
     * Validates if value is numeric
     * @param &$record Record that contains value to be validated.
     *                 Errors are saved in this record
     * @param $mode can be either "add" or "update"
     * @return $record
     */    
    function validate(&$record, $mode)
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
  
    function dbFieldType()
    {
      return "number";
    }
    
    function dbFieldSize()
    {
      return $this->m_maxsize.($this->m_decimals>0?".".$this->m_decimals:"");
    }    
  }
?>