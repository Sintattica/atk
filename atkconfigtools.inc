<?php
  /**
   *
   * A collection of functions to use in configfiles..
   *
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$
   *
   */

    /**
     * Function user
     *
     * Creates a new user
     */
    function user($name,$password,$securitylevel=0)
    {
      $GLOBALS["config_user"][$name] = Array("password"=>$password,"level"=>$securitylevel);
    }

    /**
     * Function grant
     *
     * Grants acces to an entity (group or level)
     *
     */
    function grant($node,$action,$entity)
    {
      $GLOBALS["config_access"][$node][] = Array($action=>$entity);
    }

    /**
     * Function attrib_restrict
     *
     * Restricts access to an attribute to a certain entity (group or level)
     *
     */
    function attrib_restrict($node,$attrib,$mode,$entity)
    {
      $GLOBALS["config_attribrestrict"][$node][$attrib][$mode] = $entity;
    }

    /**
     * Function language
     *
     * Adds a language for multi language forms
     *
     * @param $language the iso language code
     */
    function language($language)
    {
      $GLOBALS["config_language"][] = $language;
    }
    
    /**
     * This function is only of use when using pop3 as 
     * authentication method.
     * Some pop3 servers give specific error messages that may be
     * of interest to the user. If you use this function (in the 
     * config file), and atk encounters the specified substring
     * in a server response, the specified message is displayed.
     */
    function auth_pop3_response($substring, $message)
    {
      global $g_pop3_responses;
      $g_pop3_responses[$substring] = $message;
    }
  
    /* define an external module */  
    function module($name, $path="")
    {
      global $g_modules, $config_module_path;
      if ($path=="") $path = $config_module_path."/".$name;    
      $g_modules[$name] = $path;          
    }
?>
