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
    // number of rows of the edit box
    var $m_rows = 10;
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function atkTextAttribute($name,$rows=0,$flags=0)
 { 
      
      // compatiblity with old versions
      if (func_num_args()==3) $this->m_rows=$rows;
      else                   
      {
        $flags = $rows;
      }
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
      if ($this->m_size != 0) $cols = $this->m_size;
      else $cols = $g_layout->maxInputSize();
      $rows = $this->m_rows;
      
      if ($rows =="" || $rows == 0) $rows = 10;
	    // watch out, $maxsize isn't supported for a textarea.
      $result= '<textarea name="'.$fieldprefix.$this->formName().'" rows="'.$rows.'" cols="'.$cols.'" wrap="virtual"'.               
               '>'.htmlspecialchars($record[$this->fieldName()]).'</textarea>';
      return $result;
    }
  }
?>
