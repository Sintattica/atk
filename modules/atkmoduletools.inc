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
   * Creates an instance of a node
   * @param $node the node string
   * @return the created instance
   */
  function createNode($node)
  {
    global $config_module_path;
  
    /* module and type */
    $module = getNodeModule($node);
    $type = getNodeType($node);
    $file = "class.$type.inc";
    
    /* filename */
    if (empty($module)) $file = $config_atkroot."class.$type.inc";
    else $file = "$config_module_path/$module/class.$type.inc";
    
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

?>