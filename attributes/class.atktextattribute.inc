<?php
  /**
   * The atkTextAttribute class represents an attribute of a node
   * that is a big text field.
   *
   * @author Ivo Jansch (ivo@achievo.com)
   * @version $Revision$
   *   
   * $Id$
   *   
   */
  class atkTextAttribute extends atkAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function atkTextAttribute($name, $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a textarea
     */
    function edit($record="", $fieldprefix="")
    {
      global $g_layout;
 	    // todo: configurable rows
	    // watch out, $maxsize isn't supported for a textarea.
      $result= '<textarea name="'.$fieldprefix.$this->formName().'" rows="10" cols="'.$g_layout->maxInputSize().'" wrap="virtual"'.               
               '>'.htmlspecialchars($record[$this->fieldName()]).'</textarea>';
      return $result;
    }
  }
?>