<?php
class atkLanguage
{
  /**
  * Private
  * $atklanguage obj an instance of the atkLanguage class
  */
  var $atklanguage;
  var $LANGDIR = "languages";
  var $m_cachedlang = array();
  var $m_cachedlangfiles = array();
  var $m_fallbackmodules;
  var $m_originalmodule;
  
  /**
  * Constructor
  */
  function atkLanguage()
  {
    atkdebug("New instance made of atkLanguage");
  }
     
  /**
  * Gets an instance of the atkLanguage class
  * Using this function will ensure that only 1 instance ever exists
  * @return obj instance of the atkLanguage class
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
  * Gets the fallbackmodules
  */
  function _fillFallbackModules()
  {
    global $g_modules;
    
    $this->m_fallbackmodules=array();
    
    foreach ($g_modules as $modname => $modpath)
    {
      $this->m_fallbackmodules[] = $modname;
    }
    $this->m_fallbackmodules[] = "atk";
  }
  
  /**
  * Sets the module the text function was called with,
  * this is later used for comparison against where the string was actually found
  * @param string $module the name of the module
  */
  function setOriginalModule($module)
  {
    $this->m_originalmodule = $module;
  }
  
  /**
  * Text function, main function of the class
  * @param string $string        the name of the string to return
  * @param string $module        module in which the language file should be looked for, defaults to core module with fallback to ATK
  * @param string $node          the node to which the string belongs
  * @param string $lng           ISO 639-1 language code, defaults to config variable
  * @param string $firstfallback the first module to check as part of the fallback
  * @param bool $nodefaulttext   if true, then it doesn't returns false when it can't find a translation
  * @return string the string from the languagefile
  */
  function text($string, $module, $node="", $lng="", $firstfallback="", $nodefaulttext=false)
  {
    $atklanguage =& atkLanguage::getInstance();
    
    $atklanguage->preTextChecks($string, $module, $node, $lng, $firstfallback, $nodefaulttext);
    $finalstring = $atklanguage->getStringName($string, $module, $node, $lng);
    $atklanguage->postTextChecks($finalstring, $string, $module, $node, $lng, $firstfallback, $nodefaulttext);
    
    return $finalstring;
  }
    
  
  /**
  * This function performs some checks on the variables that were passed with the text call
  * @param string &$string       the name of the string to return
  * @param string &$module       module in which the language file should be looked for, defaults to core module with fallback to ATK
  * @param string &$node         the node to which the string belongs
  * @param string &$lng          ISO 639-1 language code, defaults to config variable
  * @param string $firstfallback the first module to check as part of the fallback
  * @param bool $nodefaulttext   if true, then it doesn't returns false when it can't find a translation
  */
  function preTextChecks(&$string, &$module, &$node, &$lng, $firstfallback, $nodefaulttext)
  {
    if ($module)
    {
      $this->setOriginalModule($module);
      array_unshift($this->m_fallbackmodules, $module);
    }
    
    if ($firstfallback)
    {
      array_unshift($this->m_fallbackmodules, $firstfallback);
    }
    
    $module = "langoverrides";
    
    if (!$lng) { $lng = atkconfig("language", atkconfig("lang")); } // language is nicer, lang is backwardscompatible with iep2.
  }
  
  /**
  * This function performs the final checks on the results of the text call
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
  }
  
  /**
  * Gets the appropriate language file and name to call the string by
  * @param string $string the name of the string to return
  * @param string $module module in which the language file should be looked for, defaults to core module with fallback to ATK
  * @param string $node   the node to which the string belongs
  * @param string $lng    ISO 639-1 language code, defaults to config variable
  * @return string the name with which to call the string we want from the languagefile
  */
  function getStringName($string, &$module, $node, $lng)
  {
//    atkdebug("Checking for string $string <br /> with module: $module <br /> node: $node <br /> and language: $lng");
    if ($this->_getLanguageFile($module,$lng))
    {
      $finalstring = $this->_getString($string, $node, $module, $lng);
      if($finalstring)
      {
        $this->_checkTextForModule($module, $string);
        return $finalstring;
      }
      else {return $this->_stringNameFallback($string, $node, $lng);}
    }
    else {return $this->_stringNameFallback($string, $node, $lng);}
  }
  
  /**
  * Verifies wether the text function was called with the proper module,
  * displays debug text when this is not the case
  * Doing this prevents the fallback function from having to scan every languagefile
  * Thus saving lots of performance
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
  * Will walk through every fallbackmodule and call getStringName for it
  * until it produces a result or runs out of fallbackmodules
  * @param string $string the name of the string to return
  * @param string $node   the node to which the string belongs
  * @param string $lng    ISO 639-1 language code, defaults to config variable
  */
  function _stringNameFallback($string, $node, $lng)
  {
    $module = array_shift($this->m_fallbackmodules);
    if ($module) { return $this->getStringName($string, $module, $node, $lng); }
    else { atkdebug("atkLanguage: We ran out of modules! $string just couldn't be found"); return; }
  }
  
  /**
  * Gets the languagefile and sets it in the member variable
  * @param string $module the name of the module we are using
  * @param string $lng    the ISO code for the language of the languagefile we want to get
  * @return bool true if everything went okay, false otherwise
  */
  function _getLanguageFile($module, $lng)
  {    
    if ($module=="atk")
    {
      $file = atkconfig("atkroot").$module."/".$this->LANGDIR."/$lng.lng";
    }
    else if ($module=="langoverrides")
    {
      $file = atkconfig("atktempdir").$this->LANGDIR."/$lng.lng";
    }
    else
    {
      $file = moduleDir($module).$this->LANGDIR."/$lng.lng";
    }
    if(file_exists($file)) {return $this->_setLanguage($file, $module, $lng);}
    else {return false;}
  }
  
  /**
  * Checks wether the language is set or not
  * If set, it does nothing and return true
  * otherwise it sets it
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
  * @param string $string    the name which was given when the text function was called
  * @param string $module    the name of the module to which the text function belongs
  * @param string $node      the name of the node to which the text function belongs
  * @param bool $checkmodule wether or not to check for module specific strings
  * @return var the true name by which the txt is called or false if we can't find any entry
  */
  function _getString($string, $node, $module, $lng, $checkmodule=true)
  {    
    if ($checkmodule)
    {
      if ($node)
      {
        $key = $node."_".$string;
        if (empty($this->m_cachedlang[$module][$lng][$key]))
        {
          $key = $module."_".$node."_".$string;
          if (empty($this->m_cachedlang[$module][$lng][$key]))
          {
            $node = "";
            return $this->_getString($string, $node, $module, $lng);
          }
        }
      }
      else
      {
        $key = $module."_".$string;
        if (empty($this->m_cachedlang[$module][$lng][$key]))
        {
          return $this->_getString($string, $node, $module, $lng,false);
        }
      }
    }
    else { $key = $string; }
    
    return (!empty($this->m_cachedlang[$module][$lng][$key])?$this->m_cachedlang[$module][$lng][$key]:false);
  }
}
?>