<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk  
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
     
  /**
   * @internal This should be a static class member, but that doesn't exist
   *           in PHP4.
   * @global atkConfig The global config object   
   */
  global $g_atkconfig;

  /**
   * Config class for loading configfiles and retrieving configoptions.
   *
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @todo Fixme :-) This class is currently not functional. Use the atkconfig()
   *                 function for retrieving config options instead.
   *
   */
  class atkConfig
  {      
    function get($varname)
    {
      global $g_atkconfig;
      
      $elems = explode(".", $varname);
      if (count($elems)==1)
      {
        // Global configvar. 
        if (!is_array($g_atkconfig["global"]))
        {
          atkdebug("Loading global config");
          // Global configfile not yet included.          
          atkConfig::loadConfig(ATK_CONFIGROOT."config.inc.php", "global");
        }
        return $g_atkconfig["global"][$varname];
      }
      else
      {
        // Module configvar.
        if (!is_array($g_atkconfig[$elems[0]]))
        {
          atkConfig::loadConfig(ATK_CONFIGROOT.moduleDir().
        }
        return $g_atkconfig[$elems[0]][$elems[1]];
      }
    }
    
    function loadConfig($configfile, $scope="global")
    {
      global $g_atkconfig;
      
      $config = array();
      
      include($configfile);
      $g_atkconfig[$scope]=$config;
          
      // Backwardscompatibilityhack: most configvariables in
      // older apps are in the form $config_...; We must put those
      // manually in the $g_atkconfig list. 
      $vars = get_defined_vars();
      foreach ($vars as $key=>$value)
      {
        if (substr($key, 0, 7)=="config_")
        {
          $g_atkconfig[$scope][substr($key,7)] = $value;
        }
      }
    }
  }
  
?>