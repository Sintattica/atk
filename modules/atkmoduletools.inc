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
  function getNode($node)
  {      
    global $g_nodeRepository;
    if (!is_object($g_nodeRepository[$node]))
    {
      atkdebug("Constructing a new node - $node");
      $g_nodeRepository[$node] = newNode($node);  
      if (is_object($g_nodeRepository[$node]))
      {
        $g_nodeRepository[$node]->m_module = getNodeModule($node);
      }
    } 
    else
    {
      atkdebug("Returning cached node instance - $node");
    }
    return $g_nodeRepository[$node];    
  }
  
  function getModule($modname)
  {
    global $g_moduleRepository;
    if (!is_object($g_moduleRepository[$modname]))
    {  
      atkdebug("Constructing a new module - $modname");
      if (class_exists("mod_".$modname))
      {
        $realmodname = "mod_".$modname;
        $mod = new $realmodname();
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
  function newNode($node)
  {
    global $g_modules, $config_atkroot, $g_overloaders;  
    
    if ($g_overloaders[$node]!="")
    {
      atkdebug("Using overloader '".$g_overloaders[$node]."' for class '".$node."'");
      // include original file, because overloaders extend the original class.
      $file = nodeFile($node);
    
      /* check for file */
      if (!file_exists($file))
      {  
        atkerror("Cannot create node, because a required file ($file) does not exist!", "critical");
        return NULL;
      }  
      /* return object */
      include_once($file);
      $node = $g_overloaders[$node];
    }
    
    $file = nodeFile($node);
    $type = getNodeType($node);
  
    /* check for file */
    if (!file_exists($file))
    {  
      if (function_exists("missing_class_handler"))
      {
        return missing_class_handler($node);
      }
      else
      {
        atkerror("Cannot create node, because a required file ($file) does not exist!", "critical");      
        return NULL;
      }      
    }  
    /* return object */
    include_once($file);
    
    $node =  new $type();
    // Nodes must be initialised.
    $node->init();
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
?>
