<?php namespace Sintattica\Atk\Security;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Module;
use Sintattica\Atk\Security\Auth\AuthInterface;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\Output;
use Sintattica\Atk\Ui\Ui;
use Sintattica\Atk\Utils\Debugger;
use Sintattica\Atk\Ui\Page;

/**
 * The security manager for ATK applications.
 *
 * Don't instantiate this class yourself, use the global instance
 * that is returned by SecurityManager::getInstance() instead.
 *
 * @todo Make a real singleton with a getInstance class.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage security
 */
class SecurityManager
{
    const AUTH_UNVERIFIED = 0; // initial value.
    const AUTH_SUCCESS = 1;
    const AUTH_LOCKED = 2;
    const AUTH_MISMATCH = 3;
    const AUTH_PASSWORDSENT = 4;
    const AUTH_MISSINGUSERNAME = 5;
    const AUTH_ERROR = -1;


    var $m_authentication = array();
    var $m_authorization = 0;
    var $m_scheme = "none";
    var $m_user = array();
    var $m_listeners = array();
    // If login really fails (no relogin box, but an errormessage), the
    // error message that caused the fatal error is put in this variable.
    var $m_fatalError = "";

    /**
     * Can we use password retrieving/recreating in current configuration
     */
    protected $m_enablepasswordmailer = false;


    static public function &getInstance()
    {
        global $g_securityManager;
        if (!is_object($g_securityManager)) {
            // The one and only security manager.
            $authentication = Config::getGlobal("authentication", "none");
            $authorization = Config::getGlobal("authorization", $authentication);
            $scheme = Config::getGlobal("securityscheme", "none");
            $g_securityManager = new SecurityManager($authentication, $authorization, $scheme);
        }
        return $g_securityManager;
    }

    /**
     * Register a new listener.
     *
     * @param SecurityListener $listener
     */
    function addListener(&$listener)
    {
        $this->m_listeners[] = $listener;
    }

    /**
     * Notify listeners of a certain event.
     *
     * @param string $event name
     * @param string $username (might be null)
     */
    function notifyListeners($event, $username)
    {
        for ($i = 0, $_i = count($this->m_listeners); $i < $_i; $i++) {
            $this->m_listeners[$i]->handleEvent($event, $username);
        }
    }

    /**
     * returns the full classname for use with Tools::atkimport(
     *
     * @param string $type
     * @return string full classname
     */
    function _getclassname($type)
    {
        // assume that when a type includes a dot, the fullclassname is used.
        if (!stristr($type, ".")) {
            $cls = __NAMESPACE__ . "\\Auth\\" . ucfirst($type) . "Auth";
        } else {
            $cls = $type;
        }
        return $cls;
    }

    /**
     * returns an array of authentication types
     * authentication_type is a comma delimited string with
     * native atk auth types like 'Db' or 'None' or it can be a
     * full classname like module.mymodule.myauthtype
     *
     * @param string $authentication_type
     * @return array authentication types
     */
    function _getAuthTypes($authentication_type)
    {
        $authentication = explode(",", trim($authentication_type));
        $types = array();
        if (!is_array($authentication)) {
            array_push($types, $this->_getclassname(trim($authentication)));
        } else {
            foreach ($authentication as $type) {
                array_push($types, $this->_getclassname(trim($type)));
            }
        }
        return $types;
    }

