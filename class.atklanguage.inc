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
    var $m_fallbackmodules;
    
    /**
     * Name of the module in which caller expected the string to be. If it 
     * turns out to be in a different module, we can inform the caller that
     * it passed the wrong module (using the debugoutput).
     *
     * @access private
     * @var String
     */
    var $m_originalmodule;
    
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
      $s_atklanguage->_fillFallbackModules();
      return $s_atklanguage;
    }
    
    /**
     * Calculate the list of fallbackmodules.
     * @access private
     */
    function _fillFallbackModules()
    {
      global $g_modules;
      
      $this->m_fallbackmodules=array();
      if (is_array($g_modules))
      { 
        foreach ($g_modules as $modname => $modpath)
        {
          $this->m_fallbackmodules[] = $modname;
        }
      }
      $this->m_fallbackmodules[] = "atk";
    }      
    
    /**
     * Text function, retrieves a translation for a certain string.
     *
     * @param String $string         the name of the string to return
     * @param String $module         module in which the language file should be looked for, defaults to core module with fallback to ATK
     * @param String $node           the node to which the string belongs
     * @param String $lng            ISO 639-1 language code, defaults to config variable
     * @param String $firstfallback  the first module to check as part of the fallback
     * @param boolean $nodefaulttext if true, then it doesn't returns false when it can't find a translation
     * @return String the string from the languagefile
     */
    function text($string, $module, $node="", $lng="", $firstfallback="", $nodefaulttext=false)
    {
      $atklanguage =& atkLanguage::getInstance();
      
      if ($atklanguage->preTextChecks($string, $module, $node, $lng, $firstfallback, $nodefaulttext))
      {
        $finalstring = $atklanguage->_getStringName($string, $module, $node, $lng, $nodefaulttext);
      }
      $atklanguage->postTextChecks($finalstring, $string, $module, $node, $lng, $firstfallback, $nodefaulttext);
      return $finalstring;
    }
      
    
    /**
     * This function performs some checks on the variables that were passed with the text call
     * if it fails this check and a false is returned atkLanguage will not continue with getting the string
     *
     * @access private
     * @param string &$string       the name of the string to return
     * @param string &$module       module in which the language file should be looked for, defaults to core module with fallback to ATK
     * @param string &$node         the node to which the string belongs
     * @param string &$lng          ISO 639-1 language code, defaults to config variable
     * @param string $firstfallback the first module to check as part of the fallback
     * @param bool $nodefaulttext   if true, then it doesn't returns false when it can't find a translation
     * @return bool                 if true then it will go on, if not it will not look up the string
     */
    function preTextChecks(&$string, &$module, &$node, &$lng, $firstfallback, $nodefaulttext)
    {
      global $ATK_VARS;
      
      if ($module)
      {
        $this->m_originalmodule = $module;
        array_unshift($this->m_fallbackmodules, $module);
      }
      
      if ($firstfallback)
      {
        array_unshift($this->m_fallbackmodules, $firstfallback);
      }
            
      $module = "langoverrides";
      
      if (!$lng && $ATK_VARS["lng"]) { $lng = $ATK_VARS["lng"]; } // we first check for a lng variable
      if (!$lng) { $lng = atkconfig("language", atkconfig("lang")); } // language is nicer, lang is backwardscompatible with iep2.
      $lng = strtolower($lng); // language files should always be lowercase
      
      if(is_numeric($string)) return false;
      return true;
    }
    
    /**
     * This function performs the final checks on the results of the text call
     *
     * @access private
     * @param string &$finalstring  the result of the text call
     * @param string &$string       the name/label/ID of the string to return
     * @param string &$module       module in which the language file should be looked for, defaults to core module with fallback to ATK
     * @param string &$node         the node to which the string belongs
     * @param string &$lng          ISO 639-1 language code, defaults to config variable
     * @param string $firstfallback the first module to check as part of the fallback
     * @param bool $nodefaulttext   if true, then it doesn't returns false when it can't find a translation
     */
    function postTextChecks(&$finalstring, $string, $module, $node, $lng, $firstfallback, $nodefaulttext)
    {
      if (!$finalstring)
      {
        if(!$nodefaulttext)
        {
          $finalstring = $this->defaultText($string); 
        }
        else 
        {
          $finalstring = false;
        }
      }
      return true;
    }
    
    /**
     * Gets the appropriate language file and name to call the string by
     *
     * @access private
     * @param string $string the name of the string to return
     * @param string $module module in which the language file should be looked for, defaults to core module with fallback to ATK
     * @param string $node   the node to which the string belongs
     * @param string $lng    ISO 639-1 language code, defaults to config variable
     * @return string the name with which to call the string we want from the languagefile
     */
    function _getStringName($string, &$module, $node, $lng, &$nodefaulttext)
    {
      if (atkconfig("language_debugging")) atkdebug("Checking for string $string <br /> with module: $module <br /> node: $node <br /> and language: $lng");
      if ($this->_getLanguageFile($module,$lng))
      {
        $finalstring = $this->_getString($string, $node, $module, $lng, $nodefaulttext);
        if($finalstring)
        {
          $this->_checkTextForModule($module, $string);
          return $finalstring;
        }
        else {return $this->_stringNameFallback($string, $node, $lng, $nodefaulttext);}
      }
      else {return $this->_stringNameFallback($string, $node, $lng, $nodefaulttext);}
    }
    
    /**
     * Verifies wether the text function was called with the proper module.
     *
     * Displays debug text when this is not the case.
     * Doing this prevents the fallback function from having to scan every languagefile.
     * Thus saving lots of performance
     *
     * @access private
     * @param string $module the module with which the text function was called
     * @param string $string the string by which we call the string
     */
    function _checkTextForModule($module, $string)
    {
      if ($module !== $this->m_originalmodule || !$module)
      {
        if (!$this->m_originalmodule)
        {
          atkdebug("atkLanguage: $string was passed without module, we found it in $module, please change the text() function accordingly");
        }
        else
        {
          atkdebug("atkLanguage: $string was not found in $this->m_originalmodule but in $module, please change text() function accordingly");
        }
      }
    }
    
    /**
     * Fallback function for the getStringName function
     *
     * Will walk through every fallbackmodule and call getStringName for it
     * until it produces a result or runs out of fallbackmodules
     *
     * @access private
     * @param string $string the name of the string to return
     * @param string $node   the node to which the string belongs
     * @param string $lng    ISO 639-1 language code, defaults to config variable
     */
    function _stringNameFallback($string, $node, $lng, $nodefaulttext)
    {
      $module = array_shift($this->m_fallbackmodules);
      if ($module) 
      { 
        return $this->_getStringName($string, $module, $node, $lng, $nodefaulttext); 
      }
      else 
      { 
        if ($nodefaulttext)
        {
          atkdebug("atkLanguage: We ran out of modules! Non-empty $string just couldn't be found");
        }
        return; 
      }
    }
    
    /**
     * Gets the languagefile and sets it in the member variable
     *
     * @access private
     * @param string $module the name of the module we are using
     * @param string $lng    the ISO code for the language of the languagefile we want to get
     * @return bool true if everything went okay, false otherwise
     */
    function _getLanguageFile($module, $lng)
    {    
      if ($module=="atk")
      {
        $file = atkconfig("atkroot").$module."/".$this->LANGDIR."$lng.lng";
      }
      else if ($module=="langoverrides")
      {
        $file = atkconfig("language_basedir",$this->LANGDIR)."$lng.lng";
      }
      else
      {
        $file = moduleDir($module).$this->LANGDIR."$lng.lng";
      }
      if(file_exists($file)) {return $this->_setLanguage($file, $module, $lng);}
      else 
      {
        if (atkconfig("language_debugging")) atkdebug("atkLanguage: failed to get language file for module: $module with language: $lng"); 
        return false;
      }
    }
    
    /**
     * Checks wether the language is set or not
     *
     * If set, it does nothing and return true
     * otherwise it sets it
     *
     * @access private
     * @param string $file file to import
     * @param string $lng  language of file to import
     * @return bool true if everything went okay
     */
    function _setLanguage($file, $module, $lng)
    {
      if ($this->m_cachedlangfiles[$module][$lng] != $file)
      {
        include($file);
        $this->m_cachedlangfiles[$module][$lng] = $file;
        $this->m_cachedlang[$module][$lng] = ${$lng};
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
     * Gets the string from the languagefile or, if we failed, returns false
     *
     * @access private
     * @param string $string    the name which was given when the text function was called
     * @param string $module    the name of the module to which the text function belongs
     * @param string $node      the name of the node to which the text function belongs
     * @return var the true name by which the txt is called or false if we can't find any entry
     */
    function _getString($string, $node, $module, $lng, &$nodefaulttext)
    { 
      if (atkconfig("atklangcheckmodule"))
      {
        if ($node)
        {
          $key = $node."_".$string;
          if (empty($this->m_cachedlang[$module][$lng][$key]) && atkconfig("atklangcheckmodule") == 2)
          {
            $key = $module."_".$string;
            if (empty($this->m_cachedlang[$module][$lng][$key]))
            {
              $key = $module."_".$node."_".$string; 
            }
          }
        }          
      }
      if (!$key || !$this->m_cachedlang[$module][$lng][$key])
      { 
        $key = $string;
      }
      
      if (is_array($this->m_cachedlang[$module][$lng]))
      {
        if (in_array($key, array_keys($this->m_cachedlang[$module][$lng])))
        {
          if ($this->m_cachedlang[$module][$lng][$key]) return $this->m_cachedlang[$module][$lng][$key];
          else $nodefaulttext=true;
        }
      }
      return;
    }
  }
?>
