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
   * Revision 4.1  2001/06/07 21:06:54  ivo
   * Added atkemailattribute
   *
   *
   */
  class atkEmailAttribute extends atkAttribute
  {    

    /**
     * Constructor
     *
     * <b>Example:</b>
     *        $this->addAttribute(new atkEmailAttribute("email",AF_OBLIGATORY));
     * @param $name Name of the attribute
     * @param $flags Flags for the attribute
     */
    function atkEmailAttribute($name, $flags = 0)
    {
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
    function validate($value)
    {
      // TODO, FIXME: put a regexp here that validates an emailaddress..
      if (!(strpos($value,".")>0) && (strpos($value,"@")>0))
        return text("invalid_email");
    }
    
  }
  
?>
