<?php
  /**
   * The atkLimitedTextAttribute class represents an attribute of a node
   * that can be limited.
   * @author Ivo Jansch (ivo@achievo.com)
   * @version Beta
   *
   * $Id$ 
   * $Log$
   * Revision 1.1  2001/04/23 10:17:08  ivo
   * Initial revision
   *
   * Revision 1.2  2001/02/22 22:46:17  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:57:57  sandy
   * Achievo Tool Kit
   *
   */
  class atkLimitedTextAttribute extends atkTextAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function atkLimitedTextAttribute($name, $flags=0)
    {
      $this->atkTextAttribute($name, $flags); // base class constructor
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="")
    {
      $result.= '<textarea name="'.$this->formName().'" rows="10" wrap="virtual" '.
                ($this->m_size > 0 ? ' cols="'.$this->m_size.'"' : '').
                '>'.htmlentities($record[$this->fieldName()]).'</textarea>';
      return $result;
    }
  }
?>
