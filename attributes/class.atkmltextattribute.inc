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
      $languages = $this->getLanguages();
      /* register javascript */
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkmultilanguage.js.php");
      $page->register_submitscript('submitSave(form);');

      $this->m_size = $this->maxInputSize();      

      /* hidden values */
      for ($i = 1; $i < sizeof($languages); $i++)
      {
        $result .= '<input type="hidden" name="'.$this->formName().'['.$languages[$i].']"'.
                 ' value="'.$record[$this->fieldName()][$languages[$i]].'">';
      }

      /* default language */
      $result .= text('language_'.$languages[0]).':<br>'.
               '<textarea name="'.$this->formName().'['.$languages[0].']"'.
               ' rows="10" wrap="virtual"'.($this->m_size > 0 ? ' cols="'.$this->m_size.'"' : '').'>'.
               htmlspecialchars($record[$this->fieldName()][$languages[0]]).
               '</textarea><br><br>';

      /* first language of other languages */
      $result .= text((count($languages)==2 ? $languages[1] : "multilanguage_choice")).':<br>'.
               '<textarea name="'.$this->formName().'['.(count($languages)==2 ? $languages[1] : "multilanguage").']"'.
               ' rows="10" wrap="virtual"'.($this->m_size > 0 ? ' cols="'.$this->m_size.'"' : '').'>'.
               htmlspecialchars($record[$this->fieldName()][$languages[1]]).
               '</textarea><br><br>';

      return $result;
    }
  }
?>