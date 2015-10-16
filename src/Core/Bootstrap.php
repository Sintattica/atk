<?php namespace Sintattica\Atk\Core;


use Sintattica\Atk\Security\SqlWhereclauseBlacklistChecker;


class Bootstrap
{

    public static function run()
    {
        require_once('adodb-time.php');
        self::requireGlobals();
        Config::loadGlobals();
        self::setLocale();
        self::setErrorHandler();
        self::setSecurity();
        self::setDebugging();
        Module::atkPreloadModules();
    }

    private static function requireGlobals()
    {
        list($usec, $sec) = explode(" ", microtime());

        /**
         * Current microtime, to see when exactly this request started.
         * @var float
         */
        $GLOBALS['g_startTime'] = (float)$usec + (float)$sec; // can't use getmicrotime here because it isn't available yet

        /**
         * Global theme variable, containing theme data
         * @deprecated In favor of Theme class
         * @var array
         */
        $GLOBALS['g_theme'] = array();

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
         * Modifiers
         */
        $GLOBALS['g_modifiers'] = array();

        /**
         * Overloaders
         */
        $GLOBALS['g_overloaders'] = array();
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
            set_error_handler('Tools::atkErrorHandler');
            error_reporting(E_ALL);
            set_exception_handler('Tools::atkExceptionHandler');
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

}