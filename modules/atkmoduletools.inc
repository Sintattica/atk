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
   * @param String $node     The node string
   * @param bool   $init     Initialize the node?
   * @param String $cache_id The cache id in the node repository
   * @param bool   $reset    Wether or not to reset the particulair node in the repository
   * @return the created instance
   */
  function &getNode($node, $init=TRUE, $cache_id="default", $reset=false)
  {
    global $g_nodeRepository;
    if (!isset($g_nodeRepository[$cache_id][$node])||!is_object($g_nodeRepository[$cache_id][$node]) || $reset)
    {
      atkdebug("Constructing a new node - $node");
      $g_nodeRepository[$cache_id][$node] = &newNode($node, $init);
    }    
    return $g_nodeRepository[$cache_id][$node];
  }

  function &getModule($modname)
  {
    global $g_moduleRepository, $g_modules;
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
   * Checks if a certain node exists.
   * @param $node the node type
   * @return node exists?
   */
  function nodeExists($node)
  {
    static $existence = array();
    if (array_key_exists($node, $existence)) 
      return $existence[$node];
      
    $module = getNodeModule($node);
    if ($module == "") $module = &new atkModule();
    else $module = &getModule(getNodeModule($node));
    
    $exists = is_object($module) && $module->nodeExists($node);
    $existence[$node] = $exists;
    return $exists;
  }

  /**
   * Return the physical directory of a module..
   */
  function moduleDir($module)
  {
    global $g_modules;
    if (isset($g_modules[$module]))
    {
      $dir = $g_modules[$module];     
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
    global $g_modules;
		
    $modulekeys = array_keys($g_modules);
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
   * Preloads all modules. If a module_preload.inc file exists in the
   * module directory, this file will be included. If no module_preload.inc
   * file exists in the module directory the module.inc file will be used 
   * instead (to remain backwards compatible).
   */
  function atkPreloadModules()
  {
    global $g_modules, $g_modifiers, $g_overloaders, $g_moduleflags;
    
    foreach ($g_modules as $modname => $modpath)
    {
      if (!hasFlag($g_moduleflags[$modname], MF_NO_PRELOAD))
      {
        $preload = "$modpath/module_preload.inc";
        $module = "$modpath/module.inc";
        $filename = file_exists($preload) ? $preload : $module;
  
        // no module file exists
        if (!file_exists($filename))
        {
          atkdebug("Couldn't find module_preload.inc or module.inc for module '$modname' in '$modpath'");
          continue;
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
        include_once($filename);
        
        for ($i=0, $_i = count($modifiers); $i < $_i; $i++)
          $g_modifiers[$modifiers[$i]][] = $modname;
  
        if (count($overloaders) > 0)
          $g_overloaders = array_merge($g_overloaders, $overloaders);
      }
    }
  }
?>
