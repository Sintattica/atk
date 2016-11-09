<?php

namespace Sintattica\Atk\Security;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Security\Auth\AuthInterface;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\Output;
use Sintattica\Atk\Ui\Ui;
use Sintattica\Atk\Utils\Debugger;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Db\Db;

/**
 * The security manager for ATK applications.
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

    public $m_authentication = [];
    public $m_authorization = 0;
    public $m_scheme = 'none';
    public $m_user = [];
    public $m_listeners = [];
    // If login really fails (no relogin box, but an errormessage), the
    // error message that caused the fatal error is put in this variable.
    public $m_fatalError = '';

    /**
     * Can we use password retrieving/recreating in current configuration.
     */
    protected $m_enablepasswordmailer = false;

    public static function getInstance()
    {
        static $s_instance = null;
        if ($s_instance == null) {
            Tools::atkdebug('Created a new SecurityManager instance');
            $authentication = Config::getGlobal('authentication', 'none');
            $authorization = Config::getGlobal('authorization', $authentication);
            $scheme = Config::getGlobal('securityscheme', 'none');
            $s_instance = new self($authentication, $authorization, $scheme);
        }

        return $s_instance;
    }

    /**
     * Register a new listener.
     *
     * @param SecurityListener $listener
     */
    public function addListener($listener)
    {
        $this->m_listeners[] = $listener;
    }

    /**
     * Notify listeners of a certain event.
     *
     * @param string $event name
     * @param string $username (might be null)
     */
    public function notifyListeners($event, $username)
    {
        for ($i = 0, $_i = count($this->m_listeners); $i < $_i; ++$i) {
            $this->m_listeners[$i]->handleEvent($event, $username);
        }
    }

    /**
     * @param string $type auth type (db, imap, config, ...) or full classname
     *
     * @return string full classname
     */
    public function _getclassname($type)
    {
        // assume that when a type includes a backslash, the fullclassname is used.
        if (!stristr($type, '\\')) {
            $cls = __NAMESPACE__.'\\Auth\\'.ucfirst(strtolower($type)).'Auth';
        } else {
            $cls = $type;
        }

        return $cls;
    }

    /**
     * returns an array of authentication types
     * authentication_type is a comma delimited string with
     * native atk auth types like 'Db' or 'None' or it can be a
     * full classname like module.mymodule.myauthtype.
     *
     * @param string $authentication_type
     *
     * @return array authentication types
     */
    public function _getAuthTypes($authentication_type)
    {
        $authentication = explode(',', trim($authentication_type));
        $types = [];
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
     * Constructor.
     *
     * @param string $authentication_type The type of authentication (user/password verification) to use
     * @param string $authorization_type The type of authorization (mostly the same as the authentication_type)
     * @param string $securityscheme The security scheme that will be used to determine who is allowed to do what
     */
    public function __construct($authentication_type = 'none', $authorization_type = 'none', $securityscheme = 'none')
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

        /*
         * Check, can we use password retrieving/recreating.
         * We take into account configuration var auth_enablepasswordmailer
         * and authentication methid, that we use
         *
         */
        if (Config::getGlobal('auth_enablepasswordmailer', false)) {
            foreach ($this->m_authentication as $class => $auth) {
                if (in_array($auth->getPasswordPolicy(), array(AuthInterface::PASSWORD_RETRIEVABLE, AuthInterface::PASSWORD_RECREATE))) {
                    $this->m_enablepasswordmailer = true;
                }
            }
        }
    }

    /**
     * Regenerates a user password and sends it to his e-mail address.
     *
     * @param string $username User for which the password should be regenerated
     *
     * @return bool
     */
    public function mailPassword($username)
    {
        $atk = Atk::getInstance();
        // Query the database for user records having the given username and return if not found
        $userNode = $atk->atkGetNode(Config::getGlobal('auth_usernode'));
        $selector = sprintf("%s.%s = '%s'", Config::getGlobal('auth_usertable'), Config::getGlobal('auth_userfield'), $username);

        $userrecords = $userNode->select($selector)->includes(array(
            Config::getGlobal('auth_userpk'),
            Config::getGlobal('auth_emailfield'),
            Config::getGlobal('auth_passwordfield'),
        ))->mode('edit')->getAllRows();

        if (count($userrecords) != 1) {
            Tools::atkdebug("User '$username' not found.");

            return false;
        }

        // Retrieve the email address
        $email = $userrecords[0][Config::getGlobal('auth_emailfield')];
        if (empty($email)) {
            Tools::atkdebug("Email address for '$username' not available.");

            return false;
        }

        // Regenerate the password
        $passwordAttr = $userNode->getAttribute(Config::getGlobal('auth_passwordfield'));
        $newPassword = $passwordAttr->generatePassword();

        // Update the record in the database
        $userrecords[0][Config::getGlobal('auth_passwordfield')]['hash'] = password_hash($newPassword, PASSWORD_DEFAULT);
        $userNode->updateDb($userrecords[0], true, '', array(Config::getGlobal('auth_passwordfield')));

        $db = $userNode->getDb();
        $db->commit();

        // Send an email containing the new password to user
        $subject = Tools::atktext('auth_passwordmail_subjectnew_password', 'atk');
        $body = Tools::atktext('auth_passwordmail_explanation', 'atk')."\n\n";
        $body .= Tools::atktext(Config::getGlobal('auth_userfield')).': '.$username."\n";
        $body .= Tools::atktext(Config::getGlobal('auth_passwordfield')).': '.$newPassword."\n";

        //TODO: replace with some mailer object
        mail($email, $subject, $body);

        // Return true
        return true;
    }

    /**
     * @param $password string
     * @param $hash string
     * @return bool
     */
    public function verify($password, $hash)
    {
        if (!Config::getGlobal('auth_usecryptedpassword')) {
            return $password == $hash;
        }

        return password_verify($password, $hash);
    }

    /**
     * Perform user authentication.
     */
    public function authenticate()
    {
        global $ATK_VARS;
        $session = &SessionManager::getSession();
        $sessionManager = SessionManager::getInstance();

        $response = self::AUTH_UNVERIFIED;

        if (Config::getGlobal('auth_loginform') == true) { // form login
            $auth_user = isset($ATK_VARS['auth_user']) ? $ATK_VARS['auth_user'] : null;
            $auth_pw = isset($ATK_VARS['auth_pw']) ? $ATK_VARS['auth_pw'] : null;
        } else { // HTTP login
            $auth_user = isset($_SERVER['PHP_AUTH_USER']) ? $_SERVER['PHP_AUTH_USER'] : null;
            $auth_pw = isset($_SERVER['PHP_AUTH_PW']) ? $_SERVER['PHP_AUTH_PW'] : null;
        }

        // throw post login event?
        $throwPostLoginEvent = false;

        // first check if we want to logout
        if (isset($ATK_VARS['atklogout']) && (!isset($session['relogin']) || $session['relogin'] != 1)) {
            $currentUser = self::atkGetUser();
            $this->notifyListeners('preLogout', isset($currentUser['name']) ? $currentUser['name'] : $auth_user);


            // Let the authentication plugin know about logout too.
            foreach ($this->m_authentication as $class => $auth) {
                $auth->logout($currentUser);
            }

            $this->notifyListeners('postLogout', isset($currentUser['name']) ? $currentUser['name'] : $auth_user);

            if (Config::getGlobal('auth_enable_rememberme')) {
                $this->rememberMeClearCookie();
                if (isset($session['remembermeTokenId'])) {
                    $this->rememberMeDeleteToken($session['remembermeTokenId']);
                }
            }

            $session = [];
            $session['relogin'] = 1;

            // do we need to login?
        } else {
            if ((!isset($session['login'])) || ($session['login'] != 1)) {


                // sometimes we manually have to set the PHP_AUTH vars
                // old style http_authorization
                if (empty($auth_user) && empty($auth_pw) && array_key_exists('HTTP_AUTHORIZATION', $_SERVER) && preg_match('/^Basic /', $_SERVER['HTTP_AUTHORIZATION'])
                ) {
                    list($auth_user, $auth_pw) = explode(':', base64_decode(substr($_SERVER['HTTP_AUTHORIZATION'], 6)));
                } // external authentication
                elseif (empty($auth_user) && empty($auth_pw) && !empty($_SERVER['PHP_AUTH_USER'])) {
                    $auth_user = $_SERVER['PHP_AUTH_USER'];
                    $auth_pw = $_SERVER['PHP_AUTH_PW'];
                }

                $authenticated = false;

                if (Config::getGlobal('auth_enable_rememberme')) {
                    $remember_user = $this->rememberMeVerifyCookie();
                    if ($remember_user) {
                        //autententhicated!
                        $throwPostLoginEvent = true;
                        $this->notifyListeners('preLogin', $remember_user);
                        $session['remembermeTokenId'] = $this->rememberMeStore($remember_user);
                        $this->storeAuth($remember_user, 'rememberme');
                        $authenticated = true;
                    }
                }


                if (!$authenticated) {
                    // Check if a username was entered
                    if ((Tools::atkArrayNvl($ATK_VARS, 'login', '') != '') && empty($auth_user) && !strstr(Config::getGlobal('authentication'), 'none')) {
                        $response = self::AUTH_MISSINGUSERNAME;
                    } // Email password if password forgotten and passwordmailer enabled
                    else {
                        if ((!empty($auth_user)) && (Config::getGlobal('auth_loginform') == true) && $this->get_enablepasswordmailer() && (Tools::atkArrayNvl($ATK_VARS,
                                    'login', '') == Tools::atktext('password_forgotten'))
                        ) {
                            $this->mailPassword($auth_user);
                            $response = self::AUTH_PASSWORDSENT;
                        } else {
                            $throwPostLoginEvent = true;
                            $this->notifyListeners('preLogin', $auth_user);

                            // check administrator and guest user
                            if ($auth_user == 'administrator' || $auth_user == 'guest') {
                                $config_pw = Config::getGlobal($auth_user.'password');

                                if (!empty($config_pw) && $this->verify($auth_pw, $config_pw)) {
                                    $authenticated = true;
                                    $response = self::AUTH_SUCCESS;
                                    if ($auth_user == 'administrator') {

                                        $this->m_user = array(
                                            'name' => 'administrator',
                                            'level' => -1,
                                            'access_level' => 9999999,
                                        );
                                    } else {
                                        $this->m_user = array('name' => 'guest', 'level' => -2, 'access_level' => 0);
                                    }
                                } else {
                                    $response = self::AUTH_MISMATCH;
                                }
                            }

                            // other users
                            // we must first explicitly check that we are not trying to login as administrator or guest.
                            // these accounts have been validated above. If we don't check this, an account could be
                            // created in the database that provides administrator access.
                            else {


                                if ($auth_user != 'administrator' && $auth_user != 'guest') {

                                    // We have a username, which we must now validate against several
                                    // checks. If all of these fail, we have a status of SecurityManager::AUTH_MISMATCH.
                                    $error = '';
                                    foreach ($this->m_authentication as $class => $obj) {
                                        $response = $obj->validateUser($auth_user, $auth_pw);
                                        if ($response == self::AUTH_SUCCESS) {
                                            Tools::atkdebug("SecurityManager::authenticate() using $class authentication");
                                            $authname = $class;
                                            break;
                                        }else{
                                            $error = $obj->m_fatalError;
                                        }
                                    }

                                    if ($response == self::AUTH_SUCCESS) { // succesful login
                                        // We store the username + securitylevel of the logged in user.
                                        $this->storeAuth($auth_user, $authname);
                                        $authenticated = true;
                                    } else {
                                        // login was incorrect. Either the supplied username/password combination is
                                        // incorrect (we just try again) or there was an error (we display an error
                                        // message)
                                        if ($response == self::AUTH_ERROR) {
                                            $this->m_fatalError = $error;
                                        }
                                    }

                                    //store remember me cookie?
                                    if ($authenticated
                                        && Config::getGlobal('auth_enable_rememberme')
                                        && isset($_REQUEST['auth_rememberme'])
                                        && $_REQUEST['auth_rememberme'] == '1'
                                    ) {
                                        $session['remembermeTokenId'] = $this->rememberMeStore($auth_user);
                                    }

                                }
                            }
                        }
                    }
                }

                // we are logged in
                if ($authenticated) {
                    $session['login'] = 1;
                }


            } else {
                // using session for authentication, because "login" was registered.
                // but we double check with some more data from the session to see
                // if the login is really valid.
                $session_auth = $sessionManager->getValue('authentication', 'globals');

                if (Config::getGlobal('authentication_session') && $session['login'] == 1 && $session_auth['authenticated'] == 1 && !empty($session_auth['user'])) {
                    $this->m_user = $session_auth['user'];
                    Tools::atkdebug('Using session for authentication / user = '.$this->m_user['name']);
                }
            }
        }


        // if there was an error, drop out.
        if ($this->m_fatalError != '') {
            return false;
        }
        // still not logged in?!
        if (!isset($session['login']) || $session['login'] != 1) {
            $location = Config::getGlobal('auth_loginpage', '');
            if ($location) {
                $location .= (strpos($location, '?') === false) ? '?' : '&';
                $location .= 'login='.$auth_user.'&error='.$response;

                if (Config::getGlobal('debug') >= 2) {
                    $debugger = Debugger::getInstance();
                    $debugger->setRedirectUrl($location);
                    Tools::atkdebug('Non-debug version would have redirected to <a href="'.$location.'">'.$location.'</a>');
                    $output = Output::getInstance();
                    $output->outputFlush();
                    exit();
                } else {
                    header('Location: '.$location);
                    exit();
                }
            } elseif (Config::getGlobal('auth_loginform')) {
                $this->loginForm($auth_user, $response);
                $output = Output::getInstance();
                $output->outputFlush();
                exit();
            } else {
                header('WWW-Authenticate: Basic realm="'.Tools::atktext('app_title').(Config::getGlobal('auth_changerealm', true) ? ' - '.strftime('%c',
                            time()) : '').'"');
                if (preg_match('/Microsoft/', $_SERVER['SERVER_SOFTWARE'])) {
                    header('Status: 401 Unauthorized');
                } else {
                    header('HTTP/1.0 401 Unauthorized');
                }

                return false;
            }
        } // we are authenticated, but atklogout is still active, let's get rid of it!
        else {
            if (isset($ATK_VARS['atklogout'])) {
                header('Location: '.Config::getGlobal('dispatcher').'?');
            } // we keep the relogin state until the atklogout variable isn't set anymore
            else {
                if (!isset($ATK_VARS['atklogout']) && isset($session['relogin']) && $session['relogin'] == 1) {
                    $session['relogin'] = 0;
                }
            }
        }
        // return
        // g_user always lowercase
        // $this->m_user["name"] = $this->m_user["name"];
        //Send the username with the header
        //This way we can always retrieve the user from apache logs
        header('user: '.$this->m_user['name']);
        $GLOBALS['g_user'] = &$this->m_user;
        $sm = SessionManager::getInstance();
        $sm->globalVar('authentication', array('authenticated' => 1, 'user' => $this->m_user), true);

        if ($throwPostLoginEvent) {
            $this->notifyListeners('postLogin', $auth_user);
        }

        return true;
    }

    protected function storeAuth($auth_user, $authname)
    {
        $this->m_user = $this->m_authorization->getUser($auth_user);
        $this->m_user['AUTH'] = $authname; // something to see which auth scheme is used
        (is_array($this->m_user['level'])) ? $dbg = implode(',', $this->m_user['level']) : $dbg = $this->m_user['level'];
        Tools::atkdebug('Logged in user: '.$this->m_user['name'].' (level: '.$dbg.')');
    }

    /**
     * Reload the current user data.
     * This method should be called if userdata, for example name or other
     * fields, have been updated for the currently logged in user.
     *
     * The method will make sure that $SsecurityManager->m_user and
     * the authenticated user in the session are refreshed.
     */
    public function reloadUser()
    {
        $sessionManager = SessionManager::getInstance();
        $user = self::atkGetUser();
        $this->m_user = $this->m_authorization->getUser($user[Config::getGlobal('auth_userfield')]);
        $old_auth = $sessionManager->getValue('authentication', 'globals');
        $old_auth['user'] = $this->m_user;
        $sessionManager->globalVar('authentication', $old_auth, true);
    }

    /**
     * Display a login form.
     *
     * @param string $defaultname The username that might already be known
     * @param int $lastresponse The lastresponse when trying to login
     *                             possible values:
     *                             SecurityManager::AUTH_MISMATCH,
     *                             SecurityManager::AUTH_LOCKED,
     *                             SecurityManager::AUTH_MISSINGUSERNAME,
     *                             SecurityManager::AUTH_PASSWORDSENT
     */
    public function loginForm($defaultname, $lastresponse)
    {
        $page = Page::getInstance();
        $ui = Ui::getInstance();

        $page->register_script(Config::getGlobal('assets_url').'javascript/tools.js');

        $tplvars = [];
        $output = '<form action="'.Config::getGlobal('dispatcher').'" method="post">';
        $output .= Tools::makeHiddenPostvars(array('atklogout'));
        $output .= '<br><br><table border="0" cellspacing="2" cellpadding="0" align="center">';

        $tplvars['atksessionformvars'] = Tools::makeHiddenPostvars(['atklogout', 'auth_rememberme']);
        $tplvars['formurl'] = Config::getGlobal('dispatcher');

        $tplvars['username'] = Tools::atktext('username');
        $tplvars['password'] = Tools::atktext('password');
        $tplvars['userfield'] = '<input class="form-control loginform" type="text" size="20" id="auth_user" name="auth_user" value="'.htmlentities($defaultname).'" />';
        $tplvars['passwordfield'] = '<input class="loginform" type="password" size="20" name="auth_pw" value="" />';
        $tplvars['submitbutton'] = '<input name="login" class="button" type="submit" value="'.Tools::atktext('login').'" />';
        $tplvars['title'] = Tools::atktext('login_form');

        if ($lastresponse == self::AUTH_LOCKED) {
            $output .= '<tr><td colspan=3 class=error>'.Tools::atktext('auth_account_locked').'<br><br></td></tr>';
            $tplvars['auth_account_locked'] = Tools::atktext('auth_account_locked');
            $tplvars['error'] = Tools::atktext('auth_account_locked');
        } elseif ($lastresponse == self::AUTH_MISMATCH) {
            $output .= '<tr><td colspan=3 class=error>'.Tools::atktext('auth_mismatch').'<br><br></td></tr>';
            $tplvars['auth_mismatch'] = Tools::atktext('auth_mismatch');
            $tplvars['error'] = Tools::atktext('auth_mismatch');
        } elseif ($lastresponse == self::AUTH_MISSINGUSERNAME) {
            $output .= '<tr><td colspan="3" class=error>'.Tools::atktext('auth_missingusername').'<br /><br /></td></tr>';
            $tplvars['auth_mismatch'] = Tools::atktext('auth_missingusername');
            $tplvars['error'] = Tools::atktext('auth_missingusername');
        } elseif ($lastresponse == self::AUTH_PASSWORDSENT) {
            $output .= '<tr><td colspan="3">'.Tools::atktext('auth_passwordmail_sent').'<br /><br /></td></tr>';
            $tplvars['auth_mismatch'] = Tools::atktext('auth_passwordmail_sent');
        }

        if (Config::getGlobal('auth_enable_rememberme')) {
            $tplvars['auth_enable_rememberme'] = true;
            if (isset($_POST['auth_rememberme']) && $_POST['auth_rememberme'] == '1') {
                $tplvars['auth_rememberme'] = true;
            }
        }

        // generate the form
        $output .= '<tr><td valign=top>'.Tools::atktext('username').'</td><td>:</td><td>'.$tplvars['userfield'].'</td></tr>';
        $output .= '<tr><td colspan=3 height=6></td></tr>';
        $output .= '<tr><td valign=top>'.Tools::atktext('password')."</td><td>:</td><td><input type=password size=15 name=auth_pw value='' /></td></tr>";
        $output .= '<tr><td colspan="3" align="center" height="50" valign="middle">';
        $output .= '<input name="login" class="button" type="submit" value="'.Tools::atktext('login').'">';
        $tplvars['auth_enablepasswordmailer'] = $this->get_enablepasswordmailer();

        if ($this->get_enablepasswordmailer()) {
            $output .= '&nbsp;&nbsp;<input name="login" class="button" type="submit" value="'.Tools::atktext('password_forgotten').'">';
            $tplvars['forgotpasswordbutton'] = '<input name="login" class="button" type="submit" value="'.Tools::atktext('password_forgotten').'">';
        }
        $output .= '</td></tr>';
        $output .= '</table></form>';

        $tplvars['content'] = $output;
        $page->addContent($ui->render('login.tpl', $tplvars));
        $o = Output::getInstance();
        $o->output($page->render(Tools::atktext('app_title'), Page::HTML_STRICT, '', $ui->render('login_meta.tpl')));
    }

    /**
     * Check if the currently logged-in user has a certain privilege on a
     * node.
     *
     * @param string $node The full nodename of the node for which to check
     *                          access privileges. (modulename.nodename notation).
     * @param string $privilege The privilege to check (atkaction).
     *
     * @return bool True if the user has the privilege, false if not.
     */
    public function allowed($node, $privilege)
    {
        static $_cache = [];

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
     * @return bool true if access is granted, false if not.
     */
    public function attribAllowed($attr, $mode, $record = null)
    {
        return $this->m_authorization->attribAllowed($this, $attr, $mode, $record);
    }

    /**
     * Check if the currently logged in user has the requested level.
     *
     * @param int $level The level to check.
     *
     * @return bool True if the user has the required level, false if not.
     */
    public function hasLevel($level)
    {
        if (is_array($level)) {
            if (is_array($this->m_user['level'])) {
                return count(array_intersect($this->m_user['level'], $level)) >= 1;
            } else {
                return in_array($this->m_user['level'], $level);
            }
        } else {
            if (is_array($this->m_user['level'])) {
                return in_array($level, $this->m_user['level']);
            } else {
                return $this->m_user['level'] == $level;
            }
        }
    }

    /**
     * Write an access entry in the logfile.
     *
     * @param string $node The full name of the node that is being accessed.
     * @param string $action The action that has been performed.
     */
    public function logAction($node, $action)
    {
        $this->log(2, "Performing $node.$action");
    }

    /**
     * Write a logentry in the logfile.
     * The entry is only written to the file, if the level of the message is
     * equal or higher than the setting of $config_logging.
     *
     * @todo Logging should be moved to a separate atkLogger class.
     *
     * @param int $level The loglevel.
     * @param string $message The message to log.
     */
    public function log($level, $message)
    {
        if (Config::getGlobal('logging') > 0 && Config::getGlobal('logging') >= $level) {
            $fp = @fopen(Config::getGlobal('logfile'), 'a');
            if ($fp) {
                $logstamp = '['.date('d-m-Y H:i:s').'] ['.$_SERVER['REMOTE_ADDR'].'] '.$this->m_user['name'].' | ';
                @fwrite($fp, $logstamp.$message."\n");
                @fclose($fp);
            } else {
                Tools::atkdebug('error opening logfile');
            }
        }
    }

    /**
     * Getter for m_enablepasswordmailer.
     *
     * @return bool
     */
    public function get_enablepasswordmailer()
    {
        return $this->m_enablepasswordmailer;
    }

    /**
     * Retrieve all known information about the currently logged-in user.
     *
     * @param $key string
     *
     * @return array Array with userinfo, or "" if no user is logged in.
     */
    public static function atkGetUser($key = '')
    {
        $sm = SessionManager::getInstance();
        $session = SessionManager::getSession();
        $user = '';
        $session_auth = is_object($sm) ? $sm->getValue('authentication', 'globals') : [];
        if (Config::getGlobal('authentication_session') && Tools::atkArrayNvl($session, 'login',
                0) == 1 && $session_auth['authenticated'] == 1 && !empty($session_auth['user'])
        ) {
            $user = $session_auth['user'];
            if (!isset($user['access_level']) || empty($user['access_level'])) {
                $user['access_level'] = 0;
            }
        }

        if ($key) {
            return $user[$key];
        }

        return $user;
    }

    /**
     * Retrieve id of the currently logged-in user.
     *
     * @return int user id or 0 if not logged in or administrator
     */
    public static function atkGetUserId()
    {
        $user = self::atkGetUser();
        $userpk = Config::getGlobal('auth_userpk');

        // check if logged in || logged in as administrator
        if ($user == '' || $userpk == '' || (is_array($user) && !isset($user[$userpk]))) {
            return 0;
        }

        return $user[$userpk];
    }

    private function rememberMeCookieName()
    {
        return Config::getGlobal('identifier').'-'.Config::getGlobal('auth_rememberme_cookiename');
    }

    /**
     * @param $user
     * @return int $tokenId
     */
    private function rememberMeStore($user)
    {
        $db = Db::getInstance();
        $dbTable = Config::getGlobal('auth_rememberme_dbtable');

        $expires = new \DateTime(Config::getGlobal('auth_rememberme_expireinterval'));

        $selector = base64_encode(openssl_random_pseudo_bytes(9));
        $authenticator = openssl_random_pseudo_bytes(33);

        $sql = "INSERT INTO `".$dbTable."` (selector, token, user, expires, created) VALUES (?, ?, ?, ?, NOW())";

        $stmt = $db->prepare($sql);
        $stmt->execute([$selector, hash('sha256', $authenticator), $user, $expires->format('Y-m-d H:i:s')]);
        $db->commit();

        //get the current tokenId
        $tokenId = $db->getValue("SELECT id FROM `".$dbTable."` WHERE selector = '".$db->escapeSQL($selector)."'");

        //create the cookie
        $tokenValue = $selector.':'.base64_encode($authenticator);
        $expires = new \DateTime(Config::getGlobal('auth_rememberme_expireinterval'));
        setcookie($this->rememberMeCookieName(), $tokenValue, $expires->format('U'), '/', null, null, true);

        return $tokenId;
    }

    private function rememberMeClearCookie()
    {
        $name = $this->rememberMeCookieName();
        unset($_COOKIE[$name]);
        setcookie($name, '', time() - 3600, '/', null, null, true);
    }

    /**
     * @param bool $deleteToken
     * @return string username
     */
    private function rememberMeVerifyCookie($deleteToken = true)
    {
        $name = $this->rememberMeCookieName();
        $remember = isset($_COOKIE[$name]) ? $_COOKIE[$name] : null;

        if (!$remember) {
            return null;
        }

        $dbTable = Config::getGlobal('auth_rememberme_dbtable');

        list($selector, $authenticator) = explode(':', $remember);

        $db = Db::getInstance();
        $sql = "SELECT * FROM `$dbTable` WHERE selector = '".$db->escapeSQL($selector)."'";
        $row = $db->getRow($sql);

        //token found?
        if (!$row) {
            return null;
        }

        $ret = $row['user'];

        try {
            //token verified?
            if (!hash_equals($row['token'], hash('sha256', base64_decode($authenticator)))) {
                throw new \Exception('token not match');
            }

            //token expired?
            if (new \Datetime() > new \DateTime($row['expires'])) {
                throw new \Exception('token expired');
            }
        } catch (\Exception $e) {
            $ret = null;
        }

        //delete token
        if ($deleteToken) {
            $this->rememberMeDeleteToken($row['id']);
        }


        return $ret;
    }

    private function rememberMeDeleteToken($id)
    {
        $db = Db::getInstance();
        $dbTable = Config::getGlobal('auth_rememberme_dbtable');
        $sql = "DELETE FROM `$dbTable` WHERE id = ?";
        $stmt = $db->prepare($sql);
        $stmt->execute([$id]);
        $db->commit();
    }
}
