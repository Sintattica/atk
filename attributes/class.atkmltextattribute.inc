<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /** @internal Include base class */
  useattrib("atkmlattribute");
   
  /**
   * The atkMlTextAttribute class represents an multilanguage attribute
   * of a node that is a big text field.
   *
   * Based on atkTextAttribute.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */  
  class atkMlTextAttribute extends atkMlAttribute
  {

    /**
     * Constructor
     * @param string $name name of the attribute
     * @param int $flags flags for this attribute
     * @param int $size The size of the attribute
     */
    function atkMlTextAttribute($name, $flags=0, $size=0)
    {
      $this->atkMlAttribute($name, $flags, $size); // base class constructor
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param Array $record array with fields
     * @param String $prefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return piece of html code with a textarea
     */
    function edit($record="", $prefix="", $mode="")
    {
      $this->m_edited=true;
      $languages = $this->getLanguages();
      $id = $this->getHtmlId($prefix);
      $this->m_languages = $languages;
      /* register javascript */
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkmultilanguage.js.php");
      //$page->register_submitscript('submitSave(form);');

      $this->m_size = $this->maxInputSize();

      /* hidden values */
      for ($i = 1; $i < sizeof($languages); $i++)
      {
        $result .= '<input type="hidden" name="'.$prefix.$this->formName().'['.$languages[$i].']"'.
                 ' value="'.htmlspecialchars($record[$this->fieldName()][$languages[$i]]).'">';
      }

      /* default language */
      $result.= '<textarea name="'.$prefix.$this->formName().'['.$languages[0].']"'.
               ' rows="10" wrap="virtual"'.($this->m_size > 0 ? ' cols="'.$this->m_size.'"' : '').'>'.
               htmlspecialchars($record[$this->fieldName()][$languages[0]]).
               '</textarea>';

      return $result;
    }

    /**
     * Adds the attribute's edit / hide HTML code to the edit array.
     *
     * This method is called by the node if it wants the data needed to create
     * an edit form.
     *
     * @param String $mode     the edit mode ("add" or "edit")
     * @param array  $arr      pointer to the edit array
     * @param array  $defaults pointer to the default values array
     * @param array  $error    pointer to the error array
     * @param String $fieldprefix   the fieldprefix
     */
    function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
      atkAttribute::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);

      if ($this->m_edited) // if we edited this attrib, we also need to add the
      {
        $key= array_search($this->m_ownerInstance->m_postvars['atkeditlng'],$this->m_languages);
        $curlng = $this->m_languages[($key!==false?$key:1)];
        $entry = array("name" => $this->m_name."_ml", "obligatory" => $this->hasFlag(AF_OBLIGATORY), "attribute" => &$this);

        /* label? */
        $entry["label"] = $this->label($defaults).' (<label id="'.$fieldprefix.$this->formName().'_label">'.atktext("language_".strtolower($curlng)).'</label>)';
        $entry["id"] = $this->getHtmlId($fieldprefix);
        $entry["tabs"] = $this->m_tabs;     
        $entry["sections"] = $this->m_sections;

        /* first language of other languages */
        $entry["html"] = '<textarea name="'.$fieldprefix.$this->formName().'['.(count($this->m_languages)==2 ? $curlng: "multilanguage").']"'.
                   ' rows="10" wrap="virtual"'.($this->m_size > 0 ? ' cols="'.$this->m_size.'"' : '').' >'.htmlspecialchars($defaults[$this->fieldName()][$curlng]).
                   '</textarea>';

        $arr["fields"][] = $entry;
        $this->m_edited = false;
      }
    }
  }
?>