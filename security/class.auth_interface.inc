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
   * Revision 4.3  2001/07/19 11:40:07  ivo
   * Updated authentication classes with an additional returncode for fatal
   * errors. Added a feature to pop3 authentication to handle server-specific
   * response codes.
   *
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
     * If authentication fails due to an error, instead of a wrong 
     * username/password combination, this variable must be filled
     * with an explanation of the reason for the error.
     */ 
    var $m_fatalError;
     
    /**
     * Authenticate a user. 
     *
     * Allowed return values: 1 - Authentication succesful
     *                        0 - Authentication failed, wrong user/password 
     *                            combination
     *                       -1 - Authentication failed due to some error
     *                            which cannot be solved by just trying again
     *                            If you return this value, you *must* also 
     *                            fill the m_fatalError variable.
     *
     * Note: if the canMd5 function returns true, $passwd is passed as an md5 string.
     */
    function validateUser($user, $passwd)
    {      
      return -1; // dummy implementation. should not be used.
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
    
    /** 
     * THE FOLLOWING FUNCTIONS MUST ONLY BE IMPLEMENTED IF YOUR AUTH CLASS
     * CAN DO AUTHORISATION. IF YOU DON'T IMPLEMENT THEM, AND IN YOUR APPLICATION
     * YOU USE YOUR CLASS BOTH FOR AUTHENTICATION AND AUTHORISATION, EVERY USER
     * HAS EQUAL RIGHTS AND IS TREATED AS ADMINISTRATOR.
     */
    
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
      return array("name"=>$user,"level"=>-1); // dummy implementation, should not be used.
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
        
  }

?>