    /**
     * Constructor
     * @access private
     *
     * @param string $authentication_type The type of authentication (user/password verification) to use
     * @param string $authorization_type The type of authorization (mostly the same as the authentication_type)
     * @param string $securityscheme The security scheme that will be used to determine who is allowed to do what
     */
    function __construct($authentication_type = "none", $authorization_type = "none", $securityscheme = "none")
    {
        Tools::atkdebug("creating securityManager (authenticationtype: $authentication_type, authorizationtype: $authorization_type, scheme: $securityscheme)");

        $authentication = $this->_getAuthTypes($authentication_type);
        foreach ($authentication as $class) {
            if (!class_exists($class)) {
                Tools::atkdebug("atkSecurityManager() unsupported authentication type or type no found for $class");
            } else {
                $this->m_authentication[$class] = new $class();
            }
        }

        /* authorization class */
        $clsname = $this->_getclassname($authorization_type);
        $this->m_authorization = new $clsname();

        /* security scheme */
        $this->m_scheme = $securityscheme;

        /**
         * Check, can we use password retrieving/recreating.
         * We take into account configuration var auth_enablepasswordmailer
         * and authentication methid, that we use
         *
         */
        if (Config::getGlobal("auth_enablepasswordmailer", false)) {
            foreach ($this->m_authentication as $auth) {
                if (in_array($auth->getPasswordPolicy(),
                    array(AuthInterface::PASSWORD_RETRIEVABLE, AuthInterface::PASSWORD_RECREATE))) {
                    $this->m_enablepasswordmailer = true;
                }
            }
        }
    }

