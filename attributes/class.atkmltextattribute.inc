<?php
  /**
   * The atkMlTextAttribute class represents an multilanguage attribute
   * of a node that is a big text field.
   *
   * Based on atkTextAttribute.
   *
   * @author Peter Verhage (peter@ibuildings.nl)
   * @see atkTextAttribute
   * @version $Revision$
   *
   * $Id$
   *
   */
 
  useattrib("atkmlattribute");
   
  class atkMlTextAttribute extends atkMlAttribute
  {
    /**
     * Constructor
     * @param $name name of the attribute
     * @param $flags flags for this attribute
     */
    function atkMlTextAttribute($name, $flags=0)
    {
      $this->atkMlAttribute($name, $flags); // base class constructor
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record array with fields
     * @return piece of html code with a textarea
     */
    function edit($record="")
    {
      global $config_language, $g_layout;

      $this->m_size = $g_layout->maxInputSize();      

      /* hidden values */
      for ($i = 1; $i < sizeof($config_language); $i++)
      {
        $result .= '<input type="hidden" name="'.$this->formName().'['.$config_language[$i].']"'.
                 ' value="'.$record[$this->fieldName()][$config_language[$i]].'">';
      }

      /* default language */
      $result .= text('language_'.$config_language[0]).':<br>'.
               '<textarea name="'.$this->formName().'['.$config_language[0].']"'.
               ' rows="10" wrap="virtual"'.($this->m_size > 0 ? ' cols="'.$this->m_size.'"' : '').'>'.
               htmlspecialchars($record[$this->fieldName()][$config_language[0]]).
               '</textarea><br><br>';

      /* first language of other languages */
      $result .= text('multilanguage_choice').':<br>'.
               '<textarea name="'.$this->formName().'[multilanguage]"'.
               ' rows="10" wrap="virtual"'.($this->m_size > 0 ? ' cols="'.$this->m_size.'"' : '').'>'.
               htmlspecialchars($record[$this->fieldName()][$config_language[1]]).
               '</textarea><br><br>';

      return $result;
    }
  }
?>
