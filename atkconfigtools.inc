<?php
  /**
   * Atk Config Tools
   *
   * A collection of functions to use in configfiles..
   *
   * Author: Ivo Jansch <ivo@ibuildings.nl>
   *
   * $Id$
   *
   $ $Log$
   $ Revision 4.1  2001/07/19 11:40:06  ivo
   $ Updated authentication classes with an additional returncode for fatal
   $ errors. Added a feature to pop3 authentication to handle server-specific
   $ response codes.
   $
   $ Revision 4.0  2001/04/23 10:19:38  ivo
   $ Revision number update
   $
   $ Revision 1.1.1.1  2001/04/23 10:17:02  ivo
   $ Added atk4 to repository.
   $
   * Revision 1.2  2001/02/22 22:49:26  peter
   * added the language() method
   *
   * Revision 1.1  2001/02/15 16:20:24  ivo
   * Major new feature: security.
   *
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
?>
