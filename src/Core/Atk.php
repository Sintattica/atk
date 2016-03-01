<?php

namespace Sintattica\Atk\Core;


use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\IndexPage;
use Sintattica\Atk\Handlers\ActionHandler;

class Atk
{

    public function __construct()
    {
        Bootstrap::run();
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
    public static function registerNode($nodeUri, $class, $actions = null, $tabs = array(), $section = null)
    {
        global $g_nodes, $g_nodesClasses;

        if (!is_array($tabs)) {
            $section = $tabs;
            $tabs = array();
        }

        $module = Tools::getNodeModule($nodeUri);
        $type = Tools::getNodeType($nodeUri);


        $g_nodesClasses[$nodeUri] = $class;

        if($actions) {
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

            $g_nodes[$section][$module][$type] = array_merge($actions, $tabs);
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
    static public function atkGetNode($nodeUri, $init = true, $cache_id = "default", $reset = false)
    {
        global $g_nodeRepository;
        $nodeUri = strtolower($nodeUri);
        if (!isset($g_nodeRepository[$cache_id][$nodeUri]) || !is_object($g_nodeRepository[$cache_id][$nodeUri]) || $reset) {
            Tools::atkdebug("Constructing a new node $nodeUri ($cache_id)");
            $g_nodeRepository[$cache_id][$nodeUri] = self::newAtkNode($nodeUri, $init);
        }
        return $g_nodeRepository[$cache_id][$nodeUri];
    }


    /**
     * Retrieves all the registered Modules
     *
     * @return array with modules
     */
    public static function atkGetModules()
    {
        global $g_modules;
        return $g_modules;
    }

    /**
     * Retrieve the Module with the given name.
     *
     * @param string $moduleName The name of the module
     * @return Module An instance of the Module
     */
    static public function atkGetModule($moduleName)
    {
        global $g_moduleRepository;

        if (!isset($g_moduleRepository[$moduleName]) || !is_object($g_moduleRepository[$moduleName])) {

            Tools::atkdebug("Constructing a new module - $moduleName");
            $modules = Atk::atkGetModules();
            $modClass = $modules[$moduleName];
            $g_moduleRepository[$moduleName] = new $modClass();
        }
        return $g_moduleRepository[$moduleName];
    }

    /**
     * Construct a new node
     * @param string $nodeUri the node uri
     * @param bool $init initialize the node?
     * @return Node new node object
     */
    static public function newAtkNode($nodeUri, $init = true)
    {
        global $g_nodesClasses;

        $nodeClass = $g_nodesClasses[$nodeUri];

        /** @var Node $node */
        $node = new $nodeClass($nodeUri);

        if ($init && $node != null) {
            $node->init();
        }
        return $node;
    }


    /**
     * Return the physical directory of a module.
     * @param string $module name of the module.
     * @return String The path to the module.
     */
    public static function moduleDir($module)
    {
        $modules = Atk::atkGetModules();
        if (isset($modules[$module])) {
            $class = $modules[$module];

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
    public static function &atkGetNodeHandler($nodeUri, $action)
    {
        global $g_nodeHandlers;
        if (isset($g_nodeHandlers[$nodeUri][$action])) {
            $handler = $g_nodeHandlers[$nodeUri][$action];
        } elseif (isset($g_nodeHandlers["*"][$action])) {
            $handler = $g_nodeHandlers["*"][$action];
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
    public static function atkRegisterNodeHandler($nodeUri, $action, $handler)
    {
        global $g_nodeHandlers;
        if (isset($g_nodeHandlers[$nodeUri][$action])) {
            return false;
        } else {
            $g_nodeHandlers[$nodeUri][$action] = $handler;
        }
        return true;
    }



    public function registerModule($moduleClass)
    {
        global $g_modules;


        $reflection = new \ReflectionClass($moduleClass);

        $name = strtolower($reflection->getStaticPropertyValue('module'));

        /** @var Module $module */
        $g_modules[$name] = $moduleClass;

        return self::atkGetModule($name);
    }
}
