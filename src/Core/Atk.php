<?php

namespace Sintattica\Atk\Core;

use Sintattica\Atk\Security\SqlWhereclauseBlacklistChecker;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\IndexPage;
use Sintattica\Atk\Handlers\ActionHandler;


class Atk
{
    var $g_nodes = [];
    var $g_nodesClasses = [];
    var $g_nodeRepository = [];
    var $g_modules = [];
    var $g_moduleRepository = [];
    var $g_nodeHandlers = [];
    var $g_nodeListeners = [];


    public function __construct()
    {
        // do nothing fancy here, because Atk isn't ready yet
        // (This is the framework, btw)

        global $g_startTime;
        $g_startTime = microtime(true);
    }

    private function init()
    {

        require_once('adodb-time.php');
        Config::init();

        if (Config::getGlobal('use_atkerrorhandler', true)) {
            set_error_handler('Sintattica\Atk\Core\Tools::atkErrorHandler');
            error_reporting(E_ALL);
            set_exception_handler('Sintattica\Atk\Core\Tools::atkExceptionHandler');
        }

        /**
         * Filter the atkselector REQUEST variable for blacklisted SQL (like UNIONs)
         */
        SqlWhereclauseBlacklistChecker::filter_request_where_clause('atkselector');
        SqlWhereclauseBlacklistChecker::filter_request_where_clause('atkfilter');


        if (Config::getGlobal('debug') > 0) {
            ini_set('display_errors', 1);
        }

        $locale = Tools::atktext('locale', 'atk', '', '', true);
        if ($locale) {
            setlocale(LC_TIME, $locale);
        }

        Tools::atkdebug('Created a new Atk instance: Server info: ' . $_SERVER['SERVER_NAME'] . ' (' . $_SERVER['SERVER_ADDR'] . ')');

        $modules = Config::getGlobal('modules');
        if(is_array($modules)){
            foreach($modules as $module) {
                $this->registerModule($module);
            }
        }

    }


    /**
     * Get new Atk object
     *
     * @return Atk class object
     */
    public static function &getInstance($time = null)
    {
        static $s_instance = null;
        if ($s_instance == null) {
            $s_instance = new Atk();
            $s_instance->init();
        }

        return $s_instance;
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
        $this->g_nodesClasses[$nodeUri] = $class;

        if ($actions) {
            // prefix tabs with tab_
            for ($i = 0, $_i = count($tabs); $i < $_i; $i++) {
                $tabs[$i] = "tab_" . $tabs[$i];
            }

            if ($module == "") {
                $module = "main";
            }
            if ($section == null) {
                $section = $module;
            }

            $this->g_nodes[$section][$module][$type] = array_merge($actions, $tabs);
        }

    }

    public function run()
    {

        $sessionManager = SessionManager::getInstance();
        $sessionManager->start();

        $securityManager = SecurityManager::getInstance();
        if ($securityManager->authenticate()) {
            $indexPage = new IndexPage($this);
            $indexPage->generate();
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
     * @return Node the node
     */
    public function atkGetNode($nodeUri, $init = true, $cache_id = "default", $reset = false)
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
     * @return Module An instance of the Module
     */
    public function atkGetModule($moduleName)
    {
        if (!static::isModule($moduleName)) {
            Tools::atkdebug("Constructing a new module - $moduleName");
            $modClass = $this->g_modules[$moduleName];
            $this->g_moduleRepository[$moduleName] = new $modClass();
        }
        return $this->g_moduleRepository[$moduleName];
    }

    public function isModule($moduleName) {
        return is_object($this->g_moduleRepository[$moduleName]);
    }

    /**
     * Construct a new node
     * @param string $nodeUri the node uri
     * @param bool $init initialize the node?
     * @return Node new node object
     */
    public function newAtkNode($nodeUri, $init = true)
    {
        $nodeClass = $this->g_nodesClasses[$nodeUri];

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
     * @param string $moduleName name of the module.
     * @return String The path to the module.
     */
    public function moduleDir($moduleName)
    {
        $modules = $this->g_modules;
        if (isset($modules[$moduleName])) {
            $class = $modules[$moduleName];

            $reflection = new \ReflectionClass($class);
            $dir = dirname($reflection->getFileName());
            if (substr($dir, -1) != '/') {
                $dir .= "/";
            }
            return $dir;
        }
        return "";
    }


    /**
     * Returns a registered node action handler.
     * @param string $nodeUri the uri of the node
     * @param string $action the node action
     * @return ActionHandler functionname or object (is_subclass_of ActionHandler) or
     *         NULL if no handler exists for the specified action
     */
    public function atkGetNodeHandler($nodeUri, $action)
    {
        if (isset($this->g_nodeHandlers[$nodeUri][$action])) {
            $handler = $this->g_nodeHandlers[$nodeUri][$action];
        } elseif (isset($this->g_nodeHandlers["*"][$action])) {
            $handler = $this->g_nodeHandlers["*"][$action];
        } else {
            $handler = null;
        }
        return $handler;
    }

    /**
     * Registers a new node action handler.
     * @param string $nodeUri the uri of the node (* matches all)
     * @param string $action the node action
     * @param string /atkActionHandler $handler handler functionname or object (is_subclass_of atkActionHandler)
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
