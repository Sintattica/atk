<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage handlers
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Class that handles userinterface internationalization.
   *
   * This class is used to retrieve the proper translations for any string
   * displayed in the userinterface. It includes only those language files
   * that are actually used, and has several fallback systems to find
   * translations if they can be find in the correct module.
   *
   * @author Boy Baukema <boy@ibuildings.nl>
   * @package atk
   *
   */
  class atkLanguage
  {
    /**
     * Directory where language files are stored.
     * @access private
     * @var String
     */
    var $LANGDIR = "languages/";

    /**
     * Contains all currently loaded language strings.
     * @access private
     * @var array
     */
    var $m_cachedlang = array();

    /**
     * List of currently loaded language files
     * @access private
     * @var array
     */
    var $m_cachedlangfiles = array();

    /**
     * List of fallback modules
     * @access private
     * @var array
     */
    var $m_fallbackmodules = array();

    /**
     * List of override modules
     * @access private
     * @var array
     */
    var $m_overridemodules = array("langoverrides");

    /**
     * Default Constructor
     * @access private
     */
    function atkLanguage()
    {
      atkdebug("New instance made of atkLanguage");
    }

    /**
     * Gets an instance of the atkLanguage class
     *
     * Using this function will ensure that only 1 instance ever exists
     * (singleton).
     *
     * @return atkLanguage Instance of the atkLanguage class
     */
    function &getInstance()
    {
      static $s_atklanguage;
      if (!is_object($s_atklanguage))
      {
        $s_atklanguage = new atkLanguage();
      }
      return $s_atklanguage;
    }

    /**
     * Add a module that serves as an override for language strings.
     *
     * @param String $module Name of the module to add.
     */
    function addOverrideModule($module)
    {
      array_unshift($this->m_overridemodules,$module);
    }

    /**
     * Add a module that servers as a fallback for language strings.
     *
     * @param String $module Name of the module to add.
     */
    function addFallbackModule($module)
    {
      $this->m_fallbackmodules[] = $module;
    }

    /**
     * Calculate the list of fallbackmodules.
     * @access private
     */
    function _getFallbackModules($modulefallback)
    {
      static $s_fallbackmodules = array();
      $key = $modulefallback ? 1 : 0; // we can be called with true or false, cache both results

      if (!array_key_exists($key, $s_fallbackmodules))
      {
        global $g_modules;

        $modules = array();
        if (is_array($g_modules) && ($modulefallback|| atkconfig("language_modulefallback",false)))
        {
          foreach ($g_modules as $modname => $modpath)
          {
            $modules[] = $modname;
          }
        }
        $modules[] = "atk";

        $s_fallbackmodules[$key] = array_merge($this->m_fallbackmodules, $modules);
      }

      return $s_fallbackmodules[$key];
    }

    /**
     * Text function, retrieves a translation for a certain string.
     *
     * @static
     * @param mixed $string           string or array of strings containing the name(s) of the string to return
     *                                when an array of strings is passed, the second will be the fallback if
     *                                the first one isn't found, and so forth
     * @param String $module          module in which the language file should be looked for,
     *                                defaults to core module with fallback to ATK
     * @param String $node            the node to which the string belongs
     * @param String $lng             ISO 639-1 language code, defaults to config variable
     * @param String $firstfallback   the first module to check as part of the fallback
     * @param boolean $nodefaulttext  if true, then it doesn't returns false when it can't find a translation
     * @param boolean $modulefallback Wether or not to use all the modules of the application in the fallback,
     *                                when looking for strings
     * @return String the string from the languagefile
     */
    function text($string, $module, $node="", $lng="", $firstfallback="", $nodefaulttext=false,$modulefallback=false)
    {
      if ($lng=="") $lng = atkLanguage::getLanguage();
      $atklanguage =& atkLanguage::getInstance();

      // If only one string given, process it immediatly
      if (!is_array($string))
        return $atklanguage->_getString($string, $module, $lng, $node, $nodefaulttext, $firstfallback, $modulefallback);

      // If multiple strings given, iterate through all strings and return the translation if found
      for ($i = 0, $_i = count($string)-1; $i <= $_i; $i++)
      {
        // Try to get the translation
        $translation = $atklanguage->_getString($string[$i], $module, $lng, $node, $nodefaulttext || ($i < $_i), $firstfallback, $modulefallback);

        // Return the translation if found
        if ($translation != "")
          return $translation;
      }
    }

    /**
     * Get the current language, either from url or if that's not present, from the config.
     * @static
     * @return String current language.
     */
    function getLanguage()
    {
      global $ATK_VARS;
      if (isset($ATK_VARS["atklng"])) { $lng = $ATK_VARS["atklng"]; } // we first check for an atklng variable
      else { $lng = atkconfig("language", atkconfig("lang")); }
      return strtolower($lng);
    }

    /**
     * Determine the list of modules we need to go through to check
     * language strings. Overrides have precedence, then the
     * passed module is considered, finally if no string is found
     * the fallbacks are checked.
     *
     * @access private
     * @param String $module manually passed module
     * @param String $firstfallback an additional module in which the
     *        translation will be searched first, if not found in the
     *        module itself.
     * @param Boolean $modulefallback If true, *all* modules are checked.
     * @return array List of modules to use to find the translations
     */
    function _getModules($module, $firstfallback="", $modulefallback=false)
    {
      $arr = array();
      if ($module) $arr[] = $module;
      if ($firstfallback!="") $arr[] = $firstfallback;
      $modules = array_merge($this->m_overridemodules, $arr, $this->_getFallbackModules($modulefallback));
      return $modules;
    }

    /**
     * This function takes care of the fallbacks when retrieving a string ids.
     * It is as following:
     * First we check for a string specific to both the module and the node
     * (module_node_key).
     * If that isn't found we check for a node specific string (node_key).
     * And if all that fails we look for a general string in the module.
     *
     * @access private
     * @param string $string        the name of the string to return
     * @param string $module        module in which the language file should be looked for,
     *                              defaults to core module with fallback to ATK
     * @param string $node          the node to which the string belongs
     * @param string $lng           ISO 639-1 language code, defaults to config variable
     * @param bool  &$nodefaulttext wether or not to pass a default text back
     * @return string the name with which to call the string we want from the languagefile
     */
    function _getString($key, $module, $lng, $node="", $nodefaulttext=false, $firstfallback="", $modulefallback=false)
    {
      // first find node specific string.
      $modules = $this->_getModules($module, $firstfallback, $modulefallback);

      if ($node!="")
      {
        foreach ($modules as $modname)
        {
          $text = $this->_getStringFromFile($module."_".$node."_".$key, $modname, $lng);
          if ($text!="") return $text;
        }

        foreach ($modules as $modname)
        {
          $text = $this->_getStringFromFile($node."_".$key, $modname, $lng);
          if ($text!="") return $text;
        }
      }

      // find generic module string
      foreach ($modules as $modname)
      {
        $text = $this->_getStringFromFile($key, $modname, $lng);
        if ($text!="") return $text;
      }

      if (!$nodefaulttext)
      {
        atkdebug("atkLanguage: translation for '$key' with module: '$module' and node: '$node' not found, returning default text",DEBUG_WARNING);
        // Still nothing found. return default string
        return $this->defaultText($key);
      }
      return "";
    }

    /**
     * Checks wether the language is set or not
     *
     * If set, it does nothing and return true
     * otherwise it sets it
     *
     * @access private
     * @param string $file    file to import
     * @param string $module  the module to import the language file from
     * @param string $lng     language of file to import
     * @return bool true if everything went okay
     */
    function _includeLanguage($module, $lng)
    {
      if (!isset($this->m_cachedlangfiles[$module][$lng])||$this->m_cachedlangfiles[$module][$lng] != 1)
      {
        $this->m_cachedlangfiles[$module][$lng] = 1;
        if ($module=="atk")
        {
          $path = atkconfig("atkroot")."atk/".$this->LANGDIR;
        }
        else if ($module=="langoverrides")
        {
          $path = atkconfig("language_basedir",$this->LANGDIR);
        }
        else
        {
          $path = moduleDir($module).$this->LANGDIR;
        }

        $file = $path.$lng.".lng";

        if (file_exists($file))
        {
          include($file);
          $this->m_cachedlang[$module][$lng] = $$lng;
          return true;
        }
        return false;
      }
      return true;
    }

    /**
     * A function to change the original "$something_text" string to
     * "Something text"
     * This is only used when we really can't find the "$something_text" anywhere
     * @param string $string the name of the string to return
     * @return string the changed string
     */
    function defaultText($string)
    {
      return ucfirst(str_replace("_"," ",str_replace('title_','',$string)));
    }

    /**
     * Gets the string from the languagefile or, if we failed, returns ""
     *
     * @access private
     * @param string $string        the name which was given when the text function was called
     * @param string $module        the name of the module to which the text function belongs
     * @param string $node          the name of the node to which the text function belongs
     * @param string $lng           the current language
     * @param bool  &$nodefaulttext wether or not to pass a default text back
     * @return var the true name by which the txt is called or "" if we can't find any entry
     */
    function _getStringFromFile($key, $module, $lng)
    {
      $this->_includeLanguage($module, $lng);

      if (isset($this->m_cachedlang[$module])
          && is_array($this->m_cachedlang[$module][$lng])
          && isset($this->m_cachedlang[$module][$lng][$key]))
      {
        return $this->m_cachedlang[$module][$lng][$key];
      }
      return "";
    }

  }
?>
