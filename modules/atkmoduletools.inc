<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage modules
   *
   * Collection of utility methods for use with modules and nodes.
   * @todo Move the global methods to classes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
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
  $g_nodeRepository = array();

  /**
   * A repository of module instances..
   * @access private
   * @var Array
   */
  $g_moduleRepository = array();

  /**
   * registered node action handlers
   * @access private
   * @var Array
   */
  $g_nodeHandlers = array();

  /**
   * registered node listeners
   * @access private
   * @var Array
   */
  $g_nodeListeners = array();

  /**
   * registered node controllers
   * @access private
   * @var Array
   */
  $g_nodeControllers = array();

  /**
   * Gets the node type of a node string
   * @param String $node the node name
   * @return String the node type
   */
  function getNodeType($node)
  {
    $arr = explode(".", $node);
    if (count($arr) == 2) return $arr[1];
    else return $node;
  }

  /**
   * Gets the module of the node
   * @param String $node the node name
   * @return String the node's module
   */
  function getNodeModule($node)
  {
    $arr = explode(".", $node);
    if (count($arr) == 2) return $arr[0];
    else return "";
  }

  /**
   * Get an instance of a node. If an instance doesn't exist, it is created.
   * @deprecated Use atkGetNode instead.
   * @param String $node     The node string
   * @param bool   $init     Initialize the node?
   * @param String $cache_id The cache id in the node repository
   * @param bool   $reset    Wether or not to reset the particulair node in the repository
   * @return atkNode the created instance
   */
  function &getNode($node, $init=TRUE, $cache_id="default", $reset=false)
  {
    return atkGetNode($node, $init, $cache_id, $reset);
  }

  /**
   * Get an instance of a node. If an instance doesn't exist, it is created.
   * @param String $node     The node string
   * @param bool   $init     Initialize the node?
   * @param String $cache_id The cache id in the node repository
   * @param bool   $reset    Wether or not to reset the particulair node in the repository
   * @return atkNode the created instance
   */
  function &atkGetNode($node, $init=TRUE, $cache_id="default", $reset=false)
  {
    global $g_nodeRepository;
    if (!isset($g_nodeRepository[$cache_id][$node])||!is_object($g_nodeRepository[$cache_id][$node]) || $reset)
    {
      atkdebug("Constructing a new node - $node");
      $g_nodeRepository[$cache_id][$node] = &newNode($node, $init);
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
  function &atkSetNode($nodename, &$node, $cache_id="default")
  {
    global $g_nodeRepository;
    $org = &$g_nodeRepository[$cache_id][$nodename];
    $g_nodeRepository[$cache_id][$nodename] = &$node;
    return $org;
  }

  /**
   * Retrieves all the registered atkModules
   *
   * @return Array with modules
   */
  function atkGetModules()
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
  function &atkGetModule($modname)
  {
    global $g_moduleRepository;

    atkimport("atk.atknode");

    if (!isset($g_moduleRepository[$modname])||!is_object($g_moduleRepository[$modname]))
    {
      atkimport("atk.modules.atkmodule");
      $corporate_base = atkconfig("corporate_module_base");
      if ($corporate_base!="")
      {
        atkimport($corporate_base);
      }

      $filename = moduleDir($modname)."module.inc";
      if (file_exists($filename))
      {
        include_once($filename);
      }
      else
      {
        atkdebug("Couldn't find module.inc for module '$modname' in '".moduleDir($modname)."'");
      }

      atkdebug("Constructing a new module - $modname");
      if (class_exists("mod_".$modname))
      {
        $realmodname = "mod_".$modname;
        $mod = new $realmodname($modname);
      }
      else if (class_exists($modname))
      {
        atkdebug("Warning: Deprecated use of short modulename '$modname'. Class in module.inc should be renamed to 'mod_$modname'.");
        $mod = new $modname();
      }
      else
      {
        atkimport("atk.utils.atkclassloader");
        $mod = atkClassLoader::invokeFromString(atkconfig("missing_module_handler"), array("module"=>$modname));
        if ($mod===false)
        {
          // Changed by Ivo: This used to construct a dummy module, so
          // modules could exist that didn't have a module.inc file.
          // We no longer support this (2003-01-11)
          $mod = NULL;
          atkdebug("Warning: module $modname does not exist");
        }
      }
      $g_moduleRepository[$modname] = $mod;
    }
    return $g_moduleRepository[$modname];
  }

  /**
   * Retrieve the atkModule with the given name.
   *
   * @deprecated Use atkGetModule instead.
   * @param String $modname The name of the module
   * @return atkModule an instance of the atkModule
   */
  function getModule($modname)
  {
    return atkGetModule($modname);
  }

  /**
   * Retrieve the full filename containing the given node.
   *
   * @param String $node The name of the node (nodename or module.nodename)
   * @return String The filename
   */
  function nodeFile($node)
  {
    global $config_classroot, $config_module_path;
    $modules = atkGetModules();

    /* module and type */
    $module = getNodeModule($node);
    $type = getNodeType($node);
    $file = "class.$type.inc";

    /* filename */
    if (empty($module))
    {
      $file = $config_classroot."class.$type.inc";
    }
    else
    {
      if (is_array($modules)&&in_array($module, array_keys($modules)))
      {

        $file = $modules[$module]."/class.$type.inc";
      }
      else
      {
        atkdebug("Couldn't find node '$node' in module '$module'. Trying default module path.");
        $file = $config_module_path."/".$module."/class.$type.inc";
      }
    }
    return $file;
  }

  /**
   * Construct a new node
   * @param String $node the node type
   * @param bool $init initialize the node?
   * @return atkNode new node object
   */
  function &newNode($node, $init=TRUE)
  {
    $module = getNodeModule($node);

    if ($module=="")
    {
      // No module, use the default instance.
      $module_inst = &new atkModule();
    }
    else
    {
      $module_inst = &getModule($module);
    }
    if (is_object($module_inst))
    {
      if (method_exists($module_inst,'newNode'))
      {
        $node = &$module_inst->newNode($node);
        if ($init && $node != NULL) $node->init();
        return $node;
      }
      else atkerror("Module $module does not have newNode function (does it extend from atkModule?)");
    }
    else atkerror("Module $module could not be instantiated.");
    return NULL;
  }

  /**
   * Checks if a certain node exists.
   * @param String $node the node type
   * @return bool node exists?
   */
  function nodeExists($node)
  {
    static $existence = array();
    if (array_key_exists($node, $existence))
      return $existence[$node];

    $module = getNodeModule($node);
    if ($module == "") $module = new atkModule();
    else $module = getModule(getNodeModule($node));

    $exists = is_object($module) && $module->nodeExists($node);
    $existence[$node] = $exists;
    atkdebug("Node $node exists? ".($exists ? 'yes' : 'no'));

    return $exists;
  }

  /**
   * Return the physical directory of a module..
   * @param String name of the module.
   * @return String The path to the module.
   */
  function moduleDir($module)
  {
    $modules = atkGetModules();
    if (isset($modules[$module]))
    {
      $dir = $modules[$module];
      if (substr($dir,-1)!='/') return $dir."/";
      return $dir;
    }
    return "";
  }

  /**
   * Creates an URL to a file within the modules directory
   * @param $module the module name
   * @param $file the directory/filename
   * @return URL to file within modules directory
   */
  function module_url($module, $file)
  {
    global $config_module_path;
    return "$config_module_path/$module/$file";
  }

  /**
   * Check wether a module is installed
   * @param String $module The modulename.
   * @return bool True if it is, false otherwise
   */
  function moduleExists($module)
  {
    $modules = atkGetModules();
    return (is_array($modules)&&in_array($module, array_keys($modules)));
  }

  /**
   * Returns a registered node action handler.
   * @deprecated Use atkGetNodeHandler instead
   * @param $node the name of the node
   * @param $action the node action
   * @return handler functionname or object (is_subclass_of atkActionHandler) or
   *         NULL if no handler exists for the specified action
   */
  function &getNodeHandler($node, $action)
  {
    return atkGetNodeHandler($node, $action);
  }

  /**
   * Registers a new node action handler.
   * @deprecated Use atkRegisterNodeHandler instead
   * @param $node the name of the node (* matches all)
   * @param $action the node action
   * @param $handler handler functionname or object (is_subclass_of atkActionHandler)
   * @return true if there is no known handler
   */
  function registerNodeHandler($node, $action, &$handler)
  {
    return atkRegisterNodeHandler($node, $action, $handler);
  }

  /**
   * Returns a registered node action handler.
   * @param $node the name of the node
   * @param $action the node action
   * @return handler functionname or object (is_subclass_of atkActionHandler) or
   *         NULL if no handler exists for the specified action
   */
  function &atkGetNodeHandler($node, $action)
  {
    global $g_nodeHandlers;
    if (isset($g_nodeHandlers[$node][$action])) $handler = $g_nodeHandlers[$node][$action];
    elseif (isset($g_nodeHandlers["*"][$action])) $handler = $g_nodeHandlers["*"][$action];
    else $handler = NULL;
    return $handler;
  }

  /**
   * Registers a new node action handler.
   * @param String $node the name of the node (* matches all)
   * @param String $action the node action
   * @param String/atkActionHandler $handler handler functionname or object (is_subclass_of atkActionHandler)
   * @return bool true if there is no known handler
   */
  function atkRegisterNodeHandler($node, $action, &$handler)
  {
    global $g_nodeHandlers;
    if (isset($g_nodeHandlers[$node][$action])) return FALSE;
    else $g_nodeHandlers[$node][$action] = &$handler;
    return TRUE;
  }


  /**
   * Registers a new node action handler.
   *
   * @param string $node the name of the node (* matches all)
   * @param string $action the node action
   * @param string $class node handler class ATK path
   *
   * @return bool true if there is no known handler
   */
  function atkRegisterNodeHandlerClass($node, $action, $class)
  {
    atkRegisterNodeHandler($node, $action, $class);
  }

  /**
   * Returns a registered node controller.
   * @param String $node the name of the node
   * @return object (is_subclass_of atkController)
   *         NULL if no controller exists for the specified node
   */
  function &atkGetNodeController($node)
  {
    global $g_nodeControllers;
    if (isset($g_nodeControllers[$node])) return $g_nodeControllers[$node];
    elseif (isset($g_nodeControllers["*"])) return $g_nodeControllers["*"];
    else return NULL;
  }

  /**
   * Registers a new node controller.
   * @param String $node the name of the node (* matches all)
   * @param atkController $controller object (is_subclass_of atkController)
   * @return bool true if there is no known handler
   */
  function atkRegisterNodeController($node, &$controller)
  {
    global $g_nodeControllers;
    if (isset($g_nodeControllers[$node])) return FALSE;
    else $g_nodeControllers[$node] = &$controller;
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
  function atkHarvestModules($function, $param="", $associative=false)
  {
    $modules = atkGetModules();
    $modulekeys = array_keys($modules);
    $total = array();

    foreach ($modulekeys as $modname)
    {
      $module = &getModule($modname);
      if (is_object($module) && method_exists($module, $function))
      {
        $res = $module->$function($param);
        if (!empty($res))
        {
          if ($associative)
          {
            $total[$modname] = $res;
          }
          else
          {
            if (is_array($res))
            {
              $total = array_merge($total, $res);
            }
            else
            {
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
  function atkReadOptimizer($newvalue=NULL)
  {
    static $s_optimized=false;

    if (!($newvalue===NULL)) // New value was set
    {
      $oldvalue = $s_optimized;
      $s_optimized=$newvalue;
      return $oldvalue;
    }
    else
    {
      return $s_optimized; // Return current value.
    }
  }

  /**
   * Preload the module
   * @see atkPreloadModules()
   * @param String $modname The modulename
   * @param String $modpath The path to the module.
   */
  function atkPreloadModule($modname, $modpath)
  {
    global $g_modifiers, $g_overloaders, $g_nodeListeners;

    $preload = "$modpath/module_preload.inc";
    $module = "$modpath/module.inc";
    $filename = file_exists($preload) ? $preload : $module;

    // no module file exists
    if (!file_exists($filename))
    {
      atkdebug("Couldn't find module_preload.inc or module.inc for module '$modname' in '$modpath'");
      return;
    }

    if ($filename != $preload)
    {
      atkdebug("Loading module - $modname");
      atkimport("atk.modules.atkmodule");
      $corporate_base = atkconfig("corporate_module_base");
      if ($corporate_base!="")
      {
        atkimport($corporate_base);
      }
    }

    // the include file may specify modifiers.
    $modifiers = array();
    $overloaders = array();
    $listeners = array();
    include_once($filename);

    for ($i=0, $_i = count($modifiers); $i < $_i; $i++)
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
  function atkPreloadModules()
  {
    global $g_moduleflags;
    $modules = atkGetModules();

    foreach ($modules as $modname => $modpath)
    {
      if ((!isset($g_moduleflags[$modname])) || (!hasFlag($g_moduleflags[$modname], MF_NO_PRELOAD)))
      {
        atkPreloadModule($modname, $modpath);
      }
    }
  }
?>