<?php
/**
 * The atkMlSelectorAttribute class represents more the less a dummy
 * attribute, which makes it possible to select languages in a multilanguage form.
 *
 * @author Peter Verhage (peter@ibuildings.nl)
 * @version $Revision$
 *
 * $Id$
 *
 */
 
useattrib("atkmlattribute");

class atkMlSelectorAttribute extends atkMlAttribute
{
  /**
   * Returns the language selector
   * @return html select box ready for displaying
   */
  function getSelector()
  {
    $languages = $this->getLanguages();

    /* check config */
    if (!is_array($languages) || sizeof($languages) == 0)
      return text("multilanguage_error_config");

    /* first selected other language */
    $result = '<input type="hidden" name="multilanguage_current" value="'.$languages[1].'">';

    /* build selection list */
    $result .= '<select onchange="changeLanguage(this)">';

    /* options */
    for ($i = 1; $i < sizeof($languages); $i++)
      $result .= '<option value="'.$languages[$i].'">'.text('language_'.$languages[$i]).'</option>';

    /* close */
    $result .= '</select>';
    return $result;
  }

  /**
   * Constructor
   * @param $name Name of the attribute
   * @param $flags Flags for this attribute
   */
  function atkMlSelectorAttribute($name="", $flags=0)
  {
    global $config_atkroot;
    /* base class constructor */
    $this->atkMlAttribute("multilanguage_select", $flags|AF_HIDE_LIST);    
  }

  /**
   * Returns a piece of html code which can be used to select a language
   * @param $record array with fields
   * @return piece of html code with a textarea
   */
  function edit($record="")
  {
    /* register javascript */    
    $page = &atkPage::getInstance();
    $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkmultilanguage.js.php");
    return $this->getSelector();
  }

  /**
   * Search language selector
   * @param $record array with fields
   * @return search field
   */
  function search($record="")
  {
    return $this->getSelector();
  }

  /**
   * Returns a piece of html code that can be used in a form to display
   * hidden values for this attribute.
   * @param $record Array with values
   * @return Piece of htmlcode
   */
  function hide($record="")
  {
    return "";
  }
  
  /**
   * No function, but is necessary
   */
  function store($a, $b)
  {
    return true;
  }

  /**
   * No function, but is necessary
   */
  function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec, $level, $mode)
  {}
  
  /**
   * Dummy imp 
   */
  function dbFieldType()
  {
    return "";
  }
}
?>