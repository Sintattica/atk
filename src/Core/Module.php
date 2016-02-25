<?php namespace Sintattica\Atk\Core;

use Sintattica\Atk\Handlers\ActionHandler;

/**
 * The Module abstract base class.
 *
 * All modules in an ATK application should derive from this class, and
 * override the methods of this abstract class as they see fit.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage modules
 * @abstract
 */
class Module
{

    /**
     * Don't use the rights of this module
     */
    const MF_NORIGHTS = 2;

    const MF_SPECIFIC_1 = 4;
    const MF_SPECIFIC_2 = 8;
    const MF_SPECIFIC_3 = 16;

    /**
     * Don't preload this module (module_preload.inc)
     */
    const MF_NO_PRELOAD = 32;


    /** @var  array $nodeMap */
    var $nodeMap;


    /**
     * Register nodes with their supported actions. Can be used
     * for security etc.
     */
    function getNodes()
    {

    }

    /**
     * This method returns an array with menu items that need to be available
     * in the main ATK menu. This function returns the array created with
     * the menuitem() method, and does not have to be extended!
     * @return array with menu items for this module
     */
    function getMenuItems()
    {

    }

    /**
     * Construct a new node. A module can override this method for it's own nodes.
     * @param string $nodeUri the node type
     * @return Node new node object
     */
    function &newNode($nodeUri)
    {

        $module = self::getNodeModule($nodeUri);

        // set the current module scope, this will be retrieved in Node
        // to set it's $this->m_module instance variable in an early stage
        self::setModuleScope($module);

        $type = self::getNodeType($nodeUri);
        $nodeClass = $this->nodeMap[$type];
        $node = new $nodeClass();
        $node->m_module = $module;

        self::resetModuleScope();

        return $node;
    }

    /**
     * Set current module scope.
     *
     * @param string $module current module
     */
    public static function setModuleScope($module)
    {
        global $g_atkModuleScope;
        $g_atkModuleScope = $module;
    }

    /**
     * Returns the current module scope.
     *
     * @return string current module
     */
    public static function getModuleScope()
    {
        global $g_atkModuleScope;
        return $g_atkModuleScope;
    }

    /**
     * Resets the current module scope.
     *
     */
    public static function resetModuleScope()
    {
        self::setModuleScope(null);
    }

    /**
     * Gets the module of the node
     * @param string $nodeUri the node uri
     * @return String the node's module
     */
    public static function getNodeModule($nodeUri)
    {
        $arr = explode(".", $nodeUri);
        if (count($arr) == 2) {
            return $arr[0];
        } else {
            return "";
        }
    }

    /**
     * Gets the node type of a node string
     * @param string $nodeUri the node uri
     * @return String the node type
     */
    public static function getNodeType($nodeUri)
    {
        $arr = explode(".", $nodeUri);
        if (count($arr) == 2) {
            return $arr[1];
        } else {
            return $nodeUri;
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
    public static function &atkGetNode($nodeUri, $init = true, $cache_id = "default", $reset = false)
    {
        global $g_nodeRepository;
        //$nodeUri = strtolower($nodeUri); // classes / directory names should always be in lower-case
        if (!isset($g_nodeRepository[$cache_id][$nodeUri]) || !is_object($g_nodeRepository[$cache_id][$nodeUri]) || $reset) {
            Tools::atkdebug("Constructing a new node $nodeUri ($cache_id)");
            $g_nodeRepository[$cache_id][$nodeUri] = Module::newAtkNode($nodeUri, $init);
        }
        return $g_nodeRepository[$cache_id][$nodeUri];
    }


    /**
     * Retrieves all the registered atkModules
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
    public static function &atkGetModule($moduleName)
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
    public static function &newAtkNode($nodeUri, $init = true)
    {

        $nodeUri = strtolower($nodeUri); // classes / directory names should always be in lower-case
        $module = self::getNodeModule($nodeUri);


        if ($module == "") {
            // No module, use the default instance.
            $module_inst = new Module();
        } else {
            $module_inst = self::atkGetModule($module);
        }
        if (is_object($module_inst)) {
            if (method_exists($module_inst, 'newNode')) {
                $node = $module_inst->newNode($nodeUri);
                if ($init && $node != null) {
                    $node->init();
                }
                return $node;
            } else {
                Tools::atkerror("Module $module does not have newNode function (does it extend from Module?)");
            }
        } else {
            Tools::atkerror("Module $module could not be instantiated.");
        }
        return null;
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


    /**
     * Get/set the status of the readoptimizer.
     * If you need the dataread-functionality of Node but don't need
     * the ui stuff, or the data write stuff, you can turn on the read
     * optimizer, so nodes load faster.
     * If you call this function without parameters (or NULL as param)
     * the optimizer value is not changed, and the function will just
     * return the current setting.
     * If you do specify a parameter, the function will return the
     * OLD setting (so you might reset it to the old value after you're
     * finished with the current node.
     *
     * @param bool $newValue the value of the readOptimizer. true turns the
     *                  optimizer on. Falls turns it off.
     * @return bool The old value of the optimizer setting, if a new
     *                 setting was passed OR
     *                 The current value if no new setting was passed.
     */
    public static function atkReadOptimizer($newValue = null)
    {
        static $s_optimized = false;

        if (!($newValue === null)) { // New value was set
            $oldValue = $s_optimized;
            $s_optimized = $newValue;
            return $oldValue;
        } else {
            return $s_optimized; // Return current value.
        }
    }


    /**
     * @param array $nodeMap
     */
    protected function setNodeMap(array $nodeMap)
    {
        $this->nodeMap = $nodeMap;
    }

    /**
     * Load a module.
     *
     * This method is used to load the modules.
     *
     * @param string $moduleClass The className of the module to load.
     * @param int $flags The module (MF_*) flags that influence how the module is
     *                  loaded.
     */
    public static function module($moduleClass, $flags = 0)
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
