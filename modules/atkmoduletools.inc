<?php
  /**
   * Atk Module Tools
   *
   * A collection of functions which can be used
   * for ATK module support.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$
   *
   */

  // A repository of node instances..
  $g_nodeRepository = array();

  // A repository of module instances..
  $g_moduleRepository = array();

  // registered node action handlers
  $g_nodeHandlers = array();

  /**
   * Gets the node type of a node string
   * @param $node the node string
   * @return the node type
   */
  function getNodeType($node)
  {
    $arr = explode(".", $node);
    if (count($arr) == 2) return $arr[1];
    else return $node;
  }

  /**
   * Gets the module of the node
   * @param $node the node string
   * @return the node's module
   */
  function getNodeModule($node)
  {
    $arr = explode(".", $node);
    if (count($arr) == 2) return $arr[0];
    else return "";
  }

  /**
   * Get an instance of a node. If an instance doesn't exist, it is created.
   * @param $node the node string
   * @param $init initialize the node?
   * @return the created instance
   */
  function &getNode($node, $init=TRUE)
  {
    global $g_nodeRepository;
    if (!is_object($g_nodeRepository[$node]))
    {
      atkdebug("Constructing a new node - $node");
      $g_nodeRepository[$node] = &newNode($node, $init);
    }
    else
    {
      atkdebug("Returning cached node instance - $node");
    }
    return $g_nodeRepository[$node];
  }

  function &getModule($modname)
  {
    global $g_moduleRepository;
    if (!is_object($g_moduleRepository[$modname]))
    {
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
        if (function_exists("missing_module_handler"))
        {
          atkdebug("Calling missing_module_handler() for module $modname");
          $mod = missing_module_handler($modname);
        }
        else
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
    else
    {
      atkdebug("Returning cached module instance - $modname");
    }
    return $g_moduleRepository[$modname];
  }

  function nodeFile($node)
  {
    global $g_modules, $config_atkroot, $config_classroot, $config_module_path;

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
      if (is_array($g_modules)&&in_array($module, array_keys($g_modules)))
      {

        $file = $g_modules[$module]."/class.$type.inc";
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
   * @param $node the node type
   * @param $init initialize the node?
   * @return new node object
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
      $module_inst = &getModule(getNodeModule($node));
    }
    if (is_object($module_inst))
    {
      $node = &$module_inst->newNode($node);
      if ($init && $node != NULL) $node->init();
      return $node;
    }
    return NULL;
  }

  /**
   * Return the physical directory of a module..
   */
  function moduleDir($module)
  {
    global $g_modules;
    $dir = $g_modules[$module];
    if ($dir=="") return "";
    else if (substr($dir,-1)!='/') return $dir."/";
    return $dir;
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

  function moduleExists($module)
  {
    global $g_modules;
    return (is_array($g_modules)&&in_array($module, array_keys($g_modules)));
  }

  /**
   * Returns a registered node action handler.
   * @param $node the name of the node
   * @param $action the node action
   * @return handler functionname or
   *         NULL if no handler exists for the specified action
   */
  function getNodeHandler($node, $action)
  {
    global $g_nodeHandler;
    if (isset($g_nodeHandler[$node][$action])) return $g_nodeHandler[$node][$action];
    elseif (isset($g_nodeHandler["*"][$action])) return $g_nodeHandler["*"][$action];
    else return NULL;
  }

  /**
   * Registers a new node action handler.
   * @param $node the name of the node (* matches all)
   * @param $action the node action
   * @param $handler handler functionname
   * @return true if there is no known handler
   */
  function registerNodeHandler($node, $action, $handler)
  {
    global $g_nodeHandler;
    if (isset($g_nodeHandler[$node][$action])) return FALSE;
    else $g_nodeHandler[$node][$action] = $handler;
    return TRUE;
  }
 
  /**
   * Perform a member function on all active modules, and return the 
   * collective result.
   * 
   * @param $function The name of the module member function to be called.
   *                  The function does not have to exist for all modules, as
   *                  atkHarvestModules will check if it exists before it
   *                  makes the call.
   * @param $param    Parameter to be passed to all functions. It is only
   *                  possible to pass zero or one parameter.
   * @param $associative If true, return is an associative
   *                     array with the results indexed by
   *                     modulename. If false, results are
   *                     put together in one array.
   *
   * @example $menuitems = atkHarvestModules("getStuff");
   *          This will return the result of the getStuff
   *          calls for all modules in a single array, 
   */
  function atkHarvestModules($function, $param="", $associative=false)
  {
    global $g_modules;
    $total = array();
    foreach ($g_modules as $modname => $modpath)
    {
      $module = &getModule($modname);      
      if (is_object($module) && method_exists($module, $function))
      {          
        $res = $module->$function($param);
        if (!empty($res))
        {
          if ($associative)
          {
            $total[$modname] = $module->$function();
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
  
?>
