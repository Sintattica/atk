<?php

/**
 *
 * A collection of functions for managing nodes
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 *
 * $Id$
 *
 */

  /**
   * Register a node in the main application
   * @param $node name of the node
   * @param $action array with actions that can be performed on the node
   */
  function registerNode($node, $action)
  {
    global $g_nodes;
    $module = getNodeModule($node);
    $type = getNodeType($node);
    
    if ($module=="") $module="main";    
    $g_nodes[$module][$type] = $action;
  }

?>