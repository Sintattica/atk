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
   * Config class for loading config files and retrieving config options.
   * Also contains misc. methods for use in config files.
   *
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   *
   */
  class atkConfig
  {      
    function get($section, $tag, $default="")
    {
      static $s_configs = array();
      $fn = 'config_'.$section.'_'.$tag;
      if (function_exists($fn)) return $fn();
      if (!is_array($s_configs[$section]))
      {
        $s_configs[$section] = array();
        atkdebug("Loading config file for section $section");
        @include_once(atkconfig("configdir").$section.".inc.php");
        if (isset($config)) $s_configs[$section] = $config;
      }
      return (isset($s_configs[$section][$tag])?$s_configs[$section][$tag]:$default);
    }
    
    /**
     * Is debugging enabled for client IP?
     *
     * @static
     */
    function ipDebugEnabled($params)
    {
      $ip = "unknown";
      if (getenv("HTTP_CLIENT_IP"))
        $ip = getenv("HTTP_CLIENT_IP");
      elseif (getenv("HTTP_X_FORWARDED_FOR"))
        $ip = getenv("HTTP_X_FORWARDED_FOR");
      elseif (getenv("REMOTE_ADDR"))
        $ip = getenv("REMOTE_ADDR");
      
      return in_array($ip, $params["list"]);
    }
    
    /**
     * Is debugging enabled by special request variable?
     *
     * @static
     */
    function requestDebugEnabled($params)
    {
      $session = &atkSessionManager::getSession();      
      
      if ($_REQUEST["atkdebug"]["key"])
        $session["debug"]["key"] = $_REQUEST["atkdebug"]["key"];
      return $session["debug"]["key"] == $params["key"];
    }

    /**
     * Returns a debug level based on the given options for
     * dynamically checking/setting the debug level. If nothing
     * found returns the default level.
     *
     * @static
     */
    function smartDebugLevel($default, $options=array())
    {
      $session = &atkSessionManager::getSession();

      $enabled = $default > 0;
      
      foreach ($options as $option)
      {
        $method = $option["type"]."DebugEnabled";
        if (is_callable(array("atkconfig", $method)))
          $enabled = $enabled || atkconfig::$method($option);
      }

      if ($enabled)
      {
        if (isset($_REQUEST["atkdebug"]["level"]))
          $session["debug"]["level"] = $_REQUEST["atkdebug"]["level"];
          
        if (isset($session["debug"]["level"]))
          return $session["debug"]["level"];
        else return max($default, 1);
      }

      return 0;
    }
  }
?>