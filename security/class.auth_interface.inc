<?php

  /**
   * This class is the interface for all auth_ classes. All new 
   * authentication/authorization methods need to derive from this 
   * class.   
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * <b>Changes</b>:
   *    
   * $Id$
   * $Log$
   * Revision 4.2  2001/07/09 15:40:46  ivo
   * Authentication classes can now indicate whether they support md5 encoding
   * of passwords.
   *
   * Revision 4.1  2001/05/21 13:38:18  ivo
   * Added a cookie that can remember your login if you close your browser.
   * (disabled by default, enable in your configfile if you need it)
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:14  ivo
   * Added atk4 to repository.
   *
   * Revision 1.3  2001/04/02 08:22:58  ivo
   * Improved groupbased attribute security.
   *
   * Revision 1.2  2001/03/16 07:52:00  ivo
   * Improved group-based security.
   *
   * Revision 1.1  2001/02/15 16:20:24  ivo
   * Major new feature: security.
   *
   *
   */

  class auth_interface 
  {         
    /**
     * Authenticate a user. 
     * Note: if the canMd5 function returns true, $passwd is passed as an md5 string.
     */
    function validateUser($user, $passwd)
    {
      return false; // dummy implementation. should not be used.
    }
    
    /** 
     * This function returns information about a user in an associative
     * array with the following elements:
     * "name" -> the userid (should normally be the same as the $user 
     *           variable that gets passed to it.
     * "level" -> The level/group(s) to which this user belongs.
     *
     */
    function getUser($user)
    {
      return array("name"=>$user,"level"=>0); // dummy implementation, should not be used.
    }
    
    /**
     * This function returns the level/group(s) that are allowed to perform
     * the given action on a node.
     */
    function getEntity($node, $action)
    {
      return array();
    }
    
    /** 
     * This function returns the level/group(s) that are allowed to 
     * view/edit ($mode) a certain attribute of a given node.
     */
    function getAttribEntity($node, $attrib, $mode)
    {
      return array();
    }
    
    /**
     * Does the authentication method support md5 encoding of passwords?
     *
     * This function should return: true if md5 is always used.
     *                              false if md5 is not supported.
     *                              atkconfig("authentication_md5") if it
     *                              is configurable.
     */ 
    function canMd5()
    {
      return atkconfig("authentication_md5");
    }
  }

?>