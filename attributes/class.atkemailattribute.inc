<?php

  /**
   * The atkEmailAttribute class represents an attribute to handle email addresses.
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * @version $Revision$
   *
   * $Id$
   *
   * $Log$
   * Revision 4.4  2001/08/27 07:38:58  ivo
   * changed validate() api: atk now passes record to the function instead of
   * just one value. Also added a mode parameter that indicates whether we
   * are in add or update mode.
   *
   * Revision 4.3  2001/07/04 10:47:51  maurice
   * Added parameter in constructor to switch DNS search on or off.
   *
   * Revision 4.2  2001/07/03 15:52:31  maurice
   * The validate function checks string and checks DNS for valid host
   *
   * Revision 4.1  2001/06/07 21:06:54  ivo
   * Added atkemailattribute
   *
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
    function atkEmailAttribute($name, $search = false, $flags = 0)
    {
      $this->$m_dnsSearch = $search;
      atkdebug("dnsSearch ".$this->$m_dnsSearch);
      $this->atkAttribute($name, $flags);

    }

    /**
     * Returns a displayable string for this value.          
     */
    function display($record)
    {  
      return '<a href="mailto:'.$record[$this->fieldName()].'">'.$record[$this->fieldName()].'</a>';
    }

    /**
     * Check if the value is a valid email address.
     * @param $value Value to be validated
     * @return "" if ok, or an errormessage if not ok.
     */
    function validate($record, $mode)
    {
      $email = $record[$this->fieldName()];
      //first check complete string
      if (!(eregi("^[_a-z0-9-]+(\.[_a-z0-9-]+)*@([0-9a-z][0-9a-z-]*[0-9a-z]\.)+[a-z]{2,3}$", $email, $check)))
      {
        return text("invalid_email");
      }
      else
      {
        atkdebug("dnsSearch ".$this->$m_dnsSearch);
        if ($this->$m_dnsSearch)
        {
          //now check if domain exists, searches DNS for MX records
          if (!( getmxrr(substr(strstr($check[0], '@'), 1), $validate_email_temp) ))
          {
            return text("invalid_email");        
          }
        }       
      } 
    }
    
  }
  
?>
