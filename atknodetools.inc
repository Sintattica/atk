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
 * Revision 4.2  2001/05/01 09:49:49  ivo
 * Replaced all require() and include() calls by require_once() and
 * include_once() calls. The if(!DEFINED)... inclusion protection in files
 * is now obsolete.
 *
 * Revision 4.1  2001/04/23 13:21:22  peter
 * Introduction of module support. An ATK application can now have zero
 * or more modules which can, but don't have to, contain ATK nodes.
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
    g_nodes[$node] = $action;
  }

?>