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
        $config = array();
        @include(atkconfig("configdir").$section.".inc.php");        
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
    
    /**
     * Restrict access to an attribute to a certain entity (group or level)
     *
     * When $config_authorization is set to "config", this method can be used
     * to restrict access to certain attributes for a given entity.
     * This means that certain users can not edit or even view some attributes
     * in a node. This is called "attribute level security".
     *
     * If this method is called on a node/attrib combination, only those users
     * who match the level/group can view/edit the attribute. If no calls are
     * made for an attribute, the attribute is considered unrestricted and every
     * user has access.
     *
     * @param String $node The node on which access is restricted.
     * @param String $attrib The name of the attribute that is to be restricted.
     * @param String $mode The action to restrict ("edit" or "view")
     * @param mixed $entity The level/group that has access to the attribute.
     */
    function attribRestrict($node,$attrib,$mode,$entity)
    {
      $GLOBALS["config_attribrestrict"][$node][$attrib][$mode] = $entity;
    }
  
    /**
     * Grants acces to an entity (group or level)
     *
     * When $config_authorization is set to "config", this method can be used
     * in the configfile to grant privileges.
     *
     * @param String $node The node on which to grant a privilege.
     * @param String $action The action (privilege) that is granted.
     * @param mixed $entity The entity (securitylevel or group) to which the 
     *                      privilege is granted.
     */
    function grant($node,$action,$entity)
    {
      $GLOBALS["config_access"][$node][] = Array($action=>$entity);
    }
    
    /**
     * Translate pop3 server responses to user readable error messages.
     *
     * This function is only of use when using pop3 as authentication method.
     * Some pop3 servers give specific error messages that may be of interest 
     * to the user. If you use this function (in the config file) and atk
     * encounters the specified substring in a server response, the specified
     * message is displayed.
     *
     * @param String $substring The substring to look for in the server
     *                          response.
     * @param String $message The message to display to the user upon encounter
     *                        of the substring.
     */
    function addPop3Response($substring, $message)
    {
      global $g_pop3_responses;
      $g_pop3_responses[$substring] = $message;
    }
    
    /**
     * Create a new user.
     *
     * When $config_authentication is set to "config", this method can be used 
     * in the configfile to create users. Mind you that anybody who has read
     * access on the config file, can read the passwords. It is advisable to 
     * use a more secure authentication method like "db" or "pop3".
     *
     * @param String $name The login name.
     * @param String $password The password of the user.
     * @param mixed $securitylevel The securitylevel or group of the user.
     *                             Permissions are granted on level/group basis,
     *                             depending on the setting of 
     *                             $config_security_scheme
     */
    function addUser($name, $password, $securitylevel=0)
    {
      $GLOBALS["config_user"][$name] = Array("password"=>$password,"level"=>$securitylevel);
    }  
  }
  
  /**
   * Retrieve a configuration value.
   *
   * In all of your code, you can use this method to retrieve a certain
   * configuration setting. Any setting defined as $config_something can be
   * retrieved as atkconfig("something").
   *
   * @param String $tag The name of the configuration variable (without the
   *                    '$config_' prefix!
   * @param mixed $default The default variable you want to use when the 
   *                       configuration variable is missing from the
   *                       config file.   
   *
   * @deprecated Use atkConfig::get() instead.
   * @return mixed The value of the configuration variable.
   */
  function atkconfig($tag, $default="")
  {
    $fn = 'config_'.$tag;
    if (function_exists($fn)) return $fn();
    return (isset($GLOBALS["config_".$tag])?$GLOBALS["config_".$tag]:$default);
  }    
  
  /** @todo: module() and the MF_ flags should be moved to moduletools, but these are
   * not present yet at configfile load time.
   */
  
  /**
   * Module flags 
   */
  define("MF_NOMENU",                          1); // don't use the menuitems from this module
  define("MF_NORIGHTS",                        2); // don't use the rights of this module
  define("MF_SPECIFIC_1",                      4);
  define("MF_SPECIFIC_2",                      8);
  define("MF_SPECIFIC_3",                     16);
  define("MF_REFERENCE",   MF_NOMENU|MF_NORIGHTS); // use this module only as a reference
  define("MF_NO_PRELOAD",                     32);
  
  /**
   * Load a module.
   *
   * This method is used in the config.inc.php or config.modules.inc file to
   * load the modules.
   *
   * @param String $name The name of the module to load.
   * @param String path The path where the module is located (relative or
   *                    absolute). If omitted, ATK assumes that the module is
   *                    installed in the default module dir (identified by 
   *                    $config_module_path).
   * @param int flags The module (MF_*) flags that influence how the module is
   *                  loaded.
   */
  function module($name, $path="", $flags=0)
  {
    global $g_modules, $config_module_path, $g_moduleflags;
    if ($path=="") $path = $config_module_path."/".$name."/";
    $g_modules[$name] = $path;
    if ($flags>0) $g_moduleflags[$name] = $flags;
  }
  
?>