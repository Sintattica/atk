<?php namespace Sintattica\Atk\Core;


use Sintattica\Atk\Security\SqlWhereclauseBlacklistChecker;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\SmartyProvider;

class Bootstrap
{

    public static function run()
    {
        require_once('adodb-time.php');
        self::initGlobals();
        Config::loadGlobals();
        self::setLocale();
        self::setErrorHandler();
        self::setDbGlobals();

        if (Config::getGlobal('session_init', true)) {
            self::setSession();
        }
        self::setSecurity();
        self::setDebugging();

        //non needed now, only for frontcontroller
        //SmartyProvider::addFunction('atk.ui.plugins', 'atkfrontcontroller');

    }

    private static function initGlobals()
    {
        /**
         * int $g_maxlevel treenode var
         */
        global $g_maxlevel;
        $g_maxlevel = 0;


        list($usec, $sec) = explode(" ", microtime());

        /**
         * Current microtime, to see when exactly this request started.
         * @var float
         */
        $GLOBALS['g_startTime'] = (float)$usec + (float)$sec; // can't use getmicrotime here because it isn't available yet

        /**
         * Global node list
         */
        $GLOBALS['g_nodes'] = array();

        /**
         * Global module list
         */
        $GLOBALS['g_modules'] = array();

        /**
         * Global menu
         */
        $GLOBALS['g_menu'] = array();

        /**
         * Global moduleflags per module
         */
        $GLOBALS['g_moduleflags'] = array();

        /**
         * Sticky global variables.
         * When you add 'key' to this, ATK will always pass 'key=$GLOBALS['key']'
         * in session urls and forms.
         * @var array
         */
        $GLOBALS['g_stickyurl'] = array();

        /**
         * Module globals
         */
        /**
         * A repository of node instances..
         * @access private
         * @var Array
         */
        $GLOBALS['g_nodeRepository'] = array();

        /**
         * A repository of module instances..
         * @access private
         * @var Array
         */
        $GLOBALS['g_moduleRepository'] = array();

        /**
         * registered node action handlers
         * @access private
         * @var Array
         */
        $GLOBALS['g_nodeHandlers'] = array();

        /**
         * registered node listeners
         * @access private
         * @var Array
         */
        $GLOBALS['g_nodeListeners'] = array();

        /**
         * registered node controllers
         * @access private
         * @var Array
         */
        $GLOBALS['g_nodeControllers'] = array();

    }

    private static function setLocale()
    {
        $locale = Tools::atktext('locale', 'atk');
        if ($locale != null) {
            setlocale(LC_TIME, $locale);
        }

        $locale = Tools::atktext('locale', 'atk');
        if ($locale != null) {
            setlocale(LC_TIME, $locale);
        }
    }


    private static function setErrorHandler()
    {
        if (Config::getGlobal('use_atkerrorhandler', true)) {
            set_error_handler('Sintattica\Atk\Core\Tools::atkErrorHandler');
            error_reporting(E_ALL);
            set_exception_handler('Sintattica\Atk\Core\Tools::atkExceptionHandler');
        }
    }

    private static function setSecurity()
    {
        /**
         * Filter the atkselector REQUEST variable for blacklisted SQL (like UNIONs)
         */
        SqlWhereclauseBlacklistChecker::filter_request_where_clause('atkselector');
        SqlWhereclauseBlacklistChecker::filter_request_where_clause('atkfilter');

        // initialise g_ array.
        $GLOBALS['g_user'] = array();
    }


    private static function setDebugging()
    {
        if (Tools::atk_value_in_array($GLOBALS['config_smart_debug'])) {
            $GLOBALS['config_debug'] = Config::smartDebugLevel($GLOBALS['config_debug'],
                $GLOBALS['config_smart_debug']);
        }

        if ($GLOBALS['config_debug'] > 0) {
            ini_set('display_errors', 1);
        }

        // show server info in debug (useful in clustered environments)
        Tools::atkdebug('Server info: ' . $_SERVER['SERVER_NAME'] . ' (' . $_SERVER['SERVER_ADDR'] . ')');
    }

    private static function setSession()
    {
        global $atklevel, $atkprevlevel, $atkstackid;

        if (isset($_REQUEST["atklevel"])) {
            $atklevel = trim($_REQUEST["atklevel"]);
        }
        if (isset($_REQUEST["atkprevlevel"])) {
            $atkprevlevel = trim($_REQUEST["atkprevlevel"]);
        }
        if (isset($_REQUEST["atkstackid"])) {
            $atkstackid = trim($_REQUEST["atkstackid"]);
        }
    }

    private static function setDbGlobals(){
        if (!Config::getGlobal('meta_caching')) {
            Tools::atkwarning("Table metadata caching is disabled. Turn on \$config_meta_caching to improve your application's performance!");
        }

        /**
         * Global array containing database instances. Global is necessary because
         * PHP4 doesn't support static class members.
         */
        global $g_dbinstances;
        $g_dbinstances = array();
    }

}