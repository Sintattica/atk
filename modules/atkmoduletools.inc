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
   * @param $reuse if set to true, the node instance is treated as singleton.
   *               if false, a new node is created.
   * @return the created instance
   */
  function &getNode($node)
  {
    global $g_nodeRepository;
    if (!is_object($g_nodeRepository[$node]))
    {
      atkdebug("Constructing a new node - $node");
      $g_nodeRepository[$node] = & newNode($node);
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
        // construct dummy module
        $mod = new atkmodule($modname);
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
   * @return new node object
   */
  function &newNode($node)
  {
    $module = getNodeModule($node);
    $module_inst = &getModule(getNodeModule($node));
    $node = &$module_inst->newNode($node);
    return $node;
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
?>