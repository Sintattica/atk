<?php

  /**
   * This class manages security in atk.
   *
   * <b>Todo's</b>   : - everything
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * <b>Changes</b>:
   *    
   * $Id$
   * $Log$
   * Revision 4.13  2001/06/19 12:58:09  peter
   * Session cookie parameters changed so the session cookie will not only
   * be domain based but also domain/path based!
   *
   * Revision 4.12  2001/06/19 11:41:49  ivo
   * Authentication data is now stored globaly instead of per namespace.
   *
   * Revision 4.11  2001/06/18 22:49:02  peter
   * Added ability to logout of the HTTP authentication (set variable atklogout to something...)
   *
   * Revision 4.10  2001/06/18 12:58:20  ivo
   * Bugfix in securitymanager constructor.
   *
   * Revision 4.9  2001/06/18 10:43:41  peter
   * Small bug fix with authorization... :/
   *
   * Revision 4.8  2001/06/18 10:24:58  peter
   * Split POP3/IMAP authentication support (mainly a new POP3 auth class).
   * Authentication and authorization can now be done by different auth classes.
   *
   * Revision 4.7  2001/06/12 12:24:23  ivo
   * New feature: users can now have more then one group in db authentication.
   *
   * Revision 4.6  2001/05/22 15:22:39  peter
   * Different small enhancements fixes...
   *
   * Revision 4.5  2001/05/21 15:18:26  ivo
   * Made securitymanager cookie and session based. Authentication now only
   * executes once per session.
   *
   * Revision 4.4  2001/05/21 13:38:18  ivo
   * Added a cookie that can remember your login if you close your browser.
   * (disabled by default, enable in your configfile if you need it)
   *
   * Revision 4.3  2001/05/07 15:13:49  ivo
   * Put config_atkroot in all files.
   *
   * Revision 4.2  2001/05/01 09:49:49  ivo
   * Replaced all require() and include() calls by require_once() and
   * include_once() calls. The if(!DEFINED)... inclusion protection in files
   * is now obsolete.
   *
   * Revision 4.1  2001/04/24 13:51:50  ivo
   * Fixed some small bugs, and updated the language files, improved the menu.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:14  ivo
   * Added atk4 to repository.
   *
   * Revision 1.7  2001/04/02 08:22:58  ivo
   * Improved groupbased attribute security.
   *
   * Revision 1.6  2001/03/29 11:22:29  ivo
   * New feature: logging
   *
   * Revision 1.5  2001/03/16 07:51:59  ivo
   * Improved group-based security.
   *
   * Revision 1.4  2001/03/14 16:36:39  ivo
   * Fixed a bug in the g_user variable.
   *
   * Revision 1.3  2001/03/14 16:16:30  sandy
   * database name and userid field is configurable
   *
   * Revision 1.2  2001/03/13 15:09:03  ivo
   * user is now stored in a global variable.
   *
   * Revision 1.1  2001/02/15 16:20:24  ivo
   * Major new feature: security.
   *
   *
   */
   
  require_once($config_atkroot."atk/security/class.auth_interface.inc");
  
  if ($config_authentication == "") $config_authentication = "none";
  if ($config_authorization  == "") $config_authorization = $config_authentication;
  if ($config_securityscheme == "") $config_securityscheme = "none";
   
  include_once($config_atkroot."atk/security/class.auth_".$config_authentication.".inc");
  include_once($config_atkroot."atk/security/class.auth_".$config_authorization.".inc");
   
  class atkSecurityManager
  {
    var $m_authentication = 0;    
    var $m_authorization = 0; 
    var $m_scheme = "none";
    var $m_user = 0;
        
     
    // constructor
    function atkSecurityManager($authentication_type="none", $authorization_type="none", $securityscheme="none")
    {       
     
      atkdebug("creating securityManager (authenticationtype: $authentication_type, authorizationtype: $authorization_type, scheme: $securityscheme)");

      /* authentication class */       
      $classname = "auth_".$authentication_type;
      $this->m_authentication = new $classname;             
      
      /* authorization class */
      if ($authorization_type == $authentication_type)
      {
        $this->m_authorization = &$this->m_authentication;
      }
      else
      { 
        $classname = "auth_".$authorization_type; 
        $this->m_authorization = new $classname;
      }
 
      /* security scheme */
      $this->m_scheme = $securityscheme;
    }
     
    function authenticate()
    {
      global $PHP_AUTH_USER, $PHP_AUTH_PW, $SCRIPT_NAME, $config_administratorpassword;
      global $g_sessionManager, $HTTP_COOKIE_VARS, $HTTP_AUTHORIZATION, $ATK_VARS, $atklogout;
      $md5 = false; // PHP_AUTH_PW is plain text..            

      // first check if we want to logout
      if (isset($atklogout) && !session_is_registered("relogin"))
      {
        session_destroy();
        $cookie_params = session_get_cookie_params();
        session_set_cookie_params($cookie_params["lifetime"], dirname($SCRIPT_NAME));
        session_start();
        session_register("relogin");

        // destroy cookie
        if (atkconfig("authentication_cookie") && $PHP_AUTH_USER!="administrator")
        {
          $cookiename = "atkauth_".str_replace(" ","_",text("app_title"));
          if (!empty($HTTP_COOKIE_VARS[$cookiename])) setcookie($cookiename, "", 0);
        }
      } 
      
      // do we need to login?
      else if (!session_is_registered("login"))
      { 
        // authenticated?
        $authenticated = false;

        // sometimes we manually have to set the PHP_AUTH vars 
        if (empty($PHP_AUTH_USER) && empty($PHP_AUTH_PW) && ereg("^Basic ", $HTTP_AUTHORIZATION))
        list($PHP_AUTH_USER, $PHP_AUTH_PW) = explode(":", base64_decode(substr($HTTP_AUTHORIZATION, 6)));

        // check previous sessions.. 
        if (atkconfig("authentication_cookie"))
        {
          // Cookiename is based on the app_title, for there may be more than 1 atk app running,
          // each with their own cookie..
          $cookiename = "atkauth_".str_replace(" ","_",text("app_title"));
          list($user, $passwd) = split("\.",$HTTP_COOKIE_VARS[$cookiename]);                           
       
          // for security reasons administrator will never be cookied..
          if ($PHP_AUTH_USER=="" && $user!="" && $user!="administrator")
          {        
            atkdebug("Using cookie to retrieve previously used userid/password");
            $PHP_AUTH_USER = $user;
            $PHP_AUTH_PW = $passwd;
            $md5 = true; // cookie is already md5;
          }
        }              
                          
        // check superuser       
        if ($config_administratorpassword!=""&&$PHP_AUTH_USER=="administrator"&&$PHP_AUTH_PW==$config_administratorpassword)
        {
          $this->m_user = Array("name"=>"administrator","level"=>-1);
          // for convenience, we also store the user as a global variable.             
          atkdebug("Administrator (configfile) logged in");
          $authenticated = true;
        } 

        // other users     
        else 
        {     
          if (atkconfig("authentication_md5")&&!$md5)
          {
            $PHP_AUTH_PW = md5($PHP_AUTH_PW);
          }       
          if($this->m_authentication->validateUser($PHP_AUTH_USER,$PHP_AUTH_PW))
          {   
            // We store the username + securitylevel of the logged in user. 
            $this->m_user = $this->m_authorization->getUser($PHP_AUTH_USER);
            // for convenience, we also store the user as a global variable.             
            atkdebug("Logged in user: ".$this->m_user["name"]." (level: ".$this->m_user["level"].")");
            $authenticated = true;

            // Remember that we are logged in..
            $g_sessionManager->globalVar("authentication",array("authenticated"=>1, "user"=>$this->m_user), true);

            // write cookie
            if (atkconfig("authentication_cookie")&&$PHP_AUTH_USER!="administrator")
            {
              setcookie($cookiename,$PHP_AUTH_USER.".".$PHP_AUTH_PW, time()+60*(atkconfig("authentication_cookie_expire")));
            }
          }
        }

        // we are logged in 
        if ($authenticated) session_register("login");
      }

      // use session for authentication
      else
      {
    //    $session_auth = $ATK_VARS["authentication"];
      $session_auth = $g_sessionManager->getValue("authentication", true);  
        if (atkconfig("authentication_session") && session_is_registered("login") && !empty($session_auth["authenticated"]) && !empty($session_auth["user"]))
        {
          $this->m_user = $session_auth["user"];
          atkdebug("Using session for authentication");
        }
      }

      // still not logged in?! 
      if (!session_is_registered("login"))
      {
        header('WWW-Authenticate: Basic realm="'.text("app_title").' - '.strftime("%c",time()).'"');
        if (ereg("Microsoft", $SERVER_SOFTWARE)) header("Status: 401 Unauthorized");
        else header("HTTP/1.0 401 Unauthorized");
        return false;
      }

      // we are authenticated, but atklogout is still active, let's get rid of it!
      else if (isset($atklogout)) header("Location: $PHP_SELF");

      // we keep the relogin state until the atklogout variable isn't set anymore
      else if (!isset($atklogout) && session_is_registered("relogin")) session_unregister("relogin");

      // return
      $GLOBALS["g_user"] = $this->m_user;
      return true;
    }     
          
    function allowed($node, $action)
    {
      if (($this->m_scheme=="none") || ($this->hasLevel(-1)) || (strtolower($this->m_user["name"])=="administrator")) 
      {
        // No securityscheme, or user is superuser.
        $allowed = true;
      }
      else
      {
       
        $required = $this->m_authorization->getEntity($node, $action);       
       
        if (count($required)==0)
        {
          // No access restrictions found.. 
          // so either nobody or anybody can perform this
          // operation, depending on the configuration.
          $allowed = !$GLOBALS["config_restrictive"];
        }
        else
        {
          if ($this->m_scheme=="level")
          {
            // in level based security, only one level is specified for each node/action combination.
            $allowed = ($this->m_user["level"] >= $required[0]);
          }
          else if ($this->m_scheme=="group")
          {           
            // user may have more then one level
            if (is_array($this->m_user["level"]))
            {
              $allowed = (count(array_intersect($this->m_user["level"], $required))>0);
            }
            else
            {
              // user has only one level
              $allowed = in_array($this->m_user["level"], $required);          
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
        
    function attribAllowed($node, $attribute, $mode)
    {
      global $config_security_attributes;
      
      if (($this->m_scheme=="none") || (!$config_security_attributes) || ($this->hasLevel(-1)) || (strtolower($this->m_user["name"])=="administrator")) 
      {
        // No securityscheme, or user is superuser.
        return true;
      }
       
      $required = $this->m_authorization->getAttribEntity($node, $attribute, $mode);
       
      if ($required==-1)
      {
        // No access restrictions found..          
        $allowed = true;
      }
      else
      {
        if ($this->m_scheme=="level")
        {
          $allowed = ($this->m_user["level"] >= $required);
        }
        else if ($this->m_scheme=="group")
        {
          $allowed = in_array($this->m_user["level"],$required);
        }
        else // unknown scheme??
        {
          $allowed = false;
        }
      }
     
      return $allowed;
     
    }
    
    /**
     * Returns true if the current logged in user has
     * the given level.
     */
    function hasLevel($level)
    {
      if (is_array($this->m_user["level"]))
      {
        return in_array($level, $this->m_user["level"]);
      }
      else
      {
        return $this->m_user["level"]==$level;
      }
    }
     
    function logAction($node, $action)
    {
      $this->log(2,"Performing $node.$action");
    }
     
    function log($level, $message)
    {
     global $config_logging, $config_logfile, $REMOTE_ADDR;
    
     if ($config_logging>0 && $config_logging>=$level)
     {
       $fp = @fopen($config_logfile,"a");
       if ($fp)
       {
         $logstamp = "[".date("d-m-Y H:i:s")."] [".$REMOTE_ADDR."] ".$this->m_user["name"]." | ";
         @fwrite($fp, $logstamp.$message."\n");
         @fclose($fp);
      }
     }
    }
     
  }
  
  /* 
    Calling this function will invoke the login process. Call this function in 
    every file that you want to have secured.
    (This is actually a small wrapper for $g_securityManager->authenticate(), 
    so you can quickly secure an application.
   */
  function atksecure()
  {
    global $g_securityManager;
    
    if (!$g_securityManager->authenticate())
    {
      echo text("login_failed");
      atkdebug("login failed");
      exit;
    }
  }      
   
  // The one and only security manager.   
  $g_securityManager = new atkSecurityManager($config_authentication, $config_authorization, $config_securityscheme);

?>