    /**
     * Regenerates a user password and sends it to his e-mail address
     *
     * @param string $username User for which the password should be regenerated
     */
    function mailPassword($username)
    {
        // Query the database for user records having the given username and return if not found
        $usernode = Module::atkGetNode(Config::getGlobal("auth_usernode"));
        $selector = sprintf("%s.%s = '%s'", Config::getGlobal("auth_usertable"),
            Config::getGlobal("auth_userfield"), $username);
        $userrecords = $usernode->selectDb($selector, "", "", "", array(
            Config::getGlobal("auth_userpk"),
            Config::getGlobal("auth_emailfield"),
            Config::getGlobal("auth_passwordfield")
        ), "edit");
        if (count($userrecords) != 1) {
            Tools::atkdebug("User '$username' not found.");
            return false;
        }

        // Retrieve the email address
        $email = $userrecords[0][Config::getGlobal("auth_emailfield")];
        if (empty($email)) {
            Tools::atkdebug("Email address for '$username' not available.");
            return false;
        }

        // Regenerate the password
        $passwordattr = $usernode->getAttribute(Config::getGlobal("auth_passwordfield"));
        $newpassword = $passwordattr->generatePassword();

        // Update the record in the database
        $userrecords[0][Config::getGlobal("auth_passwordfield")]["hash"] = md5($newpassword);
        $usernode->updateDb($userrecords[0], true, "", array(Config::getGlobal("auth_passwordfield")));

        $db = &$usernode->getDB();
        $db->commit();

        // Send an email containing the new password to user
        $subject = Tools::atktext("auth_passwordmail_subjectnew_password", "atk");
        $body = Tools::atktext("auth_passwordmail_explanation", "atk") . "\n\n";
        $body .= Tools::atktext(Config::getGlobal("auth_userfield")) . ": " . $username . "\n";
        $body .= Tools::atktext(Config::getGlobal("auth_passwordfield")) . ": " . $newpassword . "\n";
        //TODO: replace with some mailer object
        mail($email, $subject, $body);

        // Return true
        return true;
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
        $session = &SessionManager::getSession();

        $response = SecurityManager::AUTH_UNVERIFIED;

        if (Config::getGlobal("auth_loginform") == true) { // form login
            $auth_user = isset($ATK_VARS["auth_user"]) ? $ATK_VARS["auth_user"] : null;
            $auth_pw = isset($ATK_VARS["auth_pw"]) ? $ATK_VARS["auth_pw"] : null;
        } else { // HTTP login
            $auth_user = isset($_SERVER["PHP_AUTH_USER"]) ? $_SERVER["PHP_AUTH_USER"]
                : null;
            $auth_pw = isset($_SERVER["PHP_AUTH_PW"]) ? $_SERVER["PHP_AUTH_PW"] : null;
        }

        // throw post login event?
        $throwPostLoginEvent = false;

        $md5 = false; // PHP_SecurityManager::AUTH_PW is plain text..
        // first check if we want to logout
        if (isset($ATK_VARS["atklogout"]) && (!isset($session["relogin"]) || $session["relogin"] != 1)) {
            $this->notifyListeners("preLogout", $auth_user);
            $currentUser = SecurityManager::atkGetUser();

            // Let the authentication plugin know about logout too.
            foreach ($this->m_authentication as $auth) {
                $auth->logout($currentUser);
            }
            $session = array();
            $session["relogin"] = 1;

            // destroy cookie
            if (Config::getGlobal("authentication_cookie") && $auth_user != "administrator") {
                $cookiename = $this->_getAuthCookieName();
                if (!empty($_COOKIE[$cookiename])) {
                    setcookie($cookiename, "", 0);
                }
            }

            $this->notifyListeners("postLogout", $auth_user);

            if ($ATK_VARS["atklogout"] > 1) {
                header("Location: logout.php");
                exit;
            }
        } // do we need to login?
        else {
            if ((!isset($session["login"])) || ($session["login"] != 1)) {
                // authenticated?
                $authenticated = false;

                // sometimes we manually have to set the PHP_AUTH vars
                // old style http_authorization
                if (empty($auth_user) && empty($auth_pw) && array_key_exists("HTTP_AUTHORIZATION",
                        $_SERVER) && ereg("^Basic ", $_SERVER["HTTP_AUTHORIZATION"])
                ) {
                    list($auth_user, $auth_pw) = explode(":", base64_decode(substr($_SERVER["HTTP_AUTHORIZATION"], 6)));
                } // external authentication
                elseif (empty($auth_user) && empty($auth_pw) && !empty($_SERVER["PHP_AUTH_USER"])) {
                    $auth_user = $_SERVER["PHP_AUTH_USER"];
                    $auth_pw = $_SERVER["PHP_AUTH_PW"];
                }

                // check previous sessions..
                if (Config::getGlobal("authentication_cookie")) {
                    // Cookiename is based on the app_title, for there may be more than 1 atk app running,
                    // each with their own cookie..
                    $cookiename = $this->_getAuthCookieName();
                    list($enc, $user, $passwd) = explode(".",
                        base64_decode(Tools::atkArrayNvl($_COOKIE, $cookiename, "Li4=")));

                    // for security reasons administrator will never be cookied..
                    if ($auth_user == "" && $user != "" && $user != "administrator") {
                        Tools::atkdebug("Using cookie to retrieve previously used userid/password");
                        $auth_user = $user;
                        $auth_pw = $passwd;
                        $md5 = ($enc == "MD5"); // cookie may already be md5;
                    }
                }

                $authenticated = false;

                // Check if a username was entered
                if ((Tools::atkArrayNvl($ATK_VARS, "login",
                            "") != "") && empty($auth_user) && !strstr(Config::getGlobal("authentication"), "none")
                ) {
                    $response = SecurityManager::AUTH_MISSINGUSERNAME;
                } // Email password if password forgotten and passwordmailer enabled
                else {
                    if ((!empty($auth_user)) && (Config::getGlobal("auth_loginform") == true) && $this->get_enablepasswordmailer() && (Tools::atkArrayNvl($ATK_VARS,
                                "login", "") == Tools::atktext('password_forgotten'))
                    ) {
                        $this->mailPassword($auth_user);
                        $response = SecurityManager::AUTH_PASSWORDSENT;
                    } else {
                        $throwPostLoginEvent = true;
                        $this->notifyListeners("preLogin", $auth_user);

                        // check administrator and guest user
                        if ($auth_user == "administrator" || $auth_user == "guest") {
                            $config_pw = Config::getGlobal($auth_user . "password");
                            if (!empty($config_pw) && (($auth_pw == $config_pw) || (Config::getGlobal("authentication_md5") && (md5($auth_pw) == strtolower($config_pw))))) {
                                $authenticated = true;
                                $response = SecurityManager::AUTH_SUCCESS;
                                if ($auth_user == "administrator") {
                                    $this->m_user = array(
                                        "name" => "administrator",
                                        "level" => -1,
                                        "access_level" => 9999999
                                    );
                                } else {
                                    $this->m_user = array("name" => "guest", "level" => -2, "access_level" => 0);
                                }
                            } else {
                                $response = SecurityManager::AUTH_MISMATCH;
                            }
                        }

                        // other users
                        // we must first explicitly check that we are not trying to login as administrator or guest.
                        // these accounts have been validated above. If we don't check this, an account could be
                        // created in the database that provides administrator access.
                        else {
                            if ($auth_user != "administrator" && $auth_user != "guest") {
                                if (is_array($this->m_authentication)) {
                                    // We have a username, which we must now validate against several
                                    // checks. If all of these fail, we have a status of SecurityManager::AUTH_MISMATCH.
                                    foreach ($this->m_authentication as $name => $obj) {
                                        $obj->canMd5() && !$md5 ? $tmp_pw = md5($auth_pw) : $tmp_pw = $auth_pw;
                                        $response = $obj->validateUser($auth_user, $tmp_pw);
                                        if ($response == SecurityManager::AUTH_SUCCESS) {
                                            Tools::atkdebug("SecurityManager::authenticate() using $name authentication");
                                            $authname = $name;
                                            break;
                                        }
                                    }
                                }
                                if ($response == SecurityManager::AUTH_SUCCESS) { // succesful login
                                    // We store the username + securitylevel of the logged in user.
                                    $this->m_user = $this->m_authorization->getUser($auth_user);
                                    $this->m_user['AUTH'] = $authname; // something to see wich auth scheme is used
                                    if (Config::getGlobal("enable_ssl_encryption")) {
                                        $this->m_user['PASS'] = $auth_pw;
                                    } // used by aktsecurerelation to decrypt an linkpass


// for convenience, we also store the user as a global variable.
                                    (is_array($this->m_user['level'])) ? $dbg = implode(",", $this->m_user['level'])
                                        : $dbg = $this->m_user['level'];
                                    Tools::atkdebug("Logged in user: " . $this->m_user["name"] . " (level: " . $dbg . ")");
                                    $authenticated = true;

                                    // Remember that we are logged in..
                                    //$g_sessionManager->globalVar("authentication",array("authenticated"=>1, "user"=>$this->m_user), true);
                                    // write cookie
                                    if (Config::getGlobal("authentication_cookie") && $auth_user != "administrator") {
                                        // if the authentication scheme supports md5 passwords, we can safely store
                                        // the password as md5 in the cookie.
                                        if ($md5) { // Password is already md5 encoded
                                            $tmppw = $auth_pw;
                                            $enc = "MD5";
                                        } else { // password is not md5 encoded
                                            if ($this->m_authentication[$authname]->canMd5()) { // we can encode it
                                                $tmppw = md5($auth_pw);
                                                $enc = "MD5";
                                            } else { // authentication scheme does not support md5 encoding.
                                                // our only choice is to store the password plain text
                                                // :-(
                                                // NOTE: If you use a non-md5 enabled authentication
                                                // scheme then, for security reasons, you shouldn't use
                                                // $config_authentication_cookie at all.
                                                $tmppw = $auth_pw;
                                                $enc = "PLAIN";
                                            }
                                        }
                                        setcookie($cookiename, base64_encode($enc . "." . $auth_user . "." . $tmppw),
                                            time() + 60 * (Config::getGlobal("authentication_cookie_expire")));
                                    }
                                } else {
                                    // login was incorrect. Either the supplied username/password combination is
                                    // incorrect (we just try again) or there was an error (we display an error
                                    // message)
                                    if ($response == SecurityManager::AUTH_ERROR) {
                                        $this->m_fatalError = $this->m_authentication->m_fatalError;
                                    }
                                    $authenticated = false;
                                }
                            }
                        }

                        // we are logged in
                        if ($authenticated) {
                            $session["login"] = 1;
                        }
                    }
                }
            } else {
                // using session for authentication, because "login" was registered.
                // but we double check with some more data from the session to see
                // if the login is really valid.
                $session_auth = $g_sessionManager->getValue("authentication", "globals");

                if (Config::getGlobal("authentication_session") &&
                    $session["login"] == 1 &&
                    $session_auth["authenticated"] == 1 &&
                    !empty($session_auth["user"])
                ) {
                    $this->m_user = $session_auth["user"];
                    Tools::atkdebug("Using session for authentication / user = " . $this->m_user["name"]);
                } else {
                    // Invalid session
                    $authenticated = false;
                }
            }
        }

        // if there was an error, drop out.
        if ($this->m_fatalError != "") {
            return false;
        }
        // still not logged in?!
        if (!isset($session["login"]) || $session["login"] != 1) {
            $location = Config::getGlobal('auth_loginpage', '');
            if ($location) {
                $location .= (strpos($location, '?') === false) ? '?' : '&';
                $location .= 'login=' . $auth_user . '&error=' . $response;

                if (Config::getGlobal("debug") >= 2) {
                    $debugger = Debugger::getInstance();
                    $debugger->setRedirectUrl($location);
                    Tools::atkdebug('Non-debug version would have redirected to <a href="' . $location . '">' . $location . '</a>');
                    $output = Output::getInstance();
                    $output->outputFlush();
                    exit();
                } else {
                    header('Location: ' . $location);
                    exit();
                }
            } elseif (Config::getGlobal("auth_loginform")) {
                $this->loginForm($auth_user, $response);
                $output = Output::getInstance();
                $output->outputFlush();
                exit();
            } else {
                header('WWW-Authenticate: Basic realm="' . Tools::atktext("app_title") . (Config::getGlobal("auth_changerealm",
                        true)
                        ? ' - ' . strftime("%c", time()) : "") . '"');
                if (ereg("Microsoft", $_SERVER['SERVER_SOFTWARE'])) {
                    header("Status: 401 Unauthorized");
                } else {
                    header("HTTP/1.0 401 Unauthorized");
                }
                return false;
            }
        } // we are authenticated, but atklogout is still active, let's get rid of it!
        else {
            if (isset($ATK_VARS["atklogout"])) {
                header("Location: " . Tools::atkSelf() . "?");
            } // we keep the relogin state until the atklogout variable isn't set anymore
            else {
                if (!isset($ATK_VARS["atklogout"]) && isset($session["relogin"]) && $session["relogin"] == 1) {
                    $session["relogin"] = 0;
                }
            }
        }
        // return
        // g_user always lowercase
        $this->m_user["name"] = $this->m_user["name"];
        //Send the username with the header
        //This way we can always retrieve the user from apache logs
        header('user: ' . $this->m_user["name"]);
        $GLOBALS["g_user"] = &$this->m_user;
        $sm = SessionManager::getInstance();
        $sm->globalVar("authentication", array("authenticated" => 1, "user" => $this->m_user), true);
        $sm->globalVar('loginattempts', '');

        if ($throwPostLoginEvent) {
            $this->notifyListeners("postLogin", $auth_user);
        }

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
        $user = SecurityManager::atkGetUser();
        $this->m_user = $this->m_authorization->getUser($user[Config::getGlobal('auth_userfield')]);
        $g_user = $this->m_user;
        $old_auth = $g_sessionManager->getValue("authentication", "globals");
        $old_auth["user"] = $g_user;
        $g_sessionManager->globalVar("authentication", $old_auth, true);
    }

