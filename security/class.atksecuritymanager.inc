<?php

  /**
   * This class manages security in atk.
   *
   * @author Ivo Jansch (ivo@achievo.com)
   * @version $Revision$
   *
   * $Id$   
   *
   */
   
  require_once($config_atkroot."atk/security/class.auth_interface.inc");
  
  // initialise g_user array.
  $g_user = array();
  
  if ($config_authentication == "") $config_authentication = "none";
  if ($config_authorization  == "") $config_authorization = $config_authentication;
  if ($config_securityscheme == "") $config_securityscheme = "none";
   
  $authentication = explode(",",trim($config_authentication));
  if(is_array($authentication))
  {
    foreach($authentication as $value)
    {
      $value = trim($value);
      if(is_file($config_atkroot."atk/security/class.auth_".$value.".inc"))
      {
        include_once($config_atkroot."atk/security/class.auth_".$value.".inc");
      }
    }
  }
  else
  {
    if(is_file($config_atkroot."atk/security/class.auth_".$authentication.".inc"))
    {
      include_once($config_atkroot."atk/security/class.auth_".$authentication.".inc");
    }
  }

  include_once($config_atkroot."atk/security/class.auth_".$config_authorization.".inc");
   
  class atkSecurityManager
  {
    var $m_authentication = "";    
    var $m_authorization = 0; 
    var $m_scheme = "none";
    var $m_user = 0;
    
    // If login really fails (no relogin box, but an errormessage), the 
    // error message that caused the fatal error is put in this variable.
    var $m_fatalError = "";
             
    // constructor
    function atkSecurityManager($authentication_type="none", $authorization_type="none", $securityscheme="none")
    {       
     
      atkdebug("creating securityManager (authenticationtype: $authentication_type, authorizationtype: $authorization_type, scheme: $securityscheme)");

      /* authentication class */       
      $authentication = explode(",",trim($authentication_type));
      if(is_array($authentication))
      {
        foreach($authentication as $class)
        {
          $class = trim($class);
          if($class!="")
          {
            $classname = "auth_".$class;
            if(class_exists($classname))
            {
              atkdebug("atkSecurityManager() constructing class $classname");
              $this->m_authentication[$class] = new $classname;
            }
          }
        }
      }
      else
      {
        $classname = "auth_".$authentication;
        if(class_exists($classname))
        {
          atkdebug("atkSecurityManager() constructing class $classname");
          $this->m_authentication[$authentication] = new $classname;
        }
      }          
      /* authorization class */    
      //if ($authorization_type == $authentication_type)
      //{
//        $this->m_authorization = &$this->m_authentication;
  //    }
    //  else
      //{ 
        $classname = "auth_".$authorization_type; 
        $this->m_authorization = new $classname;
      //}
 
      /* security scheme */
      $this->m_scheme = $securityscheme;
    }
     
    function authenticate()
    {    
      global $PHP_AUTH_USER, $PHP_AUTH_PW, $SCRIPT_NAME, $SERVER_NAME, $config_administratorpassword;
      global $g_sessionManager, $HTTP_COOKIE_VARS, $HTTP_AUTHORIZATION, $ATK_VARS, $atklogout;
      global $config_guestpassword;
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
        if($atklogout>1)
        {
          header("Location: logout.php");
          exit;
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
          list($user, $passwd) = split("\.",base64_decode($HTTP_COOKIE_VARS[$cookiename]));          
       
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
          $this->m_user = Array("name"=>"administrator","level"=>-1, "access_level"=>9999999);
          // for convenience, we also store the user as a global variable.                     
          atkdebug("Administrator (configfile) logged in");
          $authenticated = true;
        } 
        // check guest user
        else if ($config_guestpassword!=""&&$PHP_AUTH_USER=="guest"&&$PHP_AUTH_PW==$config_guestpassword)
        {
          $this->m_user = Array("name"=>"guest","level"=>-2, "access_level"=>0);
          // for convenience, we also store the user as a global variable.                     
          atkdebug("Guest (configfile) logged in");
          $authenticated = true;
        }
        // other users     
        else 
        {     
          $response=0;
          if(is_array($this->m_authentication))
          {
            foreach($this->m_authentication as $name => $obj)
            {
              $obj->canMd5()&&!$md5 ? $tmp_pw=md5($PHP_AUTH_PW) : $tmp_pw=$PHP_AUTH_PW;
              $response = $obj->validateUser($PHP_AUTH_USER,$tmp_pw);
              if($response==1)
              {
                atkdebug("atkSecurityManager::authenticate() using $name authentication");
                $authname=$name;
                break;
              }
            }
          }
          if($response==1) // succesful login
          {   
            // We store the username + securitylevel of the logged in user. 
            $this->m_user = $this->m_authorization->getUser($PHP_AUTH_USER);
            $this->m_user['AUTH']=$authname; // something to see wich auth scheme is used
            // for convenience, we also store the user as a global variable.             
            atkdebug("Logged in user: ".$this->m_user["name"]." (level: ".$this->m_user["level"].")");
            $authenticated = true;

            // Remember that we are logged in..
            //$g_sessionManager->globalVar("authentication",array("authenticated"=>1, "user"=>$this->m_user), true);

            // write cookie
            if (atkconfig("authentication_cookie")&&$PHP_AUTH_USER!="administrator")
            {
              setcookie($cookiename,base64_encode($PHP_AUTH_USER.".".$PHP_AUTH_PW), time()+60*(atkconfig("authentication_cookie_expire")));
            }
          }
          else
          {
            // login was incorrect. Either the supplied username/password combination is 
            // incorrect (we just try again) or there was an error (we display an error 
            // message)
            if ($response==-1)
            {
              $this->m_fatalError = $this->m_authentication->m_fatalError;
            }
            $authenticated = false;            
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
        if (atkconfig("authentication_session") && 
            session_is_registered("login") && 
            $session_auth["authenticated"]==1 && 
            !empty($session_auth["user"]))
        {
          $this->m_user = $session_auth["user"];
          atkdebug("Using session for authentication / user = ".$this->m_user["name"]);
          atk_var_dump($this->m_user);
        }
      }

      // if there was an error, drop out.
      if ($this->m_fatalError!="") 
      {        
        return false;
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
      else if (isset($atklogout)) header("Location: $PHP_SELF"."?");

      // we keep the relogin state until the atklogout variable isn't set anymore
      else if (!isset($atklogout) && session_is_registered("relogin")) session_unregister("relogin");

      // return
      // g_user always lowercase
      $this->m_user["name"] =  strtolower($this->m_user["name"]);
      $GLOBALS["g_user"] = $this->m_user;
      $g_sessionManager->globalVar("authentication",array("authenticated"=>1, "user"=>$this->m_user), true);
      return true;
    }     
          
    function allowed($node, $action)
    {
      // security disabled or user is superuser? (may do anything)
      if (($this->m_scheme=="none") || ($this->hasLevel(-1)) || (strtolower($this->m_user["name"])=="administrator")) 
      {        
        $allowed = true;
      }
      // user is guest? (guests may do nothing)
      else if (($this->hasLevel(-2)) || (strtolower($this->m_user["name"])=="guest"))
      {
        $allowed = false;
      }
      // all other situations
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
      
      // No securityscheme, or user is superuser.
      if (($this->m_scheme=="none") || (!$config_security_attributes) || ($this->hasLevel(-1)) || (strtolower($this->m_user["name"])=="administrator")) 
      {        
        return true;
      }
      
      // User is guest (may do nothing)
      if (($this->hasLevel(-2)) || (strtolower($this->m_user["name"])=="administrator")) 
      {        
        return false;
      }
      
      // all other situations
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
        else
        {
          atkdebug("error opening logfile");
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
      echo '<b>'.text("login_failed").'</b>';      
      echo '<br><br>'.$g_securityManager->m_fatalError;
      exit;
    }
  }      
  
  /**
   * Retrieve the currently logged in user.
   * if no user is logged in, returns "".
   */
  function getUser()
  {
    global $g_sessionManager;
    $user = "";    
    $session_auth = $g_sessionManager->getValue("authentication", true);          
    if (atkconfig("authentication_session") && 
        session_is_registered("login") && 
        $session_auth["authenticated"]==1 && 
        !empty($session_auth["user"]))
    {
      $user = $session_auth["user"];
      if($user["access_level"]=="") $user["access_level"]=0;
    }    
    return $user;
  }
   
  // The one and only security manager.   
  $g_securityManager = new atkSecurityManager($config_authentication, $config_authorization, $config_securityscheme);

?>
