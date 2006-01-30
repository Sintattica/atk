<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk   
   *
   * Contains a set of functions for general node manipulation.
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   

  /**
   * Register a node in the main application
   * @param $node name of the node
   * @param $action array with actions that can be performed on the node
   */
  function registerNode($node, $action, $tabs=null)
  {
    global $g_nodes;
    $module = getNodeModule($node);
    $type = getNodeType($node);
    
    // prefix tabs with tab_
    for($i = 0; $i < sizeof($tabs); $i++)
      $tabs[$i] = "tab_" . $tabs[$i];
    
    if ($module=="") $module="main"; 
    $g_nodes[$module][$type] = array_merge($action,$tabs);
  }

?>