    /**
     * Display a login form.
     * @access private
     *
     * @param string $defaultname The username that might already be known
     * @param int $lastresponse The lastresponse when trying to login
     *                              possible values: SecurityManager::AUTH_MISMATCH, SecurityManager::AUTH_LOCKED, SecurityManager::AUTH_MISSINGUSERNAME, SecurityManager::AUTH_PASSWORDSENT
     */
    function loginForm($defaultname, $lastresponse)
    {
        $sm = SessionManager::getInstance();

        $loginattempts = $sm->getValue('loginattempts'); // Note: not actually how many authentication attempts, but how many times the login form has been displayed.
        if ($loginattempts == "") {
            $loginattempts = 0;
        }

        if ($loginattempts == "") {
            $loginattempts = 1;
        } else {
            $loginattempts++;
        }

        $sm->globalVar('loginattempts', $loginattempts);
        Tools::atkdebug('LoginAttempts: ' . $loginattempts);

        $page = Page::getInstance();
        $ui = Ui::getInstance();

        $page->register_style($ui->stylePath("style.css"));
        $page->register_style($ui->stylePath("login.css"));

        $page->register_script(Config::getGlobal('atkroot') . "atk/javascript/tools.js");

        $tplvars = Array();
        $output = '<form action="' . Tools::atkSelf() . '" method="post">';
        $output .= Tools::makeHiddenPostVars(array("atklogout", "loginattempts"));
        $output .= '<br><br><table border="0" cellspacing="2" cellpadding="0" align="center">';

        $tplvars["atksessionformvars"] = Tools::makeHiddenPostVars(array("atklogout", "loginattempts"));
        $tplvars["formurl"] = Tools::atkSelf();

        // max_loginattempts of 0 means no maximum.
        if (Config::getGlobal('max_loginattempts') > 0 && $loginattempts > Config::getGlobal('max_loginattempts')) {
            $output .= "<tr><td class=table>" . Tools::atktext('auth_max_loginattempts_exceeded') . "<br><br></td></tr>";
            $tplvars["auth_max_loginattempts_exceeded"] = Tools::atktext('auth_max_loginattempts_exceeded');
        } else {
            $page->register_script(Config::getGlobal('atkroot') . "atk/javascript/formfocus.js");
            $page->register_loadscript("placeFocus(false);");

            // generate the username input field
            // based upon the config_authdropdown and auth. method
            $userField = $this->auth_userField($defaultname);
            $tplvars["username"] = Tools::atktext("username");
            $tplvars["password"] = Tools::atktext("password");
            $tplvars["userfield"] = $userField;
            $tplvars["passwordfield"] = '<input class="loginform" type="password" size="15" name="auth_pw" value="" />';
            $tplvars["submitbutton"] = '<input name="login" class="button" type="submit" value="' . Tools::atktext('login') . '" />';
            $tplvars["title"] = Tools::atktext('login_form');

            if ($lastresponse == SecurityManager::AUTH_LOCKED) {
                $output .= "<tr><td colspan=3 class=error>" . Tools::atktext('auth_account_locked') . "<br><br></td></tr>";
                $tplvars["auth_account_locked"] = Tools::atktext('auth_account_locked');
                $tplvars["error"] = Tools::atktext('auth_account_locked');
            } else {
                if ($lastresponse == SecurityManager::AUTH_MISMATCH) {
                    $output .= '<tr><td colspan=3 class=error>' . Tools::atktext('auth_mismatch') . '<br><br></td></tr>';
                    $tplvars["auth_mismatch"] = Tools::atktext('auth_mismatch');
                    $tplvars["error"] = Tools::atktext('auth_mismatch');
                } else {
                    if ($lastresponse == SecurityManager::AUTH_MISSINGUSERNAME) {
                        $output .= '<tr><td colspan="3" class=error>' . Tools::atktext('auth_missingusername') . '<br /><br /></td></tr>';
                        $tplvars["auth_mismatch"] = Tools::atktext('auth_missingusername');
                        $tplvars["error"] = Tools::atktext('auth_missingusername');
                    } else {
                        if ($lastresponse == SecurityManager::AUTH_PASSWORDSENT) {
                            $output .= '<tr><td colspan="3">' . Tools::atktext('auth_passwordmail_sent') . '<br /><br /></td></tr>';
                            $tplvars["auth_mismatch"] = Tools::atktext('auth_passwordmail_sent');
                        }
                    }
                }
            }

            // generate the form
            $output .= "<tr><td valign=top>" . Tools::atktext('username') . "</td><td>:</td><td>" . $userField . "</td></tr>";
            $output .= "<tr><td colspan=3 height=6></td></tr>";
            $output .= "<tr><td valign=top>" . Tools::atktext('password') . "</td><td>:</td><td><input type=password size=15 name=auth_pw value='' /></td></tr>";
            $output .= '<tr><td colspan="3" align="center" height="50" valign="middle">';
            $output .= '<input name="login" class="button" type="submit" value="' . Tools::atktext('login') . '">';
            $tplvars["auth_enablepasswordmailer"] = $this->get_enablepasswordmailer();
            if ($this->get_enablepasswordmailer()) {
                $output .= '&nbsp;&nbsp;<input name="login" class="button" type="submit" value="' . Tools::atktext('password_forgotten') . '">';
                $tplvars["forgotpasswordbutton"] = '<input name="login" class="button" type="submit" value="' . Tools::atktext('password_forgotten') . '">';
            }
            $output .= '</td></tr>';
        }

        $output .= '</table></form>';

        $tplvars["content"] = $output;
        $page->addContent($ui->render("login.tpl", $tplvars));
        $o = Output::getInstance();
        $o->output($page->render(Tools::atktext("app_title"), Page::HTML_STRICT, "", $ui->render("login_meta.tpl")));
    }

