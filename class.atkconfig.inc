<?php

  // This should be a static class member, but that doesn't exist
  // in PHP4.
  global $g_atkconfig;

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