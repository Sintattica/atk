<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage security
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6289 $
   * $Id$
   */
   
  /**
   * This authentication method supports retrieval of the current password
   * (alternative to current $config_.....)
   */
  define ("PASSWORD_RETRIEVABLE", 2);
  
  /**
   * This authentication method supports generation of a new password for the user
   * (so password forgotten can work, even though password is encoded and not
   * retrievable). Clicking 'forgot password' results in an email with a new
   * password.
   */
  define ("PASSWORD_RECREATE", 1);

  /**
   * This authentication method supports neither retrieve, nor recreation. E.g. when using
   * LDAP, this means that ATK will not provide password forgotten.
   */
  define ("PASSWORD_STATIC", 0);
  
  /**
   * This class is the abstract baseclass (interface) for all auth_ classes. 
   * All new authentication/authorization methods need to derive from this 
   * class.   
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage security
   * @abstract
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
     * @param String $user The login of the user to authenticate.
     * @param String $passwd The password of the user. Note: if the canMd5 
     *                       function of an implementation returns true,      
     *                       $passwd will be passed as an md5 string.
     *
     * @return int AUTH_SUCCESS - Authentication succesful
     *             AUTH_MISMATCH - Authentication failed, wrong 
     *                             user/password combination
     *             AUTH_LOCKED - Account is locked, can not login
     *                           with current username.
     *             AUTH_ERROR - Authentication failed due to some 
     *                          error which cannot be solved by 
     *                          just trying again. If you return 
     *                          this value, you *must* also 
     *                          fill the m_fatalError variable.
     */
    function validateUser($user, $passwd)
    {      
      return AUTH_ERROR; // dummy implementation. should not be used.
    }

    /** 
     * Logout handling. The default implementation does simple session destruction
     * and recreates a new session.
     * @param array $user The user data (as returned by atkGetUser() 
     */
    function logout($user)
    {
        session_destroy();

        $cookie_params = session_get_cookie_params();
        $cookiepath   = atkconfig("application_root");
        $cookiedomain = (atkconfig("cookiedomain") != "") ? atkconfig("cookiedomain") : NULL;
        session_set_cookie_params($cookie_params["lifetime"], $cookiepath, $cookiedomain);
        @session_start(); 
    }
 
    /**
     * Does the authentication method support md5 encoding of passwords?
     *
     * @return boolean True if md5 is always used. false if md5 is not
     *                 supported.
     *                 Drivers that support both md5 and cleartext passwords
     *                 can return atkconfig("authentication_md5") to let the
     *                 application decide whether to use md5.
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
     * Specific implementations of the method may add more information if
     * necessary.
     *
     * @param String $user The login of the user to retrieve.
     * @return array Information about a user.
     */
    function getUser($user)
    {
      return array("name"=>$user,"level"=>-1); // dummy implementation, should not be used.
    }
    
    /**
     * Check if the currently logged-in user has a certain privilege on a
     * node.
     * 
     * @param atkSecurityManager $securityMgr The security manager instance.
     * @param String $node The full nodename of the node for which to check
     *                     access privileges. (modulename.nodename notation).
     * @param String $privilege The privilege to check (atkaction).
     * @return boolean True if the user has the privilege, false if not.
     */    
    function allowed(&$securityMgr, $node, $privilege)
    {
      // security disabled or user is superuser? (may do anything)
      if (($securityMgr->m_scheme=="none") || ($securityMgr->hasLevel(-1)) || (strtolower($securityMgr->m_user["name"])=="administrator"))
      {
        $allowed = true;
      }
      // user is guest? (guests may do nothing)
      else if (($securityMgr->hasLevel(-2)) || (strtolower($securityMgr->m_user["name"])=="guest"))
      {
        $allowed = false;
      }
      // other
      else
      {
        $required = $this->getEntity($node, $privilege);
  
        if (count($required)==0)
        {
          // No access restrictions found..
          // so either nobody or anybody can perform this
          // operation, depending on the configuration.
          $allowed = !atkconfig("restrictive");
        }
        else
        {
          if ($securityMgr->m_scheme=="level")
          {
            // in level based security, only one level is specified for each node/action combination.
            $allowed = ($securityMgr->m_user["level"] >= $required[0]);
          }
          else if ($securityMgr->m_scheme=="group")
          {
            // user may have more then one level
            if (is_array($securityMgr->m_user["level"]))
            {
              $allowed = (count(array_intersect($securityMgr->m_user["level"], $required))>0);
            }
            else
            {
              // user has only one level
              $allowed = in_array($securityMgr->m_user["level"], $required);
            }
  
          }
          else // unknown scheme??
          {
            $allowed = false;
          }
        }  
      }
      
      return $allowed;    
    }
    

    /**
     * Check if the currently logged-in user has the right to view, edit etc.
     * an attribute of a node.
     * 
     * @param atkSecurityManager $securityMgr  the security manager
     * @param atkAttribute       $attr         attribute reference
     * @param string             $mode         mode (add, edit, view etc.)
     * @param array              $record       record data
     * 
     * @return boolean true if access is granted, false if not.
     */
    function attribAllowed(&$securityMgr, &$attr, $mode, $record=NULL)   
    {
      $node = $attr->m_ownerInstance->atkNodeType();
      $attribute = $attr->fieldName();
      
      // security disabled or user is superuser? (may do anything)
      if (($securityMgr->m_scheme=="none") || (!atkconfig("security_attributes"))  || ($securityMgr->hasLevel(-1)) || (strtolower($securityMgr->m_user["name"])=="administrator"))
      {
        $allowed = true;
      }
      // user is guest? (guests may do nothing)
      else if (($securityMgr->hasLevel(-2)) || (strtolower($securityMgr->m_user["name"])=="guest"))
      {
        $allowed = false;
      }
      // other
      else
      {      
        // all other situations
        $required = $this->getAttribEntity($node, $attribute, $mode);
  
        if ($required==-1)
        {
          // No access restrictions found..
          $allowed = true;
        }
        else
        {
          if ($securityMgr->m_scheme=="level")
          {
            $allowed = ($securityMgr->m_user["level"] >= $required);
          }
          else if ($securityMgr->m_scheme=="group")
          {
            $allowed = in_array($securityMgr->m_user["level"],$required);
          }
          else // unknown scheme??
          {
            $allowed = false;
          }
        }
      }
      
      return $allowed;              
    }
    
    /**
     * This function returns the level/group(s) that are allowed to perform
     * the given action on a node.
     * @param String $node The full nodename of the node for which to check
     *                     the privilege. (modulename.nodename)
     * @param String $action The privilege to check.
     * @return mixed One (int) or more (array) entities that are allowed to
     *               perform the action.
     */
    function getEntity($node, $action)
    {
      return array();
    }
    
    /** 
     * This function returns the level/group(s) that are allowed to 
     * view/edit a certain attribute of a given node.
     * @param String $node The full nodename of the node for which to check
     *                     attribute access.
     * @param String $attrib The name of the attribute to check
     * @param String $mode "view" or "edit"
     * @param mixed One (int) or more (array) entities that are allowed to 
     *              view/edit the attribute. 
     */
    function getAttribEntity($node, $attrib, $mode)
    {
      return array();
    }    
    
    /**
     * This function returns the list of users that may login. This can be
     * used to display a dropdown of users from which to choose.
     *
     * Implementations that do not support retrieval of a list of users, 
     * should either not implement this method, or return an empty array.
     *
     * @return array List of users as an associative array with the following 
     *               format: array of records, each record is an associative
     *               array with a userid and a username field.
     */
    function getUserList()
    {
      return array();
    }
    
    /**
     * This function returns "get password" policy for current auth method
     *
     * @return const
     */
    function getPasswordPolicy()
    {
      return PASSWORD_STATIC;    
    }
    
    /**
     * This function returns password or false, if password can't be retrieve/recreate
     * 
     * @param string $username User for which the password should be regenerated
     *
     * @return mixed string with password or false
     */
    function getPassword($username)
    {
      return false;    
    }
    
  }

?>
