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
 * $Log$
 * Revision 1.14  2001/09/05 12:17:05  sandy
 * bug fix for the external modules
 *
 * Revision 1.13  2001/08/30 15:28:35  ivo
 * Added preliminary external module support.
 *
 * Revision 1.12  2001/05/17 15:03:45  peter
 * Added module_url function, usefull for url's to files in the module dir.
 *
 * Revision 1.11  2001/05/14 12:57:27  ivo
 * Added some code to detect if node was created succesfully.
 *
 * Revision 1.10  2001/05/11 15:24:54  peter
 * Some updates for the module support and some other things...
 *
 * Revision 1.9  2001/05/11 14:59:56  ivo
 * Fixed serious bug in onetoone relation that could overwrite records in a
 * table..
 *
 * Revision 1.8  2001/05/10 08:31:01  ivo
 * Major upgrade. Changes:
 * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
 *   singletons by default, and nodefunctions pass around recordsets.
 * + Session management for forms. If you now leave a page through a click on
 *   a link, the session remembers everything from your form and restores it
 *   when you return.
 * + New relation: oneToOneRelation
 * + Reimplemented the embedded editForm feature (forms inside forms)
 *
 * Revision 1.7  2001/05/07 15:13:49  ivo
 * Put config_atkroot in all files.
 *
 * Revision 1.6  2001/05/07 14:46:15  ivo
 * Atk now has a configurable rootdirectory (specified in dispatcher)
 *
 * Revision 1.5  2001/05/01 09:49:49  ivo
 * Replaced all require() and include() calls by require_once() and
 * include_once() calls. The if(!DEFINED)... inclusion protection in files
 * is now obsolete.
 *
 * Revision 1.4  2001/05/01 09:15:51  ivo
 * Initial session based atk version.
 *
 * Revision 1.3  2001/04/24 12:40:50  ivo
 * skel cleanup, typo fix in moduletools.
 *
 * Revision 1.2  2001/04/24 11:50:13  ivo
 * Bugfixes to modulesupport: changed all 'require' statements to 'include'
 * statements, since 'require' is always executed at script compile time,
 * not at runtime.
 *
 * Revision 1.1  2001/04/23 13:21:22  peter
 * Introduction of module support. An ATK application can now have zero
 * or more modules which can, but don't have to, contain ATK nodes.
 *
 */
 
  // A repository of node instances..
  $g_nodeRepository = array();
  
  /**
   * Registers an ATK module
   */
  function registerModule($module)
  {
    global $config_module_path;
    
    $filename = "$config_module_path/$module/module.inc";
    if (file_exists($filename))
    {
      include_once($filename);
      $GLOBALS["g_modules"][] = $module;
    }
  }
    
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
    atkdebug("Returning node instance - $node");
    return $g_nodeRepository[$node];    
  }
  
  /**
   * Construct a new node 
   * @param $node the node type
   * @return new node object
   */
  function newNode($node)
  {
   global $config_module_path, $config_atkroot, $g_externalModules;
  
    /* module and type */
    $module = getNodeModule($node);
    $type = getNodeType($node);
    $file = "class.$type.inc";
    
    /* filename */
    if (empty($module)) $file = $config_atkroot."class.$type.inc";  
    else 
    {
      // module can be an external module.. if it is, we use it's defined path.
      if (is_array($g_externalModules)&&in_array($module, array_keys($g_externalModules)))
      {
        $file = $g_externalModules[$module]."/class.$type.inc";
      }
      else
      {
        // standard module from the defined module path
        $file = "$config_atkroot$config_module_path/$module/class.$type.inc";
      }
    }
  
    /* check for file */
    if (!file_exists($file))
    {
      atkdebug("Cannot create node, because a required file ($file) does not exist!", "critical");
      return NULL;
    }

    /* return object */
    include_once($file);
    
    return new $type();
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
?>