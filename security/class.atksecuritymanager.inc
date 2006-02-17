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
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal includes and definitions
   */

  // Some result defines
  define ("AUTH_UNVERIFIED", 0); // initial value.
  define ("AUTH_SUCCESS", 1);
  define ("AUTH_LOCKED", 2);
  define ("AUTH_MISMATCH", 3);
  define ("AUTH_ERROR",-1);

  /**
   * The security manager for ATK applications.
   *
   * Don't instantiate this class yourself, use the global instance
   * that is returned by atkGetSecurityManager() instead.
   *
   * @todo Make a real singleton with a getInstance class.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage security
   */
  class atkSecurityManager
  {
    var $m_authentication = "";
    var $m_authorization = 0;
    var $m_scheme = "none";
    var $m_user = array();

    // If login really fails (no relogin box, but an errormessage), the
    // error message that caused the fatal error is put in this variable.
    var $m_fatalError = "";

    /**
     * Constructor
     * @access private
     */
    function atkSecurityManager($authentication_type="none", $authorization_type="none", $securityscheme="none")
    {
      atkdebug("creating securityManager (authenticationtype: $authentication_type, authorizationtype: $authorization_type, scheme: $securityscheme)");

      $atkroot = atkconfig("atkroot");
      require_once($atkroot."atk/security/class.auth_interface.inc");


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
            include_once($atkroot."atk/security/class.".$classname.".inc");
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
        include_once($atkroot."atk/security/class.".$classname.".inc");
        if(class_exists($classname))
        {
          atkdebug("atkSecurityManager() constructing class $classname");
          $this->m_authentication[$authentication] = new $classname;
        }
      }

      /* authorization class */
      $classname = "auth_".$authorization_type;
      include_once($atkroot."atk/security/class.auth_".$authorization_type.".inc");
      $this->m_authorization = new $classname;

      /* security scheme */
      $this->m_scheme = $securityscheme;
    }

    /**
     * Perform user authentication.
     *
     * Called by the framework, it should not be necessary to call this method
     * directly.
     */
    function authenticate()
    {
      global $g_sessionManager, $ATK_VARS;
      $session = &atkSessionManager::getSession();

      $response=AUTH_UNVERIFIED;

      if (atkconfig("auth_loginform")==true) // form login
      {
        $auth_user = isset($ATK_VARS["auth_user"]) ? $ATK_VARS["auth_user"] : null;
        $auth_pw = isset($ATK_VARS["auth_pw"]) ? $ATK_VARS["auth_pw"] : null;
      }
      else // HTTP login
      {
        $auth_user = $_SERVER["PHP_AUTH_USER"];
        $auth_pw = $_SERVER["PHP_AUTH_PW"];
      }

      $md5 = false; // PHP_AUTH_PW is plain text..
      // first check if we want to logout
      if (isset($ATK_VARS["atklogout"]) && (!isset($session["relogin"])||$session["relogin"]!=1))
      {
        session_destroy();
        $cookie_params = session_get_cookie_params();
        session_set_cookie_params($cookie_params["lifetime"], dirname($_SERVER["SCRIPT_NAME"]));
        @session_start();
        unset($session);
        $session["relogin"]=1;

        // destroy cookie
        if (atkconfig("authentication_cookie") && $auth_user!="administrator")
        {
          $cookiename = $this->_getAuthCookieName();
          if (!empty($_COOKIE[$cookiename])) setcookie($cookiename, "", 0);
        }
        if($ATK_VARS["atklogout"]>1)
        {
          header("Location: logout.php");
          exit;
        }
      }
      // do we need to login?
      else if ((!array_key_exists("login", $session)) || ($session["login"]!=1))
      {
        // authenticated?
        $authenticated = false;

        // sometimes we manually have to set the PHP_AUTH vars
        // old style http_authorization
        if (empty($auth_user) && empty($auth_pw) && array_key_exists("HTTP_AUTHORIZATION", $_SERVER) && ereg("^Basic ", $_SERVER["HTTP_AUTHORIZATION"]))
        {
          list($auth_user, $auth_pw) = explode(":", base64_decode(substr($_SERVER["HTTP_AUTHORIZATION"], 6)));
        }
        // external authentication
        elseif (empty($auth_user) && empty($auth_pw) && !empty($_SERVER["PHP_AUTH_USER"]))
        {
          $auth_user = $_SERVER["PHP_AUTH_USER"];
          $auth_pw = $_SERVER["PHP_AUTH_PW"];
        }

        // check previous sessions..
        if (atkconfig("authentication_cookie"))
        {
          // Cookiename is based on the app_title, for there may be more than 1 atk app running,
          // each with their own cookie..
          $cookiename = $this->_getAuthCookieName();
          list($enc, $user, $passwd) = split("\.",base64_decode($_COOKIE[$cookiename]));

          // for security reasons administrator will never be cookied..
          if ($auth_user=="" && $user!="" && $user!="administrator")
          {
            atkdebug("Using cookie to retrieve previously used userid/password");
            $auth_user = $user;
            $auth_pw = $passwd;
            $md5 = ($enc=="MD5"); // cookie may already be md5;
          }
        }

        $authenticated = false;

        // check superuser
        if (atkconfig("administratorpassword")!=""&&$auth_user=="administrator"&&$auth_pw==atkconfig("administratorpassword"))
        {
          $this->m_user = array("name"=>"administrator","level"=>-1, "access_level"=>9999999);
          // for convenience, we also store the user as a global variable.
          atkdebug("Administrator (configfile) logged in");
          $authenticated = true;
        }
        // check guest user
        else if (atkconfig("guestpassword")!=""&&$auth_user=="guest"&&$auth_pw==atkconfig("guestpassword"))
        {
          $this->m_user = Array("name"=>"guest","level"=>-2, "access_level"=>0);
          // for convenience, we also store the user as a global variable.
          atkdebug("Guest (configfile) logged in");
          $authenticated = true;
        }
        // other users
        // we must first explicitly check that we are not trying to login as administrator or guest.
        // these accounts have been validated above. If we don't check this, an account could be
        // created in the database that provides administrator access.
        else if ($auth_user!="administrator"&&$auth_user!="guest")
        {
          if(is_array($this->m_authentication))
          {
            // We have a username, which we must now validate against several
            // checks. If all of these fail, we have a status of AUTH_MISMATCH.
            foreach($this->m_authentication as $name => $obj)
            {
              $obj->canMd5()&&!$md5 ? $tmp_pw=md5($auth_pw) : $tmp_pw=$auth_pw;
              $response = $obj->validateUser($auth_user,$tmp_pw);
              if($response==AUTH_SUCCESS)
              {
                atkdebug("atkSecurityManager::authenticate() using $name authentication");
                $authname=$name;
                break;
              }
            }
          }
          if($response==AUTH_SUCCESS) // succesful login
          {
            // We store the username + securitylevel of the logged in user.
            $this->m_user = $this->m_authorization->getUser($auth_user);
            $this->m_user['AUTH']=$authname; // something to see wich auth scheme is used
            if (atkconfig("enable_ssl_encryption")) $this->m_user['PASS']=$auth_pw; // used by aktsecurerelation to decrypt an linkpass
            // for convenience, we also store the user as a global variable.
            (is_array($this->m_user['level'])) ? $dbg = implode(",",$this->m_user['level']) : $dbg = $this->m_user['level'];
            atkdebug("Logged in user: ".$this->m_user["name"]." (level: ".$dbg.")");
            $authenticated = true;

            // Remember that we are logged in..
            //$g_sessionManager->globalVar("authentication",array("authenticated"=>1, "user"=>$this->m_user), true);

            // write cookie
            if (atkconfig("authentication_cookie")&&$auth_user!="administrator")
            {
              // if the authentication scheme supports md5 passwords, we can safely store
              // the password as md5 in the cookie.
              if ($md5) // Password is already md5 encoded
              {
                $tmppw = $auth_pw;
                $enc = "MD5";
              }
              else // password is not md5 encoded
              {
                if ($this->m_authentication[$authname]->canMd5()) // we can encode it
                {
                  $tmppw = md5($auth_pw);
                  $enc = "MD5";
                }
                else // authentication scheme does not support md5 encoding.
                     // our only choice is to store the password plain text
                     // :-(
                     // NOTE: If you use a non-md5 enabled authentication
                     // scheme then, for security reasons, you shouldn't use
                     // $config_authentication_cookie at all.
                {
                  $tmppw = $auth_pw;
                  $enc = "PLAIN";
                }
              }
              setcookie($cookiename,base64_encode($enc.".".$auth_user.".".$tmppw), time()+60*(atkconfig("authentication_cookie_expire")));
            }
          }
          else
          {
            // login was incorrect. Either the supplied username/password combination is
            // incorrect (we just try again) or there was an error (we display an error
            // message)
            if ($response==AUTH_ERROR)
            {
              $this->m_fatalError = $this->m_authentication->m_fatalError;
            }
            $authenticated = false;
          }
        }
        // we are logged in
        if ($authenticated) $session["login"]=1;

      }
      else
      {
        // using session for authentication, because "login" was registered.
        // but we double check with some more data from the session to see
        // if the login is really valid.
        $session_auth = $g_sessionManager->getValue("authentication", "globals");

        if (atkconfig("authentication_session") &&
            $session["login"]==1 &&
            $session_auth["authenticated"]==1 &&
            !empty($session_auth["user"]))
        {
          $this->m_user = $session_auth["user"];
          atkdebug("Using session for authentication / user = ".$this->m_user["name"]);
        }
        else
        {
          // Invalid session
          $authenticated = false;
        }
      }

      // if there was an error, drop out.
      if ($this->m_fatalError!="")
      {
        return false;
      }
      // still not logged in?!
      if (!isset($session["login"])||$session["login"]!=1)
      {
        if(atkconfig("auth_loginform"))
        {
          $this->loginForm($auth_user, $response);
          $output = &atkOutput::getInstance();
          $output->outputFlush();
          exit();
        }
        else
        {
          header('WWW-Authenticate: Basic realm="'.atktext("app_title").' - '.strftime("%c",time()).'"');
          if (ereg("Microsoft", $_SERVER['SERVER_SOFTWARE'])) header("Status: 401 Unauthorized");
          else header("HTTP/1.0 401 Unauthorized");
          return false;
        }
      }

      // we are authenticated, but atklogout is still active, let's get rid of it!
      else if (isset($ATK_VARS["atklogout"])) header("Location: ".$_SERVER["PHP_SELF"]."?");

      // we keep the relogin state until the atklogout variable isn't set anymore
      else if (!isset($ATK_VARS["atklogout"]) && isset($session["relogin"]) && $session["relogin"]==1) $session["relogin"]=0;

      // return
      // g_user always lowercase
      $this->m_user["name"] =  strtolower($this->m_user["name"]);
      //Send the username with the header
      //This way we can always retrieve the user from apache logs
      header('user: '.$this->m_user["name"]);
      $GLOBALS["g_user"] = $this->m_user;
      $g_sessionManager->globalVar("authentication",array("authenticated"=>1, "user"=>$this->m_user), true);
      sessionStore('loginattempts',''); //reset maxloginattempts
      return true;
    }

    /**
     * Reload the current user data.
     * This method should be called if userdata, for example name or other
     * fields, have been updated for the currently logged in user.
     *
     * The method will make sure that $g_securityManager->m_user, $g_user and
     * the authenticated user in the session are refreshed.
     */
    function reloadUser()
    {
      global $g_user, $g_sessionManager;
      $user = getUser();
      $this->m_user = $this->m_authorization->getUser($user["userid"]);
      $g_user = &$this->m_user;
      $old_auth = $g_sessionManager->getValue("authentication", "globals");
      $old_auth["user"] = $g_user;
      $g_sessionManager->globalVar("authentication",$old_auth, true);
    }

    /**
     * Display a login form.
     * @access private
     */
    function loginForm($defaultname, $lastresponse)
    {

      $loginattempts = sessionLoad('loginattempts');
      if($loginattempts=="") $loginattempts = 0;

      if($loginattempts=="")
      {
        $loginattempts = 1;
      }
      else
      {
        $loginattempts++;
      }

      sessionStore('loginattempts',$loginattempts);

      atkdebug('LoginAttempts: '.$loginattempts);

      $page = &atknew("atk.ui.atkpage");
      $ui = &atkinstance("atk.ui.atkui");

      $page->register_style($ui->stylePath("style.css"));

      $page->register_script(atkConfig('atkroot')."atk/javascript/tools.js");
      $page->register_script(atkConfig('atkroot')."atk/javascript/formfocus.js");

      $tplvars = Array();
      $output = "";
      $output.='<form action="'.$_SERVER["PHP_SELF"].'" method="post">';
      $output.=makeHiddenPostVars(array("atklogout", "loginattempts"));
      $output.='<br><br><table border=0 cellspacing=2 cellpadding=0 align=center>';

      $tplvars["atksessionformvars"] = makeHiddenPostVars(array("atklogout", "loginattempts"));
      $tplvars["formurl"] = $_SERVER["PHP_SELF"];

      // max_loginattempts of 0 means no maximum.
      if(atkconfig('max_loginattempts')>0 && $loginattempts>atkconfig('max_loginattempts'))
      {
        $output.="<tr><td class=table>".atktext('auth_max_loginattempts_exceeded')."<br><br></td></tr>";
        $tplvars["auth_max_loginattempts_exceeded"] = atktext('auth_max_loginattempts_exceeded');
      }
      else
      {
        // generate the username input field
        // based upon the config_authdropdown and auth. method
        $userField = $this->auth_userField($defaultname);
        $tplvars["username"] = atktext("username");
        $tplvars["password"] = atktext("password");
        $tplvars["userfield"] = $userField;
        $tplvars["passwordfield"] = '<input class="loginform" type="password" size="15" name="auth_pw" value="">';
        $tplvars["submitbutton"] = '<input class="button" type="submit" value="'.atktext('login').'">';
        $tplvars["title"] = atktext('login_form');

        if ($lastresponse == AUTH_LOCKED)
        {
          $output.="<tr><td colspan=3 class=error>".atktext('auth_account_locked')."<br><br></td></tr>";
          $tplvars["auth_account_locked"] = atktext('auth_account_locked');
        }
        else if($lastresponse == AUTH_MISMATCH)
        {
          $output.='<tr><td colspan=3 class=error>'.atktext('auth_mismatch').'<br><br></td></tr>';
          $tplvars["auth_mismatch"] = atktext('auth_mismatch');
        }

        // generate the form
        $output.="<tr><td valign=top>".atktext('username')."</td><td>:</td><td>".$userField."</td></tr>";
        $output.="<tr><td colspan=3 height=6></td></tr>";
        $output.="<tr><td valign=top>".atktext('password')."</td><td>:</td><td><input type=password size=15 name=auth_pw value=''></td></tr>";
        $output.="<tr><td colspan=3 align=center height=50 valign=middle><input class=button type=submit value=".atktext('login')."></td></tr>";

      }

      $output.='</table></form>';

      $output.='<SCRIPT type="text/javascript">placeFocus()</SCRIPT>';

      $tplvars["content"]=$output;
      $page->addContent($ui->render("login.tpl", $tplvars));
      $o = &atkOutput::getInstance();
      $o->output($page->render(atktext("app_title")));
    }

    /**
     * Generate field for entering the username (dropdown or input box,
     * depending on settings.
     * @access private
     */
    function auth_userField($defaultname)
    {
      if (atkconfig("auth_dropdown") == true)
      {
        $auth_types = $this->m_authentication;
        $userlist = array();

        // Administrator and guest user may be present.
        if (atkconfig("administratorpassword")!="")
        {
          $userlist[] = array("userid"=>"administrator", "username"=>"Administrator");
        }
        if (atkconfig("guestpassword")!="")
        {
          $userlist[] = array("userid"=>"guest", "username"=>"Guest");
        }

        foreach ($auth_types as $type => $obj)
        {
          $userlist = array_merge($userlist, $obj->getUserList());
        }

        $userField  = '<select name="auth_user">'."\n";

        for ($i=0,$_i=count($userlist);$i<$_i;$i++)
        {
          $selected = "";
          if (trim(strtolower($defaultname)) == strtolower(trim($userlist[$i]["userid"])))
          {
            $selected = " selected";
          }
          $userField .= "<option value='".$userlist[$i]["userid"]."'".$selected.">".$userlist[$i]["username"]."</option>\n";
        }
        $userField .= "</SELECT>\n";

        return $userField;
      }
      else
      {
        return '<input type="text" size="15" name="auth_user" value="'.$defaultname.'">';
      }
    }

    /**
     * Check if the currently logged-in user has a certain privilege on a
     * node.
     * @param String $node The full nodename of the node for which to check
     *                     access privileges. (modulename.nodename notation).
     * @param String $action The privilege to check.
     * @return boolean True if the user has the privilege, false if not.
     */
    function allowed($node, $privilege)
    {
      static $_cache = array();

      if (!isset($_cache[$node][$privilege]))
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

          $required = $this->m_authorization->getEntity($node, $privilege);

          if (count($required)==0)
          {
            // No access restrictions found..
            // so either nobody or anybody can perform this
            // operation, depending on the configuration.
            $allowed = !atkconfig("restrictive");
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

        $_cache[$node][$privilege] = $allowed;
      }

      return $_cache[$node][$privilege];
    }

    /**
     * Check if the currently logged-in user has the right to view or edit
     * an attribute of a node.
     * @param String $node The full nodename of the node for which to check
     *                     access.
     * @param String $attribute The name of the attribute to check.
     * @param String $mode "edit" or "view"
     * @return boolean True if access is granted, false if not.
     */
    function attribAllowed($node, $attribute, $mode)
    {
      // No securityscheme, or user is superuser.
      if (($this->m_scheme=="none") || (!atkconfig("security_attributes")) || ($this->hasLevel(-1)) || (strtolower($this->m_user["name"])=="administrator"))
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
     * Check if the currently logged in user has the requested level.
     * @param int $level The level to check.
     * @return boolean True if the user has the required level, false if not.
     */
    function hasLevel($level)
    {
      if (is_array($level))
      {
        if (is_array($this->m_user["level"]))
        {
          return (count(array_intersect($this->m_user["level"], $level)) >= 1);
        }
        else
        {
          return in_array($this->m_user["level"], $level);
        }
      }
      else
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
    }

    /**
     * Write an access entry in the logfile.
     * @param String $node The full name of the node that is being accessed.
     * @param String $action The action that has been performed.
     */
    function logAction($node, $action)
    {
      $this->log(2,"Performing $node.$action");
    }

    /**
     * Write a logentry in the logfile.
     * The entry is only written to the file, if the level of the message is
     * equal or higher than the setting of $config_logging.
     *
     * @todo Logging should be moved to a separate atkLogger class.
     * @param int $level The loglevel.
     * @param String $message The message to log.
     */
    function log($level, $message)
    {

      if (atkconfig("logging")>0 && atkconfig("logging")>=$level)
      {
        $fp = @fopen(atkconfig("logfile"),"a");
        if ($fp)
        {
          $logstamp = "[".date("d-m-Y H:i:s")."] [".$_SERVER["REMOTE_ADDR"]."] ".$this->m_user["name"]." | ";
          @fwrite($fp, $logstamp.$message."\n");
          @fclose($fp);
        }
        else
        {
          atkdebug("error opening logfile");
        }
      }
    }

    function _getAuthCookieName()
    {
      return "atkauth_3".str_replace(" ","_",atktext("app_title"));
    }
  }

  /**
   * Check if the currently logged-in user has a certain privilege.
   *
   * @deprecated Use the allowed method of the security manager instead.
   * @param String $node The full name of the node for which to check access.
   * @param String $privilege The privilege to check.
   * @return boolean True if access is granted, false if not.
   */
  function is_allowed($node, $privilege)
  {
    $secMgr = &atkGetSecurityManager();
    if (!is_object($secMgr)) return 1;
    else return $secMgr->allowed($node, $privilege);
  }

?>
