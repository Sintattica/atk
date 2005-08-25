<?php

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
      echo '<b>'.text("login_failed").'</b>';      
      echo '<br><br>'.$securityMgr->m_fatalError;
      exit;
    }
  }      
  
  /**
   * Retrieve all known information about the currently logged-in user.
   * @return array Array with userinfo, or "" if no user is logged in.
   */
  function getUser()
  {
    global $g_sessionManager;
    $session = &atkSessionManager::getSession();
    $user = "";    
    $session_auth = $g_sessionManager->getValue("authentication", "globals");          
    if (atkconfig("authentication_session") && 
        $session["login"]==1 && 
        $session_auth["authenticated"]==1 && 
        !empty($session_auth["user"]))
    {
      $user = $session_auth["user"];
      if($user["access_level"]=="") $user["access_level"]=0;
    }

    return $user;
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


?>