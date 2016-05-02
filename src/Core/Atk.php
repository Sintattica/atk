<?php

namespace Sintattica\Atk\Core;

use App\Modules\App\Module;
use Sintattica\Atk\Security\SqlWhereclauseBlacklistChecker;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\IndexPage;
use Sintattica\Atk\Handlers\ActionHandler;
use Dotenv\Dotenv;

class Atk
{
    const VERSION = 'v9.0.x-dev';

    public $g_nodes = [];
    public $g_nodesClasses = [];
    public $g_nodeRepository = [];
    public $g_modules = [];
    public $g_moduleRepository = [];
    public $g_nodeHandlers = [];
    public $g_nodeListeners = [];

    /** @var $s_instance Atk */
    public static $s_instance = null;

    public function __construct($environment, $basedir)
    {
        global $g_startTime;
        $g_startTime = microtime(true);

        if (static::$s_instance) {
            throw new \RuntimeException('Only one Atk app can be created');
        }

        static::$s_instance = $this;

        //load .env variables only in development environment
        if (!$environment || in_array(strtolower($environment), ['dev', 'develop', 'development'])) {
            $dotEnv = new Dotenv($basedir);
            $dotEnv->load();
        }

        require_once 'adodb-time.php';

        Config::init();

        if (Config::getGlobal('debug') > 0) {
            ini_set('display_errors', 1);
        }

        if (Config::getGlobal('use_atkerrorhandler', true)) {
            set_error_handler('Sintattica\Atk\Core\Tools::atkErrorHandler');
            error_reporting(E_ALL);
            set_exception_handler('Sintattica\Atk\Core\Tools::atkExceptionHandler');
        }

        // Filter the atkselector REQUEST variable for blacklisted SQL (like UNIONs)
        SqlWhereclauseBlacklistChecker::filter_request_where_clause('atkselector');
        SqlWhereclauseBlacklistChecker::filter_request_where_clause('atkfilter');

        // set locale
        $locale = Tools::atktext('locale', 'atk', '', '', true);
        if ($locale) {
            setlocale(LC_TIME, $locale);
        }

        $debug = 'Created a new Atk ('.self::VERSION.') instance: Server info: '.$_SERVER['SERVER_NAME'].' ('.$_SERVER['SERVER_ADDR'].')';
        $debug .= ' Environment: '.$environment;

        Tools::atkdebug($debug);

        //load modules
        $modules = Config::getGlobal('modules');
        if (is_array($modules)) {
            foreach ($modules as $module) {
                static::$s_instance->registerModule($module);
            }
        }
    }

    /**
     * Get new Atk object.
     *
     * @return Atk class object
     */
    public static function getInstance()
    {
        if (!is_object(static::$s_instance)) {
            throw new \RuntimeException('Atk instance not available');
        }

        return static::$s_instance;
    }

    public function run()
    {
        $sessionManager = SessionManager::getInstance();
        $sessionManager->start();

        $securityManager = SecurityManager::getInstance();
        if ($securityManager->authenticate()) {

            $indexPageClass = Config::getGlobal('indexPage');

            /** @var IndexPage $indexPage */
            $indexPage = new $indexPageClass($this);
            $indexPage->generate();
        }
    }
    

    /**
     * Tells ATK that a node exists, and what actions are available to
     * perform on that node.  Note that registerNode() is not involved in
     * deciding which users can do what, only in establishing the full set
     * of actions that can potentially be performed on the node.
     *
     * @param string $nodeUri uri of the node
     * @param string $class class of the node
     * @param $actions array with actions that can be performed on the node
     * @param $tabs array of tabnames for which security should be handled.
     *              Note that tabs that every user may see need not be
     *              registered.
     */
    public function registerNode($nodeUri, $class, $actions = null, $tabs = array(), $section = null)
    {
        if (!is_array($tabs)) {
            $section = $tabs;
            $tabs = array();
        }

        $module = Tools::getNodeModule($nodeUri);
        $type = Tools::getNodeType($nodeUri);
        $this->g_nodesClasses[strtolower($nodeUri)] = $class;

        if ($actions) {
            // prefix tabs with tab_
            for ($i = 0, $_i = count($tabs); $i < $_i; ++$i) {
                $tabs[$i] = 'tab_'.$tabs[$i];
            }

            if ($module == '') {
                $module = 'main';
            }
            if ($section == null) {
                $section = $module;
            }

            $this->g_nodes[$section][$module][$type] = array_merge($actions, $tabs);
        }
    }

