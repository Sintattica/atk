<?php
  /**
   * The atkMlHtmlAttribute class is the same as a normal
   * atkMlTextAttribute. It only has a different display
   * function. For this attribute, the value is rendered as-is,
   * which means you can use html codes in the text.
   *
   * Based on atkHtmlAttribute.
   *
   * @author Peter Verhage (peter@ibuildings.nl)
   * @version $Revision$
   * @see atkHtmlAttribute
   *
   * $Id$
   *
   */
  class atkMlHtmlAttribute extends atkMlTextAttribute
  {
    /**
     * New line to BR boolean
     */
     var $nl2br = false;

    /**
     * Constructor
     * @param $name name of the attribute
     * @param $flags flags of the attribute
     * @param $nl2br nl2br boolean
     */
    function atkMlHtmlAttribute($name, $flags=0, $nl2br = false)
    {
      $this->atkMlTextAttribute($name, $flags); // base class constructor
      $this->nl2br = $nl2br;
    }

    /**
     * Returns a displayable string for this value.
     * @param $record Array wit fields
     * @return Formatted string
     */
    function display($record)
    {
      global $config_language;
      if ($this->nl2br) return nl2br($record[$this->fieldName()][$config_language[0]]);
      else return $record[$this->fieldName()][$config_language[0]];
    }
  }
?>
