<?php
  /**
   * The atkNumberAttribute class represents an attribute of a node
   * that can have a numeric value.
   * @author Ivo Jansch (ivo@achievo.com)
   * @version 0.5
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
  }
?>