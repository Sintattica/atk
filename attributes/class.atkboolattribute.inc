<?php
  /**
   * The atkBoolAttribute class represents an attribute of a node
   * that can either be true or false.
   * @author Ivo Jansch (ivo@achievo.com)
   * @version 0.5
   *
   * $Id$
   * $Log$
   * Revision 1.1  2001/04/23 10:17:07  ivo
   * Initial revision
   *
   * Revision 1.2  2001/02/22 22:46:17  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:57:56  sandy
   * Achievo Tool Kit
   *
   */
  class atkBoolAttribute extends atkAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function atkBoolAttribute($name, $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="")
    {
      $checked = "";
      if ($record[$this->fieldName()]>0) $checked="checked";
      return '<input type="checkbox" name="'.$this->formName().'" value="1" '.$checked.'>';
    }
    

    /**
     * Returns a piece of html code that can be used in a form to search for values   
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function search($record="")
    {
      $checked = "";
      if ($record[$this->fieldName()]>0) $checked="checked";
      return '<input type="checkbox" name="search['.$this->formName().']" value="1" '.$checked.'>';
    }    

    /**
     * Returns a displayable string for this value.
     * @param $record Array with boolean field
     * @return yes or no
     */
    function display($record)
    {
	    // We display true (1) as 'Yes' and false as 'No'
	    if ($record[$this->fieldName()]>0) return text("yes");
	    return text("no");
    }
  }
?>
