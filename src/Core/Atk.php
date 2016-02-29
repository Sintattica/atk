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
        global $g_nodeRepository;
        $nodeUri = strtolower($nodeUri);
        if (!isset($g_nodeRepository[$cache_id][$nodeUri]) || !is_object($g_nodeRepository[$cache_id][$nodeUri]) || $reset) {
            Tools::atkdebug("Constructing a new node $nodeUri ($cache_id)");
            $g_nodeRepository[$cache_id][$nodeUri] = $this->newAtkNode($nodeUri, $init);
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
     * @param string $modname The name of the module
     * @return Module An instance of the atkModule
     */
    public function atkGetModule($moduleName)
    {
        global $g_moduleRepository;

        if (!isset($g_moduleRepository[$moduleName]) || !is_object($g_moduleRepository[$moduleName])) {

            Tools::atkdebug("Constructing a new module - $moduleName");
            $modules = self::atkGetModules();
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
    protected function &newAtkNode($nodeUri, $init = true)
    {
        $nodeUri = strtolower($nodeUri);
        $module = Module::getNodeModule($nodeUri);
        $module_inst = $this->atkGetModule($module);

        $node = $module_inst->newNode($nodeUri);
        if ($init && $node != null) {
            $node->init();
        }
        return $node;
    }


    /**
     * Return the physical directory of a module..
     * @param string $module name of the module.
     * @return String The path to the module.
     */
    public static function moduleDir($module)
    {
        $modules = self::atkGetModules();
        if (isset($modules[$module])) {
            $dir = $modules[$module];
            if (substr($dir, -1) != '/') {
                return $dir . "/";
            }
            return $dir;
        }
        return "";
    }


    /**
     * Check wether a module is installed
     * @param string $module The modulename.
     * @return bool True if it is, false otherwise
     */
    public static function moduleExists($module)
    {
        $modules = self::atkGetModules();
        return (is_array($modules) && in_array($module, array_keys($modules)));
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





    public function registerModule($moduleClass, $flags = 0)
    {
        global $g_modules, $g_moduleflags;

        $reflection = new \ReflectionClass($moduleClass);

        $name = strtolower($reflection->getShortName());

        /** @var Module $module */
        $g_modules[$name] = $moduleClass;

        if ($flags > 0) {
            $g_moduleflags[$name] = $flags;
        }

    }
}
