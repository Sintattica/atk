<?php
  /**
   * The atkMlTextAttribute class represents an multilanguage attribute
   * of a node that is a big text field.
   *
   * Based on atkTextAttribute.
   *
   * @author Peter Verhage (peter@ibuildings.nl)
   * @see atkTextAttribute
   *
   * $Id$
   * $Log$
   * Revision 4.3  2001/08/16 07:17:49  ivo
   * Replaced htmlentities with htmlspecialchars for some attributes.
   * (people with different charactersets experience problems with
   * htmlentities)
   *
   * Revision 4.2  2001/06/01 14:59:33  ivo
   * Better metadata reading for postgresql.
   * Fixed a bug with input sizes in textarea.
   *
   * Revision 4.1  2001/04/25 08:46:08  ivo
   * Big optimisations: using references for attribute manipulation.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:09  ivo
   * Added atk4 to repository.
   *
   * Revision 1.1  2001/02/22 22:43:14  peter
   * initial release of the multilanguage attributes
   *
   */
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
