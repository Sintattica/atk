<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage include
   *
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  // initialise g_ array.
  $g_user = array();

  /**
   * Calling this function will invoke the login process. Call this function in
   * every file that you want to have secured.
   * (This is actually a small wrapper for $securityManager->authenticate(),
   * so you can quickly secure an application.
   */
  function atksecure()
  {
    $securityMgr = &atkGetSecurityManager();

    if (!$securityMgr->authenticate())
    {
      echo '<b>'.atktext("login_failed", "atk").'</b>';
      echo '<br><br>'.$securityMgr->m_fatalError;
      exit;
    }
  }

  /**
   * Retrieve all known information about the currently logged-in user.
   * @deprecated use atkGetUser instead
   * @return array Array with userinfo, or "" if no user is logged in.
   */
  function getUser()
  {
    return atkGetUser();
  }

  /**
   * Retrieve all known information about the currently logged-in user.
   * @todo move to its own 'atkUser' class
   * @return array Array with userinfo, or "" if no user is logged in.
   */  
  function atkGetUser()
  {
    global $g_sessionManager;
    $session = &atkSessionManager::getSession();
    $user = "";
    $session_auth = $g_sessionManager->getValue("authentication", "globals");
    if (atkconfig("authentication_session") &&
        atkArrayNvl($session, "login", 0)==1 &&
        $session_auth["authenticated"]==1 &&
        !empty($session_auth["user"]))
    {
      $user = $session_auth["user"];
      if($user["access_level"]=="") $user["access_level"]=0;
    }

    return $user;    
  }

  /**
   * Retrieve id of the currently logged-in user.
   * @todo move to its own 'atkUser' class
   * @return int user id or 0 if not logged in or administrator
   */
  function atkGetUserId()
  {
    $user = atkGetUser();
    $userpk = atkConfig('auth_userpk');
    
    // check if logged in || logged in as administrator
    if ($user == "" || $userpk == "" ||
       (is_array($user) && !isset($user[$userpk]))) return 0;
       
    return $user[$userpk];
  }

  /**
   * Wrapper method to access the security manager.
   * @todo Rewrite to a getInstance method in atkSecurityManager.
   */
  function &atkGetSecurityManager()
  {
    global $g_securityManager; // WORKAROUND: we use a global var since many
                               // atk apps rely on a global variable being present.
                               // In ATK5 we must create a clean singleton with
                               // a static instance.
    if (!is_object($g_securityManager))
    {
      atkimport("atk.security.atksecuritymanager");
      // The one and only security manager.
      $authentication = atkconfig("authentication", "none");
      $authorization = atkconfig("authorization", $authentication);
      $scheme = atkconfig("securityscheme", "none");
      $g_securityManager = new atkSecurityManager($authentication, $authorization, $scheme);
    }
    return $g_securityManager;
  }

  /**
   * Replaced the securitymanager at runtime with a different instance.
   *
   * Useful for testing, for example replacing the security manager with a
   * mock version.
   * @param atkSecurityManager Securitymanager instance
   * @return atkSecurityManager The original instance
   */
  function &atkSetSecurityManager(&$mgr)
  {
    global $g_securityManager;
    $obj = $g_securityManager;
    $g_securityManager = $mgr;
    return $obj;
  }
?>