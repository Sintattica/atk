<?php
/**
 * The atkMlSelectorAttribute class represents more the less a dummy
 * attribute, which makes it possible to select languages in a multilanguage form.
 *
 * @author Peter Verhage (peter@ibuildings.nl)
 *
 * $Id$
 * $Log$
 * Revision 4.0  2001/04/23 10:20:39  ivo
 * revision tags.
 *
 * Revision 1.1.1.1  2001/04/23 10:17:09  ivo
 * Added atk4 to repository.
 *
 * Revision 1.2  2001/03/14 12:23:23  peter
 * added hide method, to reflect Ivo's changes
 *
 * Revision 1.1  2001/02/22 22:43:14  peter
 * initial release of the multilanguage attributes
 *
 */
class atkMlSelectorAttribute extends atkAttribute
{
  /**
   * Returns the language selector
   * @return html select box ready for displaying
   */
  function getSelector()
  {
    global $config_language;

    /* check config */
    if (!is_array($config_language) || sizeof($config_language) == 0)
      return text("multilanguage_error_config");

    /* first selected other language */
    $result = '<input type="hidden" name="multilanguage_current" value="'.$config_language[1].'">';

    /* build selection list */
    $result .= '<select onchange="changeLanguage(this)">';

    /* options */
    for ($i = 1; $i < sizeof($config_language); $i++)
      $result .= '<option value="'.$config_language[$i].'">'.text('language_'.$config_language[$i]).'</option>';

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
    /* base class constructor */
    $this->atkAttribute("multilanguage_select", $flags|AF_HIDE_LIST);

    /* register javascript */
    $GLOBALS['g_layout']->register_script("atk/javascript/class.atkmultilanguage.js.php");
  }

  /**
   * Returns a piece of html code which can be used to select a language
   * @param $record array with fields
   * @return piece of html code with a textarea
   */
  function edit($record="")
  {
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
  {}

  /**
   * No function, but is necessary
   */
  function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec, $level, $mode)
  {}
}
?>