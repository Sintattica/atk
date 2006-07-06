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

  /** @internal include base class */
  useattrib("atkmlattribute");

  /**
   * The atkMlSelectorAttribute class represents more the less a dummy
   * attribute, which makes it possible to select languages in a multilanguage form.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkMlSelectorAttribute extends atkMlAttribute
  {
    var $m_mlselectorattribute = true;

    /**
     * Returns the language selector
     * @return html select box ready for displaying
     */
    function getSelector($prefix="")
    {
      static $s_wroteStrings=false;

      $languages = $this->getLanguages();

      /* check config */
      if (!is_array($languages) || sizeof($languages) == 0)
        return atktext("multilanguage_error_config",$this->m_ownerInstance->m_module,$this->m_ownerInstance->atknodetype());
  
      /* first selected other language */
      //$formname = $prefix.'[multilanguage_current]';
      $result = '<input id="'.$prefix.'_current" type="hidden" name="'.$formname.'" value="'.($this->m_ownerInstance->m_postvars['atkeditlng']?$this->m_ownerInstance->m_postvars['atkeditlng']:$languages[1]).'">';

      /* build selection list */
      $result .= '<select id="'.$prefix.'_lgswitch" name="'.$prefix.'_lgswitch" onchange="changeLanguage(this, \''.$prefix.'\', '.(atkconfig("multilanguage_linked")?'true':'false').')">';

      /* options */
      for ($i = 1; $i < sizeof($languages); $i++)
        $result .= '<option value="'.$languages[$i].'" '.(strtolower($languages[$i])==strtolower($this->m_ownerInstance->m_postvars['atkeditlng'])?'selected':'').'>'.text('language_'.strtolower($languages[$i])).'</option>';

      /* close */
      $result .= '</select>';

      if (!$s_wroteStrings)
      {
        $script = "str_languages = new Array();\n";
        for ($i=0, $_i=count($languages); $i<$_i; $i++)
        {
           $script.= "str_languages['".$languages[$i]."'] = '".text('language_'.strtolower($languages[$i]))."';\n";
        }
        $page = &atkPage::getInstance();
        $page->register_scriptcode($script);
        $s_wroteStrings=true;
      }
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
      $this->m_mlattribute=FALSE; // just a selector and not a real mlattribute
    }

    /**
     * Returns a piece of html code which can be used to select a language
     * @param $record array with fields
     * @return piece of html code with a textarea
     */
    function edit($record="", $prefix="")
    {
      /* register javascript */
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkmultilanguage.js.php");
      $page->register_submitscript('mlPreSubmit(\''.$prefix.'\', form);');

      // new style notification script thingee
      $code = "function atkMlSwitch(oldlng, newlng)
               {";

      foreach(array_keys($this->m_ownerInstance->m_attribList) as $attrname)
      {
        $p_attrib = &$this->m_ownerInstance->getAttribute($attrname);
        if (method_exists($p_attrib, "getMlSwitchCode"))
        {
          $code.= $p_attrib->getMlSwitchCode()."\n";
        }
      }

      $code.= "}";

      $page->register_scriptcode($code);

      return $this->getSelector($prefix);
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
    
    function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
      $lngs = $this->getLanguages();
      if (count($lngs)<=1)
      {
        return parent::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);
      }
    }
  }
?>