    /**
     * Generate field for entering the username (dropdown or input box,
     * depending on settings.
     * @access private
     *
     * @param string $defaultname The username that might already be known
     */
    function auth_userField($defaultname)
    {
        if (Config::getGlobal("auth_dropdown") == true) {
            $auth_types = $this->m_authentication;
            $userlist = array();

            // Administrator and guest user may be present.
            if (Config::getGlobal("administratorpassword") != "") {
                $userlist[] = array("userid" => "administrator", "username" => "Administrator");
            }
            if (Config::getGlobal("guestpassword") != "") {
                $userlist[] = array("userid" => "guest", "username" => "Guest");
            }

            foreach ($auth_types as $type => $obj) {
                $userlist = array_merge($userlist, $obj->getUserList());
            }

            $userField = '<select id="auth_user" name="auth_user" class="form-control">' . "\n";

            for ($i = 0, $_i = count($userlist); $i < $_i; $i++) {
                $selected = "";
                if (trim(strtolower($defaultname)) == strtolower(trim($userlist[$i]["userid"]))) {
                    $selected = " selected";
                }
                $userField .= "<option value='" . $userlist[$i]["userid"] . "'" . $selected . ">" . $userlist[$i]["username"] . "</option>\n";
            }
            $userField .= "</select>\n";

            return $userField;
        } else {
            return '<input class="form-control loginform" type="text" size="15" id="auth_user" name="auth_user" value="' . htmlentities($defaultname) . '" />';
        }
    }