    /**
     * Get an instance of a node. If an instance doesn't exist, it is created.  Note that nodes
     * are cached (unless $reset is true); multiple requests for the same node will return exactly
     * the same node object.
     *
     * @param string $nodeUri The node uri
     * @param bool $init Initialize the node?
     * @param string $cache_id The cache id in the node repository
     * @param bool $reset Whether or not to reset the particular node in the repository
     *
     * @return Node the node
     */
    public function atkGetNode($nodeUri, $init = true, $cache_id = 'default', $reset = false)
    {
        $nodeUri = strtolower($nodeUri);
        if (!isset($this->g_nodeRepository[$cache_id][$nodeUri]) || !is_object($this->g_nodeRepository[$cache_id][$nodeUri]) || $reset) {
            Tools::atkdebug("Constructing a new node $nodeUri ($cache_id)");
            $this->g_nodeRepository[$cache_id][$nodeUri] = $this->newAtkNode($nodeUri, $init);
        }

        return $this->g_nodeRepository[$cache_id][$nodeUri];
    }

    /**
     * Retrieve the Module with the given name.
     *
     * @param string $moduleName The name of the module
     *
     * @return Module An instance of the Module
     */
    public function atkGetModule($moduleName)
    {
        if (!static::isModule($moduleName)) {
            Tools::atkdebug("Constructing a new module - $moduleName");
            $modClass = $this->g_modules[$moduleName];

            /* @var Module $module */
            $menu = Menu::getInstance();
            $module = new $modClass(static::$s_instance, $menu);
            $this->g_moduleRepository[$moduleName] = $module;
            $module->boot();
        }

        return $this->g_moduleRepository[$moduleName];
    }

    public function isModule($moduleName)
    {
        return is_object($this->g_moduleRepository[$moduleName]);
    }

    /**
     * Construct a new node.
     *
     * @param string $nodeUri the node uri
     * @param bool $init initialize the node?
     *
     * @return Node new node object
     */
    public function newAtkNode($nodeUri, $init = true)
    {
        $nodeClass = $this->g_nodesClasses[strtolower($nodeUri)];

        Tools::atkdebug("Creating a new node: $nodeUri class: $nodeClass");

        /** @var Node $node */
        $node = new $nodeClass($nodeUri);
        if ($init && $node != null) {
            $node->init();
        }

        return $node;
    }

    /**
     * Return the physical directory of a module.
     *
     * @param string $moduleName name of the module.
     *
     * @return string The path to the module.
     */
    public function moduleDir($moduleName)
    {
        $modules = $this->g_modules;
        if (isset($modules[$moduleName])) {
            $class = $modules[$moduleName];

            $reflection = new \ReflectionClass($class);
            $dir = dirname($reflection->getFileName());
            if (substr($dir, -1) != '/') {
                $dir .= '/';
            }

            return $dir;
        }

        return '';
    }

    /**
     * Returns a registered node action handler.
     *
     * @param string $nodeUri the uri of the node
     * @param string $action the node action
     *
     * @return ActionHandler functionname or object (is_subclass_of ActionHandler) or
     *                       NULL if no handler exists for the specified action
     */
    public function atkGetNodeHandler($nodeUri, $action)
    {
        if (isset($this->g_nodeHandlers[$nodeUri][$action])) {
            $handler = $this->g_nodeHandlers[$nodeUri][$action];
        } elseif (isset($this->g_nodeHandlers['*'][$action])) {
            $handler = $this->g_nodeHandlers['*'][$action];
        } else {
            $handler = null;
        }

        return $handler;
    }

    /**
     * Registers a new node action handler.
     *
     * @param string $nodeUri the uri of the node (* matches all)
     * @param string $action the node action
     * @param string /atkActionHandler $handler handler functionname or object (is_subclass_of atkActionHandler)
     *
     * @return bool true if there is no known handler
     */
    public function atkRegisterNodeHandler($nodeUri, $action, $handler)
    {
        if (isset($this->g_nodeHandlers[$nodeUri][$action])) {
            return false;
        } else {
            $this->g_nodeHandlers[$nodeUri][$action] = $handler;
        }

        return true;
    }

    public function registerModule($moduleClass)
    {
        $reflection = new \ReflectionClass($moduleClass);
        $name = strtolower($reflection->getStaticPropertyValue('module'));
        $this->g_modules[$name] = $moduleClass;

        return $this->atkGetModule($name);
    }
}
