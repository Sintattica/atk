<?php

  /**
   * The atkHtmlAttribute class is the same as a normal
   * atkAttribute. It only has a different display
   * function. For this attribute, the value is
   * rendered as-is, which means you can use html
   * codes in the text.
   *
   * <b>Remarks:</b>
   *   There might me times where you want the user to be able to
   *   use html tags, but you don't want to give him the
   *   inconvenience of using br's for each line. For this
   *   reason the constructor accepts a parameter which tells
   *   it to do a newline-to-br conversion.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *   
   * $Id$   
   *
   */
  class atkHtmlAttribute extends atkTextAttribute
  {
    /**
     * New line to BR boolean
     */
    var $nl2br = false;

    /** 
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags of the attribute
     * @param $nl2br nl2br boolean
     */    
    function atkHtmlAttribute($name, $flags=0, $nl2br = false)
    {
      $this->atkAttribute($name,$flags); // base class constructor
      $this->nl2br = $nl2br;
    }

    /**
     * Returns a displayable string for this value.
     * @param $record Array wit fields
     * @return Formatted string
     */
    function display($record)
    {
      if ($this->nl2br) return nl2br($record[$this->fieldName()]);
      else return $record[$this->fieldName()];
    }
  }
?>