    /**
     * Check if the currently logged-in user has a certain privilege on a
     * node.
     * @param String $node The full nodename of the node for which to check
     *                     access privileges. (modulename.nodename notation).
     * @param String $privilege The privilege to check (atkaction).
     * @return boolean True if the user has the privilege, false if not.
     */
    function allowed($node, $privilege)
    {
        static $_cache = array();

        if (!isset($_cache[$node][$privilege])) {
            // ask authorization instance
            $allowed = $this->m_authorization->allowed($this, $node, $privilege);
            $_cache[$node][$privilege] = $allowed;
        }
        return $_cache[$node][$privilege];
    }

    /**
     * Check if the currently logged-in user has the right to view, edit etc.
     * an attribute of a node.
     *
     * @param Attribute $attr attribute reference
     * @param string $mode mode (add, edit, view etc.)
     * @param array $record record data
     *
     * @return boolean true if access is granted, false if not.
     */
    function attribAllowed(&$attr, $mode, $record = null)
    {
        return $this->m_authorization->attribAllowed($this, $attr, $mode, $record);
    }

    /**
     * Check if the currently logged in user has the requested level.
     * @param int $level The level to check.
     * @return boolean True if the user has the required level, false if not.
     */
    function hasLevel($level)
    {
        if (is_array($level)) {
            if (is_array($this->m_user["level"])) {
                return (count(array_intersect($this->m_user["level"], $level)) >= 1);
            } else {
                return in_array($this->m_user["level"], $level);
            }
        } else {
            if (is_array($this->m_user["level"])) {
                return in_array($level, $this->m_user["level"]);
            } else {
                return $this->m_user["level"] == $level;
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
        $this->log(2, "Performing $node.$action");
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

        if (Config::getGlobal("logging") > 0 && Config::getGlobal("logging") >= $level) {
            $fp = @fopen(Config::getGlobal("logfile"), "a");
            if ($fp) {
                $logstamp = "[" . date("d-m-Y H:i:s") . "] [" . $_SERVER["REMOTE_ADDR"] . "] " . $this->m_user["name"] . " | ";
                @fwrite($fp, $logstamp . $message . "\n");
                @fclose($fp);
            } else {
                Tools::atkdebug("error opening logfile");
            }
        }
    }

    /**
     * If we are using cookies to store the login information this function will generate
     * the cookiename
     *
     * @return string cookiename based on the application title
     */
    function _getAuthCookieName()
    {
        return "atkauth_3" . str_replace(" ", "_", Tools::atktext("app_title"));
    }

    /**
     * Getter for m_enablepasswordmailer
     *
     * @return bool
     */
    public function get_enablepasswordmailer()
    {
        return $this->m_enablepasswordmailer;
    }

    /**
     * Calling this function will invoke the login process. Call this function in
     * every file that you want to have secured.
     * (This is actually a small wrapper for $securityManager->authenticate(),
     * so you can quickly secure an application.
     */
    public static function atksecure()
    {
        $securityMgr = self::getInstance();

        if (!$securityMgr->authenticate()) {
            echo '<b>' . Tools::atktext("login_failed", "atk") . '</b>';
            echo '<br><br>' . $securityMgr->m_fatalError;
            exit;
        }
    }


    /**
     * Retrieve all known information about the currently logged-in user.
     * @param $key string
     * @return array Array with userinfo, or "" if no user is logged in.
     */
    public static function atkGetUser($key = '')
    {
        $sessionmanager = SessionManager::getSessionManager();
        $session = SessionManager::getSession();
        $user = "";
        $session_auth = is_object($sessionmanager) ? $sessionmanager->getValue("authentication", "globals")
            : array();
        if (Config::getGlobal("authentication_session") &&
            Tools::atkArrayNvl($session, "login", 0) == 1 &&
            $session_auth["authenticated"] == 1 &&
            !empty($session_auth["user"])
        ) {
            $user = $session_auth["user"];
            if (!isset($user["access_level"]) || empty($user["access_level"])) {
                $user["access_level"] = 0;
            }
        }

        if ($key) {
            return $user[$key];
        }
        return $user;
    }

    /**
     * Retrieve id of the currently logged-in user.
     * @return int user id or 0 if not logged in or administrator
     */
    public static function atkGetUserId()
    {
        $user = self::atkGetUser();
        $userpk = Config::getGlobal('auth_userpk');

        // check if logged in || logged in as administrator
        if ($user == "" || $userpk == "" ||
            (is_array($user) && !isset($user[$userpk]))
        ) {
            return 0;
        }

        return $user[$userpk];
    }
}
