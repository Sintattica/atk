<?php

  /**
   * The atkEmailAttribute class represents an attribute to handle email addresses.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @version $Revision$
   *
   * $Id$
   *   
   */
  class atkEmailAttribute extends atkAttribute
  {    

    /**
     * Bool to set DNS search in validate function
     */

    var $m_dnsSearch = false;

    /**
     * Constructor
     *
     * <b>Example:</b>
     *        $this->addAttribute(new atkEmailAttribute("email",AF_OBLIGATORY));
     * @param $name Name of the attribute
     * @param $search Search DNS for MX records in validate function
     * @param $flags Flags for the attribute
     */
    function atkEmailAttribute($name, $search = false, $flags = 0, $size=0)
    {
      $this->$m_dnsSearch = false;      
      $this->atkAttribute($name, $flags, $size);
    }

    /**
     * Returns a displayable string for this value.          
     */
    function display($record)
    {  
      return '<a href="mailto:'.$record[$this->fieldName()].'">'.$record[$this->fieldName()].'</a>';
    }

    /**
     * Validates  email adress through regular expression and dns check
     * @param &$record Record that contains value to be validated.
     *                 Errors are saved in this record
     * @param $mode can be either "add" or "update"
     * @return $record
     */
    function validate(&$record, $mode)
    {
      $email = $record[$this->fieldName()];
      //first check complete string
      if (!(eregi("^[_a-z0-9-]+(\.[_a-z0-9-]+)*@([0-9a-z][0-9a-z-]*[0-9a-z]\.)+[a-z]{2,4}$", $email, $check)))
      {
        triggerError($record, $this->fieldName(), 'error_invalid_email');
      }
      else
      {
        if ($this->$m_dnsSearch)
        {
          //now check if domain exists, searches DNS for MX records
          $domain = substr(strstr($check[0], '@'), 1);
          if (!( checkdnsrr( $domain, "ANY") ))
          {
            triggerError($record, $this->fieldName(), 'error_unkown_domain', text('error_unkown_domain')." ".$domain);
          }
        }       
      } 
    }
    
    function dbFieldType()
    {
      return "string";
    }
    
  }
  
?>
