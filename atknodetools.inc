<?php

/**
 * Atk Node Tools
 *
 * A collection of functions for managing nodes
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 *
 * $Id$
 * $Log$
 * Revision 4.1  2001/04/23 13:21:22  peter
 * Introduction of module support. An ATK application can now have zero
 * or more modules which can, but don't have to, contain ATK nodes.
 *
 */
if (!defined("ATKNODETOOLS_INC"))
{
  define("ATKNODETOOLS_INC", 1);

  /**
   * Register a node in the main application
   * @param $node name of the node
   * @param $action array with actions that can be performed on the node
   */
  function registerNode($node, $action)
  {
    global $g_nodes;
    g_nodes[$node] = $action;
  }
}
?>