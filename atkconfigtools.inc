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

    /* module flags */
    define("MF_NOMENU",                          1); // don't use the menuitems from this module
    define("MF_NORIGHTS",                        2); // don't use the rights of this module
    define("MF_SPECIFIC_1",                      4);
    define("MF_SPECIFIC_2",                      8);
    define("MF_SPECIFIC_3",                     16);
    define("MF_REFERENCE",   MF_NOMENU|MF_NORIGHTS); // use this module only as a reference

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

    /**
     * Define an external module.
     *
     * @param name the module name
     * @param path the module path
     * @param flags the module flags (see above)
     */
    function module($name, $path="", $flags=0)
    {
      global $g_modules, $config_module_path, $g_moduleflags;
      if ($path=="") $path = $config_module_path."/".$name."/";
      $g_modules[$name] = $path;
      if ($flags>0) $g_moduleflags[$name] = $flags;
    }

    /**
     * Function atkconfig
     *
     * @return the value of a config variable
     */
    function atkconfig($tag)
    {
      return $GLOBALS["config_".$tag];
    }

?>
