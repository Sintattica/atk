<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage modules
 *
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 6323 $
 * $Id$
 */


/**
 * @internal Includes, global definitions etc.
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


/**
 * The atkModule abstract base class.
 *
 * All modules in an ATK application should derive from this class, and
 * override the methods of this abstract class as they see fit.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage modules
 * @abstract
 */
class atkModule
{
    /**
     * We keep track if the node currently being instantiated is an
     * overloader or not.
     *
     * @var boolean
     */
    static $s_isOverloader = false;
    var $m_name;

    /**
     * Constructor. The module needs to register it's nodes
     * overhere, create its menuitems etc.
     * @param String $name The name of the module.
     */
    function atkModule($name = "")
    {
        $this->m_name = $name;
    }

    /**
     * Register nodes with their supported actions. Can be used
     * for security etc.
     */
    function getNodes()
    {

    }

    /**
     * Returns an array with filenames of attributes that need to be included
     * in order to let this module work properly.
     * @return array with attribute filenames
     */
    function getAttributes()
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
     * Create a new menu item, optionally configuring access control.  This
     * function can also be used to create separators, submenus and submenu items.
     *
     * @param String $name The menuitem name. The name that is displayed in the
     *                     userinterface can be influenced by putting
     *                     "menu_something" in the language files, where 'something'
     *                     is equal to the $name parameter.
     *                     If "-" is specified as name, the item is a separator.
     *                     In this case, the $url parameter should be empty.
     * @param String $url The url to load in the main application area when the
     *                    menuitem is clicked. If set to "", the menu is treated
     *                    as a submenu (or a separator if $name equals "-").
     *                    The dispatch_url() method is a useful function to
     *                    pass as this parameter.
     * @param String $parent The parent menu. If omitted or set to "main", the
     *                       item is added to the main menu.
     * @param mixed $enable This parameter supports the following options:
     *                      1: menuitem is always enabled
     *                      0: menuitem is always disabled
     *                         (this is useful when you want to use a function
     *                         call to determine when a menuitem should be
     *                         enabled. If the function returns 1 or 0, it can
     *                         directly be passed to this method in the $enable
     *                         parameter.
     *                      array: when an array is passed, it should have the
     *                             following format:
     *                             array("node","action","node","action",...)
     *                             When an array is passed, the menu checks user
     *                             privileges. If the user has any of the
     *                             node/action privileges, the menuitem is
     *                             enabled. Otherwise, it's disabled.
     * @param int $order The order in which the menuitem appears. If omitted,
     *                   the items appear in the order in which they are added
     *                   to the menu, with steps of 100. So, if you have a menu
     *                   with default ordering and you want to place a new
     *                   menuitem at the third position, pass 250 for $order.
     */
    function menuitem($name = "", $url = "", $parent = "main", $enable = 1, $order = 0)
    {
        /* call basic menuitem */
        if (empty($parent))
            $parent = 'main';
        atkTools::menuitem($name, $url, $parent, $enable, $order, $this->m_name);
    }

    /**
     * This method can be used to return an array similar to the menu array
     * but with links to (a) preference(s) page(s) for this module. The items
     * that will be returned have to be added with the preferencelink() method.
     * @return array with preference links for this module
     */
    function getPreferenceLinks()
    {

    }

    /**
     * This method is similar to the getPreferenceLinks() method but instead
     * will return links to (an) admin page(s) for this module. The array which
     * will be returned have to be created with the adminlink() method.
     * @return array with admin links for this module
     */
    function getAdminLinks()
    {

    }

    /**
     * Returns the node overloader if it exists. Else it
     * will just return the module/node name of the given node.
     * @param string $node module/node string
     * @return string (overloader) module/node string
     */
    function nodeOverloader($node)
    {
        global $g_overloaders;

        /* overloader check */
        if (!empty($g_overloaders[$node])) {
            atkTools::atkdebug("Using overloader '" . $g_overloaders[$node] . "' for class '" . $node . "'");
            self::$s_isOverloader = true;
            $node = self::newAtkNode($g_overloaders[$node], FALSE);
            self::$s_isOverloader = false;
            return $node;
        } /* no overloader */ else {
            return null;
        }
    }

    /**
     * Returns the node file for the given node.
     *
     * @param string $node the node type
     * @return string node filename
     */
    function getNodeFile($node)
    {
        global $config_classroot, $config_module_path;
        $modules = self::atkGetModules();

        /* module and type */
        $module = atkModule::getNodeModule($node);
        $type = atkModule::getNodeType($node);
        $file = "class.$type.inc";

        /* filename */
        if (empty($module)) {
            $file = $config_classroot . "class.$type.inc";
        } else {
            if (is_array($modules) && in_array($module, array_keys($modules))) {
                if (file_exists("{$modules[$module]}/nodes/class.{$type}.inc")) {
                    $file = "{$modules[$module]}/nodes/class.{$type}.inc";
                } else {
                    $file = "{$modules[$module]}/class.{$type}.inc";
                }
            } else {
                atkTools::atkdebug("Couldn't find node '$node' in module '$module'. Trying default module path.");
                $file = $config_module_path . "/" . $module . "/class.$type.inc";
            }
        }
        return $file;
    }

    /**
     * Returns the fixture path for the given fixture.
     *
     * @param string $fixture <module.fixture> string
     * @return string path to fixture without extension
     */
    function getFixturePath($fixture)
    {
        $module = self::getNodeModule($fixture);
        $fixture = self::getNodeType($fixture);
        $path = self::moduleDir($module) . 'testcases/fixtures/' . $fixture;
        return $path;
    }

    /**
     * Construct a new node. A module can override this method for it's own nodes.
     * @param atkNode $node the node type
     * @return new node object
     */
    function &newNode($node)
    {
        global $config_atkroot;

        /* include the base file */
        atkTools::atkimport("atk.atknode");

        $corporate_base = atkConfig::getGlobal("corporate_node_base");
        if ($corporate_base != "") {
            atkTools::atkimport($corporate_base);
        }

        /* check for file */
        $file = $this->getNodeFile($node);
        if (!file_exists($file)) {
            atkTools::atkimport("atk.utils.atkclassloader");
            $res = atkClassLoader::invokeFromString(atkConfig::getGlobal("missing_class_handler"), array(array("node" => $node, "module" => $this->m_name)));
            if ($res !== false) {
                return $res;
            } else {
                atkTools::atkerror("Cannot create node, because a required file ($file) does not exist!");
                return NULL;
            }
        }

        /* include file */
        include_once($file);

        /* module */
        $module = self::getNodeModule($node);

        // set the current module scope, this will be retrieved in atkNode
        // to set it's $this->m_module instance variable in an early stage
        if (!self::$s_isOverloader) {
            self::setModuleScope($module);
        }

        /* now that we have included the node source file, we check
         * for overloaders (because overloaders might need the included file!)
         */
        $overloader = &$this->nodeOverloader($node);
        if ($overloader != NULL) {
            $overloader->m_module = $module;

            if (!self::$s_isOverloader) {
                self::resetModuleScope();
            }

            return $overloader;
        }

        /* initialize node and return */
        $type = self::getNodeType($node);
        $node = new $type();
        $node->m_module = $module;

        if (!self::$s_isOverloader) {
            self::resetModuleScope();
        }

        return $node;
    }

    /**
     * Set current module scope.
     *
     * @param string $module current module
     * @static
     */
    function setModuleScope($module)
    {
        global $g_atkModuleScope;
        $g_atkModuleScope = $module;
    }

    /**
     * Returns the current module scope.
     *
     * @return string current module
     * @static
     */
    function getModuleScope()
    {
        global $g_atkModuleScope;
        return $g_atkModuleScope;
    }

    /**
     * Resets the current module scope.
     *
     * @static
     */
    function resetModuleScope()
    {
        self::setModuleScope(null);
    }

    /**
     * Checks if a certain node exists for this module.
     * @param string $node the node type
     * @return node exists?
     */
    function nodeExists($node)
    {
        // check for file
        $file = $this->getNodeFile($node);
        return file_exists($file);
    }

    /**
     * Get the modifier functions for this node
     *
     * @param atkNode $node
     * @return array Array with modifier function names
     */
    function getModifierFunctions(&$node)
    {
        return array($node->m_type . "_modifier", str_replace(".", "_", $node->atknodetype()) . "_modifier");
    }

    /**
     * Modifies the given node
     *
     * @param atkNode $node Node to be modified
     */
    function modifier(&$node)
    {
        // Determine the modifier name and existance for modifiers that modify any node having the this node's type in any module
        $specificmodifiers = $this->getModifierFunctions($node);

        // Set the number of applied modifiers to zero
        $appliedmodifiers = 0;

        // Loop through the possible modifiers and apply them if found
        foreach ($specificmodifiers as $modifiername) {
            // If the modifiers is found
            if (method_exists($this, $modifiername)) {
                // Add a debug line so we know, the modifier is applied
                atkTools::atkdebug(sprintf("Applying modifier %s from module %s to node %s", $modifiername, $this->m_name, $node->m_type));

                // Apply the modifier
                $node->m_modifier = $this->m_name;
                $this->$modifiername($node);
                $node->m_modifier = "";

                // Increase the number of applied modifiers
                $appliedmodifiers++;
            }
        }

        // If none of the modifiers was found, add a warning to the debug log
        if ($appliedmodifiers == 0)
            atkTools::atkdebug(sprintf("Failed to apply modifier function %s from module %s to node %s; modifier function not found", implode(" or ", $specificmodifiers), $this->m_name, $node->m_type), DEBUG_WARNING);
    }


    /**
     * Gets the node type of a node string
     * @param String $node the node name
     * @return String the node type
     */
    public static function getNodeType($node)
    {
        $arr = explode(".", $node);
        if (count($arr) == 2)
            return $arr[1];
        else
            return $node;
    }

    /**
     * Gets the module of the node
     * @param String $node the node name
     * @return String the node's module
     */
    public static function getNodeModule($node)
    {
        $arr = explode(".", $node);
        if (count($arr) == 2)
            return $arr[0];
        else
            return "";
    }


    /**
     * Get an instance of a node. If an instance doesn't exist, it is created.  Note that nodes
     * are cached (unless $reset is true); multiple requests for the same node will return exactly
     * the same node object.
     *
     * @param String $node The node string
     * @param bool $init Initialize the node?
     * @param String $cache_id The cache id in the node repository
     * @param bool $reset Whether or not to reset the particular node in the repository
     * @return atkNode the node
     */
    public static function &atkGetNode($node, $init = TRUE, $cache_id = "default", $reset = false)
    {
        global $g_nodeRepository;
        $node = strtolower($node); // classes / directory names should always be in lower-case
        if (!isset($g_nodeRepository[$cache_id][$node]) || !is_object($g_nodeRepository[$cache_id][$node]) || $reset) {
            atkTools::atkdebug("Constructing a new node $node ($cache_id)");
            $g_nodeRepository[$cache_id][$node] = &self::newAtkNode($node, $init);
        }
        return $g_nodeRepository[$cache_id][$node];
    }

    /**
     * Replace, at runtime, the in-memory instance of a node.
     *
     * This is useful to replace nodes with mocks for testing purposes.
     *
     * @param String $nodename The full name of the node (module.nodename)
     * @param atkNode $node The node instance to replace the current one
     * @param String $cache_id If set, replaces only the instance with a certain
     *                         cache_id
     * @return atkNode The current node, useful to restore afterwards. Can be
     *                 NULL.
     */
    public static function &atkSetNode($nodename, &$node, $cache_id = "default")
    {
        global $g_nodeRepository;
        $nodename = strtolower($nodename); // classes / directory names should always be in lower-case
        $org = &$g_nodeRepository[$cache_id][$nodename];
        $g_nodeRepository[$cache_id][$nodename] = &$node;
        return $org;
    }

    /**
     * Retrieves all the registered atkModules
     *
     * @return Array with modules
     */
    public static function atkGetModules()
    {
        global $g_modules;
        return $g_modules;
    }

    /**
     * Retrieve the atkModule with the given name.
     *
     * @param String $modname The name of the module
     * @return atkModule An instance of the atkModule
     */
    public static function &atkGetModule($modname)
    {
        global $g_moduleRepository;

        atkTools::atkimport("atk.atknode");

        if (!isset($g_moduleRepository[$modname]) || !is_object($g_moduleRepository[$modname])) {
            $corporate_base = atkConfig::getGlobal("corporate_module_base");
            if ($corporate_base != "") {
                atkTools::atkimport($corporate_base);
            }

            $filename = self::moduleDir($modname) . "module.inc";
            if (file_exists($filename)) {
                include_once($filename);
            } else {
                atkTools::atkdebug("Couldn't find module.inc for module '$modname' in '" . self::moduleDir($modname) . "'");
            }

            atkTools::atkdebug("Constructing a new module - $modname");
            if (class_exists("mod_" . $modname)) {
                $realmodname = "mod_" . $modname;
                $mod = new $realmodname($modname);
            } else if (class_exists($modname)) {
                atkTools::atkdebug("Warning: Deprecated use of short modulename '$modname'. Class in module.inc should be renamed to 'mod_$modname'.");
                $mod = new $modname();
            } else {
                atkTools::atkimport("atk.utils.atkclassloader");
                $mod = atkClassLoader::invokeFromString(atkConfig::getGlobal("missing_module_handler"), array(array("module" => $modname)));
                if ($mod === false) {
                    // Changed by Ivo: This used to construct a dummy module, so
                    // modules could exist that didn't have a module.inc file.
                    // We no longer support this (2003-01-11)
                    $mod = NULL;
                    atkTools::atkdebug("Warning: module $modname does not exist");
                }
            }
            $g_moduleRepository[$modname] = $mod;
        }
        return $g_moduleRepository[$modname];
    }

    /**
     * Construct a new node
     * @param String $node the node type
     * @param bool $init initialize the node?
     * @return atkNode new node object
     */
    public static function &newAtkNode($node, $init = TRUE)
    {
        $node = strtolower($node); // classes / directory names should always be in lower-case
        $module = self::getNodeModule($node);

        if ($module == "") {
            // No module, use the default instance.
            $module_inst = new atkModule();
        } else {
            $module_inst = self::atkGetModule($module);
        }
        if (is_object($module_inst)) {
            if (method_exists($module_inst, 'newNode')) {
                $node = $module_inst->newNode($node);
                if ($init && $node != NULL)
                    $node->init();
                return $node;
            } else
                atkTools::atkerror("Module $module does not have newNode function (does it extend from atkModule?)");
        } else
            atkTools::atkerror("Module $module could not be instantiated.");
        return NULL;
    }

    /**
     * Checks if a certain node exists.
     * @param String $node the node type
     * @return bool node exists?
     */
    public static function atkNodeExists($node)
    {
        static $existence = array();
        if (array_key_exists($node, $existence))
            return $existence[$node];

        $module = self::getNodeModule($node);
        if ($module == "")
            $module = new atkModule();
        else
            $module = self::atkGetModule(self::getNodeModule($node));

        $exists = is_object($module) && $module->nodeExists($node);
        $existence[$node] = $exists;
        atkTools::atkdebug("Node $node exists? " . ($exists ? 'yes' : 'no'));

        return $exists;
    }

    /**
     * Return the physical directory of a module..
     * @param String name of the module.
     * @return String The path to the module.
     */
    public static function moduleDir($module)
    {
        $modules = self::atkGetModules();
        if (isset($modules[$module])) {
            $dir = $modules[$module];
            if (substr($dir, -1) != '/')
                return $dir . "/";
            return $dir;
        }
        return "";
    }


    /**
     * Check wether a module is installed
     * @param String $module The modulename.
     * @return bool True if it is, false otherwise
     */
    public static function moduleExists($module)
    {
        $modules = self::atkGetModules();
        return (is_array($modules) && in_array($module, array_keys($modules)));
    }


    /**
     * Returns a registered node action handler.
     * @param $node the name of the node
     * @param $action the node action
     * @return handler functionname or object (is_subclass_of atkActionHandler) or
     *         NULL if no handler exists for the specified action
     */
    public static function &atkGetNodeHandler($node, $action)
    {
        global $g_nodeHandlers;
        if (isset($g_nodeHandlers[$node][$action]))
            $handler = $g_nodeHandlers[$node][$action];
        elseif (isset($g_nodeHandlers["*"][$action]))
            $handler = $g_nodeHandlers["*"][$action];
        else
            $handler = NULL;
        return $handler;
    }

    /**
     * Registers a new node action handler.
     * @param String $node the name of the node (* matches all)
     * @param String $action the node action
     * @param String /atkActionHandler $handler handler functionname or object (is_subclass_of atkActionHandler)
     * @return bool true if there is no known handler
     */
    public static function atkRegisterNodeHandler($node, $action, $handler)
    {
        global $g_nodeHandlers;
        if (isset($g_nodeHandlers[$node][$action]))
            return FALSE;
        else
            $g_nodeHandlers[$node][$action] = $handler;
        return TRUE;
    }



    /**
     * Perform a member function on all active modules, and return the
     * collective result.
     *
     * <b>Example:</b>
     * <code>
     *  $menuitems = atkHarvestModules("getStuff");
     * </code>
     * This will return the result of the getStuff calls for all modules in a
     * single array.
     *
     * @param String $function The name of the module member function to be
     *                         called. The function does not have to exist for
     *                         all modules, as atkHarvestModules will check if
     *                         it exists before it makes the call.
     * @param mixed $param Parameter to be passed to all functions. It is only
     *                     possible to pass zero or one parameter.
     * @param boolean $associative If true, return is an associative array with
     *                             the results indexed by modulename. If false,
     *                             results are put together in one array.
     * @return array The result of the harvest.
     */
    public static function atkHarvestModules($function, $param = "", $associative = false)
    {
        $modules = self::atkGetModules();
        $modulekeys = array_keys($modules);
        $total = array();

        foreach ($modulekeys as $modname) {
            $module = self::atkGetModule($modname);
            if (is_object($module) && method_exists($module, $function)) {
                $res = $module->$function($param);
                if (!empty($res)) {
                    if ($associative) {
                        $total[$modname] = $res;
                    } else {
                        if (is_array($res)) {
                            $total = array_merge($total, $res);
                        } else {
                            $total[] = $res;
                        }
                    }
                }
            }
        }
        return $total;
    }

    /**
     * Get/set the status of the readoptimizer.
     * If you need the dataread-functionality of atkNode but don't need
     * the ui stuff, or the data write stuff, you can turn on the read
     * optimizer, so nodes load faster.
     * If you call this function without parameters (or NULL as param)
     * the optimizer value is not changed, and the function will just
     * return the current setting.
     * If you do specify a parameter, the function will return the
     * OLD setting (so you might reset it to the old value after you're
     * finished with the current node.
     *
     * @param $newvalue New value of the readoptimizer. true turns the
     *                  optimizer on. Falls turns it off.
     * @return boolean The old value of the optimizer setting, if a new
     *                 setting was passed OR
     *                 The current value if no new setting was passed.
     */
    public static function atkReadOptimizer($newvalue = NULL)
    {
        static $s_optimized = false;

        if (!($newvalue === NULL)) { // New value was set
            $oldvalue = $s_optimized;
            $s_optimized = $newvalue;
            return $oldvalue;
        } else {
            return $s_optimized; // Return current value.
        }
    }

    /**
     * Preload the module
     * @see atkPreloadModules()
     * @param String $modname The modulename
     * @param String $modpath The path to the module.
     */
    public static function atkPreloadModule($modname, $modpath)
    {
        global $g_modifiers, $g_overloaders, $g_nodeListeners;

        $preload = "$modpath/module_preload.inc";
        $module = "$modpath/module.inc";
        $filename = file_exists($preload) ? $preload : $module;

        // no module file exists
        if (!file_exists($filename)) {
            atkTools::atkdebug("Couldn't find module_preload.inc or module.inc for module '$modname' in '$modpath'");
            return;
        }

        if ($filename != $preload) {
            atkTools::atkdebug("Loading module - $modname");
            $corporate_base = atkConfig::getGlobal("corporate_module_base");
            if ($corporate_base != "") {
                atkTools::atkimport($corporate_base);
            }
        }

        // the include file may specify modifiers.
        $modifiers = array();
        $overloaders = array();
        $listeners = array();
        include_once($filename);

        for ($i = 0, $_i = count($modifiers); $i < $_i; $i++)
            $g_modifiers[$modifiers[$i]][] = $modname;

        if (count($overloaders) > 0)
            $g_overloaders = array_merge($g_overloaders, $overloaders);

        if (count($listeners) > 0)
            $g_nodeListeners = array_merge_recursive($g_nodeListeners, $listeners);
    }

    /**
     * Preloads all modules. If a module_preload.inc file exists in the
     * module directory, this file will be included. If no module_preload.inc
     * file exists in the module directory the module.inc file will be used
     * instead (to remain backwards compatible).
     */
    public static function atkPreloadModules()
    {
        global $g_moduleflags;
        $modules = self::atkGetModules();

        foreach ($modules as $modname => $modpath) {
            if ((!isset($g_moduleflags[$modname])) || (!atkTools::hasFlag($g_moduleflags[$modname], MF_NO_PRELOAD))) {
                self::atkPreloadModule($modname, $modpath);
            }
        }
    }
}

