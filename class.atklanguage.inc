<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
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
     * Instance.
     * 
     * @var atkLanguage
     */
    private static $s_instance = null;
    
    /**
     * Supported languages.
     * 
     * @var array
     */
    private static $s_supportedLanguages = null;
    
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
     * List of custum language string overrides
     * @access private
     * @var array
     */
    var $m_customStrings = array();

    /**
     * Default Constructor
     * @access private
     */
    public function __construct()
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
    public static function getInstance()
    {
      if (self::$s_instance == null)
      {
        self::$s_instance = new self();
      }
      
      return self::$s_instance;
    }

    /**
     * Add a module that serves as an override for language strings.
     *
     * @param String $module Name of the module to add.
     */
    public function addOverrideModule($module)
    {
      array_unshift($this->m_overridemodules,$module);
    }

    /**
     * Add a module that servers as a fallback for language strings.
     *
     * @param String $module Name of the module to add.
     */
    public function addFallbackModule($module)
    {
      $this->m_fallbackmodules[] = $module;
    }

    /**
     * Calculate the list of fallbackmodules.
     * 
     * @access protected
     * @param bool $modulefallback  Wether or not to use all the modules of the application in the fallback,
     *                              when looking for strings
     * @return array Array of fallback modules
     */
    protected function _getFallbackModules($modulefallback)
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
     * @param bool   $nodefaulttext   if true, then it doesn't returns false when it can't find a translation
     * @param bool   $modulefallback  Wether or not to use all the modules of the application in the fallback,
     *                                when looking for strings
     * @return String the string from the languagefile
     */
    public static function text($string, $module, $node="", $lng="", $firstfallback="", $nodefaulttext=false,$modulefallback=false)
    {
      // We don't translate nothing
      if ($string=='') return '';
      if ($lng=="") $lng = atkLanguage::getLanguage();
      $lng = strtolower($lng);
      $atklanguage = atkLanguage::getInstance();

      // If only one string given, process it immediatly
      if (!is_array($string))
        return $atklanguage->_getString($string, $module, $lng, $node, $nodefaulttext, $firstfallback, $modulefallback);

      // If multiple strings given, iterate through all strings and return the translation if found
      for ($i = 0, $_i = count($string); $i < $_i; $i++)
      {
        // Try to get the translation
        $translation = $atklanguage->_getString($string[$i], $module, $lng, $node, $nodefaulttext || ($i < ($_i-1)), $firstfallback, $modulefallback);

        // Return the translation if found
        if ($translation != "")
          return $translation;
      }
      return "";

    }

    /**
     * Returns all strings for the given modulename.
     *
     * The returned struct will contain key-value pairs for the translation 
     * keys, and their respective translation.
     *  
     * @param String $module  Module in which the language file should be 
     * @param String $lng     ISO 639-1 language code, defaults to config 
     * @return array Translations 
     */
    public static function getStringsForModule($module, $lng="") 
    {

      if ($lng=="") $lng = atkLanguage::getLanguage();
      $atklanguage = atkLanguage::getInstance();

      $text = $atklanguage->_getStringFromFile($key, $module, $lng);
      $atklanguage->_includeLanguage($module, $lng);

      if (isset($atklanguage->m_cachedlang[$module])
          && is_array($atklanguage->m_cachedlang[$module][$lng])) {
              return $atklanguage->m_cachedlang[$module][$lng];
      }

      return array();

    }

    /**
     * Get the current language, either from url, or if that's not present, from what the user has set.
     * @static
     * @return String current language.
     */
    public static function getLanguage()
    {
      global $ATK_VARS;

      if (isset($ATK_VARS["atklng"]) && (in_array($ATK_VARS["atklng"], atkLanguage::getSupportedLanguages()) || in_array($ATK_VARS["atklng"], atkconfig('supported_languages'))))
      {
        $lng = $ATK_VARS["atklng"];
      } // we first check for an atklng variable
      else { $lng = atkLanguage::getUserLanguage(); }
      return strtolower($lng);
    }

    /**
     * Change the current language.
     * Note that his only remains set for the current request, it's not
     * session based.
     * @static
     * @param String $lng The language to set
     */
    public static function setLanguage($lng)
    {
      global $ATK_VARS;
      $ATK_VARS["atklng"] = $lng;
    }

    /**
     * Get the selected language of the current user if he/she set one,
     * otherwise we try to get it from the browser settings and if even THAT
     * fails, we return the default language.
     *
     * @static
     * @return unknown
     */
    public static function getUserLanguage()
    {
      $supported = atkLanguage::getSupportedLanguages();
      $sessionmanager=null;
      if(function_exists('atkGetSessionManager')) $sessionmanager = &atkGetSessionManager();
      if (!empty($sessionmanager))
      {
        if (function_exists("getUser"))
        {
          $userinfo = getUser();
          $fieldname = atkconfig('auth_languagefield');
          if (isset($userinfo[$fieldname]) && in_array($userinfo[$fieldname],$supported)) return $userinfo[$fieldname];
        }
      }

      // Otherwise we check the headers
      if (atkconfig('use_browser_language', false))
      {
        $headerlng = atkLanguage::getLanguageFromHeaders();
        if ($headerlng && in_array($headerlng,$supported)) return $headerlng;
      }

      // We give up and just return the default language
      return atkconfig('language');
    }

    /**
     * Get the primary languagecode that the user has set in his/her browser
     *
     * @static
     * @return String The languagecode
     */
    public static function getLanguageFromHeaders()
    {
      if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE']))
      {
        $langs = split ('[,;]',$_SERVER['HTTP_ACCEPT_LANGUAGE']);
        if ($langs[0]!="")
        {
          $elems=explode("-", $langs[0]); // lng might contain a subset after the dash.
          $autolng=$elems[0];
        }
      }
      return $autolng;
    }
    
    /**
     * Explicitly sets the supported languages.
     * 
     * @param array $languages supported languages
     */
    public static function setSupportedLanguages(array $languages)
    {
      self::$s_supportedLanguages = $languages;
    }

    /**
     * Get the languages supported by the application
     *
     * @static
     * @return Array An array with the languages supported by the application.
     */
    public static function getSupportedLanguages()
    {
      $supportedlanguagesmodule = atkconfig('supported_languages_module');
      if (self::$s_supportedLanguages == null && $supportedlanguagesmodule)
      {
        $supportedlanguagesdir = atkLanguage::getLanguageDirForModule($supportedlanguagesmodule);
        atkimport('atk.utils.atkdirectorytraverser');
        $supportedlanguagescollector = new getSupportedLanguagesCollector();
        $traverser = new atkDirectoryTraverser();
        $traverser->addCallbackObject($supportedlanguagescollector);
        $traverser->traverse($supportedlanguagesdir);
        self::$s_supportedLanguages = $supportedlanguagescollector->getLanguages();
      }

      return (array) self::$s_supportedLanguages;
    }

    /**
     * Determine the list of modules we need to go through to check
     * language strings. Overrides have precedence, then the
     * passed module is considered, finally if no string is found
     * the fallbacks are checked.
     *
     * @access protected
     * @param String $module manually passed module
     * @param String $firstfallback an additional module in which the
     *        translation will be searched first, if not found in the
     *        module itself.
     * @param Boolean $modulefallback If true, *all* modules are checked.
     * @return array List of modules to use to find the translations
     */
    protected function _getModules($module, $firstfallback="", $modulefallback=false)
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
     * @access protected
     * 
     * @param string $key             the name of the string to return
     * @param string $module          module in which the language file should be looked for,
     *                                defaults to core module with fallback to ATK
     * @param string $lng             ISO 639-1 language code, defaults to config variable
     * @param string $node            the node to which the string belongs
     * @param bool   $nodefaulttext   wether or not to pass a default text back
     * @param string $firstfallback   the first module to check as part of the fallback
     * @param bool   $modulefallback  Wether or not to use all the modules of the application in the fallback,
     *                                when looking for strings
     * @return string the name with which to call the string we want from the languagefile
     */
    protected function _getString($key, $module, $lng, $node="", $nodefaulttext=false, $firstfallback="", $modulefallback=false)
    {
      // First find node specific string.
      $modules = $this->_getModules($module, $firstfallback, $modulefallback);

      // Second check custom Strings
      if(isset($this->m_customStrings[$lng]) && isset($this->m_customStrings[$lng][$key]))
        return $this->m_customStrings[$lng][$key];

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
        if (atkconfig("debug_translations", false))
          atkdebug("atkLanguage: translation for '$key' with module: '$module' and node: '$node' and language: '$lng' not found, returning default text");

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
     * @access protected
     * @param string $module  the module to import the language file from
     * @param string $lng     language of file to import
     * @return bool true if everything went okay
     */
    protected function _includeLanguage($module, $lng)
    {
      if (!isset($this->m_cachedlangfiles[$module][$lng])||$this->m_cachedlangfiles[$module][$lng] != 1)
      {
        $this->m_cachedlangfiles[$module][$lng] = 1;
        $path = $this->getLanguageDirForModule($module);

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
     * Method for getting the relative path to the languagedirectory
     * of a module.
     * Supports 2 special modules:
     * - atk (returns the path of the atk languagedir)
     * - langoverrides (returns the path of the languageoverrides dir)
     *
     * Special method in that it can run both in static and non-static
     * mode.
     *
     * @param String $module The module to get the languagedir for
     * @return String The relative path to the languagedir
     */
    public function getLanguageDirForModule($module)
    {
      if ($module=="atk")
      {
        $path = atkconfig("atkroot")."atk/".(isset($this)?$this->LANGDIR:'languages/');
      }
      else if ($module=="langoverrides")
      {
        $path = atkconfig("language_basedir",(isset($this)?$this->LANGDIR:'languages/'));
      }
      else
      {
        $path = moduleDir($module).(isset($this)?$this->LANGDIR:'languages/');
      }
      return $path;
    }

    /**
     * A function to change the original "$something_text" string to
     * "Something text"
     * This is only used when we really can't find the "$something_text" anywhere
     * @param string $string the name of the string to return
     * @return string the changed string
     */
    public function defaultText($string)
    {
      return ucfirst(str_replace("_"," ",str_replace('title_','',$string)));
    }

    /**
     * Gets the string from the languagefile or, if we failed, returns ""
     *
     * @access protected
     * @param string $key           the name which was given when the text function was called
     * @param string $module        the name of the module to which the text function belongs
     * @param string $lng           the current language
     * @return var the true name by which the txt is called or "" if we can't find any entry
     */
    protected function _getStringFromFile($key, $module, $lng)
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

    /**
     * Set a custom language string
     *
     * @param string $code The code of the custom string
     * @param string $text Text
     * @param string $lng Language
     */
    public function setText($code,$text,$lng)
    {
      if(!isset($this->m_customStrings[$lng])) $this->m_customStrings[$lng]=array();
      $this->m_customStrings[$lng][$code]=$text;
    }
  }


  /**
   * A collector for supported languages
   * @author Boy Baukema <boy@ibuildings.nl>
   * @package atk
   */
  class getSupportedLanguagesCollector
  {
    var $m_languages=array();

    function visitFile($fullpath)
    {
      if (substr($fullpath,strlen($fullpath)-4)==='.lng')
      {
        $exploded = explode('/',$fullpath);
        $lng = array_pop($exploded);
        $this->m_languages[] = substr($lng,0,2);
      }
    }

    public function getLanguages()
    {
      return $this->m_languages;
    }
  }
?>
