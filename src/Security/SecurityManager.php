<?php

namespace Sintattica\Atk\Security;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Security\Auth\AuthInterface;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\Output;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Ui\Ui;
use Sintattica\Atk\Utils\Debugger;

/**
 * The security manager for ATK applications.
 */
class SecurityManager
{
    const AUTH_UNVERIFIED = 0; // initial value.
    const AUTH_SUCCESS = 1;
    const AUTH_LOCKED = 2;
    const AUTH_MISMATCH = 3;
    const AUTH_MISSINGUSERNAME = 5;
    const AUTH_ERROR = -1;

    public $m_authentication = [];

    /** @var AuthInterface $m_authorization */
    public $m_authorization;

    public $m_scheme = 'none';
    public $m_user;
    public $m_listeners = [];
    public $m_fatalError;
    public $auth_response;

    /**
     * @var array $system_users are special system users.
     * Can be enabled adding an atk config password value (administratorpassword / guestpassword)
     */
    protected $system_users = [
        ['name' => 'administrator', 'level' => -1, 'access_level' => 9999999],
        ['name' => 'guest', 'level' => -2, 'access_level' => 0],
    ];

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

        $this->auth_response = self::AUTH_UNVERIFIED;
    }

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
     * Perform user auth / deauth
     */
    public function run()
    {
        global $ATK_VARS;

        $isCli = php_sapi_name() === 'cli';

        // Logout?
        if (isset($ATK_VARS['atklogout'])) {
            $this->logout();
            if (!$isCli) {
                header('Location: '.Config::getGlobal('dispatcher'));
            }
            exit;
        }

        // Get some vars
        $session = &SessionManager::getSession();
        $auth_rememberme = isset($ATK_VARS['auth_rememberme']) ? $ATK_VARS['auth_rememberme'] : 0;

        if (Config::getGlobal('auth_loginform') == true) {
            // form login
            $auth_user = isset($ATK_VARS['auth_user']) ? $ATK_VARS['auth_user'] : '';
            $auth_pw = isset($ATK_VARS['auth_pw']) ? $ATK_VARS['auth_pw'] : '';
        } else {
            // HTTP login
            $auth_user = isset($_SERVER['PHP_AUTH_USER']) ? $_SERVER['PHP_AUTH_USER'] : '';
            $auth_pw = isset($_SERVER['PHP_AUTH_PW']) ? $_SERVER['PHP_AUTH_PW'] : '';
        }

        // try a session login
        if (isset($session['login']) && $session['login'] == 1) {
            $this->sessionLogin();
        }

        // try a rememberme login
        if (Config::getGlobal('auth_enable_rememberme') && $this->auth_response === self::AUTH_UNVERIFIED) {
            $this->rememberMeLogin();
        }

        // u2fauth verification?
        if (Config::getGlobal('auth_enable_u2f') && $this->auth_response === self::AUTH_UNVERIFIED && isset($ATK_VARS['u2f_response'])) {
            $this->u2fAuthenticate($auth_user, $ATK_VARS['u2f_response']);
        }

        // try a standard login with user / password
        if ($this->auth_response === self::AUTH_UNVERIFIED) {
            if($auth_user || $isCli) {
                $this->login($auth_user, $auth_pw);
            }

            if (Config::getGlobal('auth_enable_u2f') && $this->auth_response === self::AUTH_SUCCESS) {
                $u2f_enabledfield = Config::getGlobal('auth_u2f_enabledfield');
                if (isset($this->m_user[$u2f_enabledfield]) && $this->m_user[$u2f_enabledfield]) {
                    $this->u2fAuthenticationForm($auth_user, $auth_rememberme);
                    exit;
                }
            }
        }

        // Error handling
        if ($this->m_fatalError) {
            if (Config::getGlobal('auth_loginform')) {
                $this->loginForm($auth_user, $this->m_fatalError);
            }
            exit;
        }

        // Not logged in: redirect to login page or set Unauthorized header and exit
        if (!$this->m_user) {
            if (!$isCli) {
                $location = Config::getGlobal('auth_loginpage', '');
                if ($location) {
                    $location .= (strpos($location, '?') === false) ? '?' : '&';
                    $location .= 'login='.urlencode($auth_user).'&error='.$this->auth_response;

                    if (Config::getGlobal('debug') >= 2) {
                        $debugger = Debugger::getInstance();
                        $debugger->setRedirectUrl($location);
                        Tools::atkdebug('Non-debug version would have redirected to <a href="'.$location.'">'.$location.'</a>');
                        $output = Output::getInstance();
                        $output->outputFlush();
                    } else {
                        header('Location: '.$location);
                    }
                } elseif (Config::getGlobal('auth_loginform')) {
                    $error =  $this->getAuthResponseTranslation($this->auth_response);
                    $this->loginForm($auth_user, $error);
                } else {
                    header('WWW-Authenticate: Basic realm="'.Tools::atktext('app_title').(Config::getGlobal('auth_changerealm', true) ? ' - '.strftime('%c',
                                time()) : '').'"');
                    if (preg_match('/Microsoft/', $_SERVER['SERVER_SOFTWARE'])) {
                        header('Status: 401 Unauthorized');
                    } else {
                        header('HTTP/1.0 401 Unauthorized');
                    }
                }
            }

            exit;
        }

        // successfully logged in

        //store remember me cookie
        if (Config::getGlobal('auth_enable_rememberme') && $auth_rememberme == '1') {
            $session['remembermeTokenId'] = $this->rememberMeStore($this->m_user['name']);
        }

        $this->postLogin($this->m_user);

        if (empty($session['login'])) {
            $session['login'] = 1;
            $this->notifyListeners('postLogin', $this->m_user['name']);
        }
    }

    public function getAuthResponseTranslation($auth_response){
        $error = '';
        switch ($auth_response) {
            case self::AUTH_LOCKED:
                $error = Tools::atktext('auth_account_locked');
                break;
            case self::AUTH_MISMATCH:
                $error = Tools::atktext('auth_mismatch');
                break;
            case self::AUTH_MISSINGUSERNAME:
                $error = Tools::atktext('auth_missingusername');
                break;
        }
        return $error;
    }

    public function isAuthenticated()
    {
        return $this->m_user ? true : false;
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
     * @param array extra data
     */
    public function notifyListeners($event, $username, $extra = [])
    {
        for ($i = 0, $_i = Tools::count($this->m_listeners); $i < $_i; ++$i) {
            $this->m_listeners[$i]->handleEvent($event, $username, $extra);
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
     * @param $password string
     * @param $hash string
     * @return bool
     */
    static public function verify($password, $hash)
    {
        if (Config::getGlobal('auth_ignorepasswordmatch')) {
            return true;
        }

        if (!Config::getGlobal('auth_usecryptedpassword')) {
            return $password == $hash;
        }

        return password_verify($password, $hash);
    }

    public function logout()
    {
        $currentUser = self::atkGetUser();
        $username = isset($currentUser['name']) ? $currentUser['name'] : '';

        $this->notifyListeners('preLogout', $username);

        foreach ($this->m_authentication as $class => $auth) {
            $auth->logout($currentUser);
        }

        if (Config::getGlobal('auth_enable_rememberme')) {
            $this->rememberMeDestroy();
        }

        $this->m_user = null;
        SessionManager::getInstance()->destroy();

        $this->notifyListeners('postLogout', $username);
    }

    protected function sessionLogin()
    {
        $sessionManager = SessionManager::getInstance();
        $session_auth = $sessionManager->getValue('authentication');
        if (Config::getGlobal('authentication_session') && $session_auth['authenticated'] == 1 && !empty($session_auth['user'])) {
            $this->m_user = &$session_auth['user'];
            Tools::atkdebug('SecurityManager: Using session for authentication / user = '.$this->m_user['name']);
            $this->auth_response = self::AUTH_SUCCESS;
        }
    }

    public function login($auth_user, $auth_pw)
    {
        $this->notifyListeners('preLogin', $auth_user);

        // System user
        if ($system_user = $this->getSystemUser($auth_user)) {
            $config_pw = Config::getGlobal($system_user['name'].'password');
            $match = ! empty($config_pw) && (Config::getGlobal('auth_ignorepasswordmatch') || self::verify($auth_pw, $config_pw));
            if ($match) {
                $this->auth_response = self::AUTH_SUCCESS;
                $this->m_user = $system_user;
                return $this->m_user;
            }

            $this->auth_response = self::AUTH_MISMATCH;
            $this->notifyListeners('errorLogin', $auth_user, ['auth_response' => $this->auth_response]);
            return;
        }

        // Standard user
        foreach ($this->m_authentication as $class => $obj) {
            $this->auth_response = $obj->validateUser($auth_user, $auth_pw);
            if ($this->auth_response === self::AUTH_SUCCESS) {
                $this->m_fatalError = null;
                $this->storeAuth($auth_user, $class);
                return $this->m_user;

            }elseif ($this->auth_response === self::AUTH_ERROR) {
                $this->m_fatalError = isset($obj->m_fatalError) ? $obj->m_fatalError : 'Login error';
            }
        }

        $extra = ['auth_response' => $this->auth_response];
        if($this->m_fatalError) {
            $extra['fatal_error'] = $this->m_fatalError;
        }
        $this->notifyListeners('errorLogin', $auth_user, $extra);
        return;
    }

    /**
     * @param array $user
     */
    public function postLogin(&$user) {
        $isCli = php_sapi_name() === 'cli';

        $this->m_user = $user;
        $GLOBALS['g_user'] = &$user;
        $sm = SessionManager::getInstance();
        $sm->setValue('authentication', ['authenticated' => 1, 'user' => $user]);
        if (!$isCli) {
            header('user: '.$user['name']);
        }
    }

    protected function storeAuth($auth_user, $auth_name)
    {
        Tools::atkdebug("SecurityManager: Using $auth_name for authentication / user = $auth_user");

        if ($system_user = $this->getSystemUser($auth_user)) {
            $this->m_user = $system_user;
        } else {
            $this->m_user = $this->m_authorization->getUser($auth_user);
        }

        $this->m_user['AUTH'] = $auth_name; // something to see which auth scheme is used
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
        $old_auth = $sessionManager->getValue('authentication');
        $old_auth['user'] = $this->m_user;
        $sessionManager->setValue('authentication', $old_auth);
    }

    /**
     * Display a login form.
     *
     * @param string $defaultname The username that might already be known
     * @param string $error The error message
     */
    public function loginForm($defaultname, $error = '')
    {
        $page = Page::getInstance();
        $ui = Ui::getInstance();

        $tplvars = [];
        $tplvars['atksessionformvars'] = Tools::makeHiddenPostvars(['atklogout', 'auth_rememberme', 'u2f_response']);
        $tplvars['formurl'] = Config::getGlobal('dispatcher');
        $tplvars['username'] = Tools::atktext('username');
        $tplvars['password'] = Tools::atktext('password');
        $tplvars['defaultname'] = htmlentities($defaultname);
        $tplvars['passwordfield'] = '<input class="loginform" type="password" size="20" name="auth_pw" value="" />';
        $tplvars['submitbutton'] = '<input name="login" class="button" type="submit" value="'.Tools::atktext('login').'" />';
        $tplvars['title'] = Tools::atktext('login_form');
        if ($error != '') {
            $tplvars['error'] = $error;
        }

        if (Config::getGlobal('auth_enable_rememberme')) {
            $tplvars['auth_enable_rememberme'] = true;
            if (isset($_POST['auth_rememberme']) && $_POST['auth_rememberme'] == '1') {
                $tplvars['auth_rememberme'] = true;
            }
        }

        $page->addContent($ui->render('login.tpl', $tplvars));
        $output = Output::getInstance();
        $output->output($page->render(Tools::atktext('app_title'), Page::HTML_STRICT, '', $ui->render('login_meta.tpl')));
        $output->outputFlush();
        exit;
    }

    public function getSystemUser($username)
    {
        foreach ($this->system_users as $user) {
            if ($user['name'] === $username) {
                return $user;
            }
        }

        return null;
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
                return Tools::count(array_intersect($this->m_user['level'], $level)) >= 1;
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
     * Retrieve all known information about the currently logged-in user.
     *
     * @param $key string
     *
     * @return array Array with userinfo, or null if no user is logged in.
     */
    public static function atkGetUser($key = '')
    {
        $user = null;
        $sm = SessionManager::getInstance();
        $session_auth = is_object($sm) ? $sm->getValue('authentication') : [];
        if (Config::getGlobal('authentication_session') && $session_auth['authenticated'] == 1 && !empty($session_auth['user'])
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

        // check if logged in || logged in as system user
        if ($user == '' || $userpk == '' || (is_array($user) && !isset($user[$userpk]))) {
            return 0;
        }

        return $user[$userpk];
    }

    public static function isUserAdmin($user = null)
    {
        if ($user === null) {
            $user = self::atkGetUser();
        }

        // special administrator system user
        if ($user['name'] === 'administrator' && (!isset($user['id']) || is_null($user['id'])) && Config::getGlobal('administratorpassword') != '') {
            return true;
        }

        $auth_administratorfield = Config::getGlobal('auth_administratorfield');
        if ($auth_administratorfield && isset($user[$auth_administratorfield]) && in_array(strtolower($user[$auth_administratorfield]), ['y', 'j', 'yes', 'on', 'true', 't', '1'])) {
            return true;
        }

        $auth_administratorusers = Config::getGlobal('auth_administratorusers');
        if (is_array($auth_administratorusers) && in_array($user['name'], $auth_administratorusers)) {
            return true;
        }

        return false;
    }

    private function rememberMeLogin()
    {
        $remember_user = $this->rememberMeVerifyCookie();

        if ($remember_user) {
            $session = &SessionManager::getSession();
            $this->notifyListeners('preLogin', $remember_user);
            $session['remembermeTokenId'] = $this->rememberMeStore($remember_user);
            $isValid = $this->m_authorization->isValidUser($remember_user);
            if ($isValid) {
                $this->storeAuth($remember_user, 'rememberme');
                $this->auth_response = self::AUTH_SUCCESS;
                Tools::atkdebug('Using rememberme for authentication / user = '.$remember_user);
            }
        }
    }

    private function rememberMeCookieName()
    {
        return Config::getGlobal('identifier').'-'.Config::getGlobal('auth_rememberme_cookiename');
    }

    /**
     * @param $username
     * @return int $tokenId
     */
    private function rememberMeStore($username)
    {
        $db = Db::getInstance();
        $dbTable = Config::getGlobal('auth_rememberme_dbtable');

        $expires = new \DateTime(Config::getGlobal('auth_rememberme_expireinterval'));

        $selector = base64_encode(openssl_random_pseudo_bytes(9));
        $authenticator = openssl_random_pseudo_bytes(33);

        $userfield = Config::getGlobal('auth_userfield');
        $query = $db->createQuery($dbTable);
        $query->addField('selector', $selector)
            ->addField('token', hash('sha256', $authenticator))
            ->addField($userfield, $username)
            ->addField('expires', $expires->format('Y-m-d H:i:s'))
            ->addField('created', date('Y-m-d H:i:s'));
        $query->executeInsert();
        $db->commit();

        //get the current tokenId
        $tokenId = $db->getValue(
            "SELECT id FROM ".Db::quoteIdentifier($dbTable)." WHERE selector = :selector",
            [':selector' => $selector]
        );

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
        $sql = 'SELECT * FROM '.Db::quoteIdentifier($dbTable)." WHERE selector = :selector";
        $row = $db->getRow($sql, [':selector' => $selector]);

        //token found?
        if (!$row) {
            return null;
        }

        $ret = $row[Config::getGlobal('auth_userfield')];

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
        $sql = 'DELETE FROM '.Db::quoteIdentifier($dbTable).' WHERE id = ?';
        $stmt = $db->prepare($sql);
        $stmt->execute([$id]);
        $db->commit();
    }

    private function rememberMeDestroy()
    {
        $this->rememberMeClearCookie();
        $session = &SessionManager::getSession();
        if (isset($session['remembermeTokenId'])) {
            $this->rememberMeDeleteToken($session['remembermeTokenId']);
        }
    }


    /****** U2F ******/

    /**
     * @return U2F
     */
    public function getU2F()
    {
        static $s_u2f;
        if ($s_u2f === null) {
            $scheme = isset($_SERVER['HTTPS']) ? "https://" : "http://";
            $s_u2f = new U2F($scheme.$_SERVER['HTTP_HOST']);
        }

        return $s_u2f;
    }

    public function u2fGetRegistrations($user_id)
    {
        $table = Config::getGlobal('auth_u2f_dbtable');
        $userfk = Config::getGlobal('auth_userfk');

        $db = Db::getInstance();
        $sel = $db->prepare('select * from '.Db::quoteIdentifier($table).' where '.Db::quoteIdentifier($userfk).' = ?');
        $sel->execute([$user_id]);
        $res = [];
        while ($r = $sel->fetch()) {
            $res[] = json_decode(json_encode($r), false);
        }

        return $res;
    }

    private function u2fUpdateRegistration($reg)
    {
        $table = Config::getGlobal('auth_u2f_dbtable');
        $db = Db::getInstance();
        $upd = $db->prepare('UPDATE '.Db::quoteIdentifier($table).' set counter = ? where id = ?');
        $upd->execute([$reg->counter, $reg->id]);
        $db->commit();
    }

    private function u2fAuthenticate($auth_user, $u2f_response)
    {
        $session = &SessionManager::getSession();

        try {
            $requests = isset($session['u2f_authReq']) ? $session['u2f_authReq'] : '[]';
            $u2f = $this->getU2F();
            $user = $this->m_authorization->getUser($auth_user);
            $user_id = $user[Config::getGlobal('auth_userpk')];
            $registrations = $this->u2fGetRegistrations($user_id);
            $reg = $u2f->doAuthenticate(json_decode($requests), $registrations, json_decode($u2f_response));
            $this->u2fUpdateRegistration($reg);
            $this->auth_response = self::AUTH_SUCCESS;
            $this->storeAuth($auth_user, 'u2f');
        } catch (\Exception $e) {

            $res = json_decode($u2f_response, true);
            $error = '';
            if (isset($res['errorCode'])) {
                $error = Tools::atktext('u2f_errorcode_'.$res['errorCode'], '', '', '', '', true, false);
            }

            if ($error == '') {
                $error = Tools::atktext('u2f_error');
            }

            $this->auth_response = self::AUTH_ERROR;
            $this->m_fatalError = $error;
        } finally {
            unset($session['u2f_authReq']);
        }
    }

    private function u2fAuthenticationForm($auth_user, $auth_rememberme)
    {
        $session = &SessionManager::getSession();
        $page = Page::getInstance();
        $ui = Ui::getInstance();
        $user = $this->m_authorization->getUser($auth_user);
        $user_id = $user[Config::getGlobal('auth_userpk')];
        $u2f = $this->getU2F();
        $registrations = $this->u2fGetRegistrations($user_id);
        $requests = json_encode($u2f->getAuthenticateData($registrations));
        $session['u2f_authReq'] = $requests;

        $tplvars = [];
        $tplvars['formurl'] = Config::getGlobal('dispatcher');
        $tplvars['auth_user'] = htmlentities($auth_user);
        $tplvars['requests'] = $requests;
        $tplvars['auth_rememberme'] = $auth_rememberme;

        $result = $ui->render('u2f.tpl', $tplvars);
        $output = Output::getInstance();
        $page->register_script(Config::getGlobal('assets_url').'javascript/u2f-api.js');
        $page->addContent($result);
        $output->output($page->render(Tools::atktext('app_title')));
        $output->outputFlush();
        exit;
    }
}
