<?php

require "atk/modules/atkmoduletools.inc";

/**
 * Atk Menu Tools
 *
 * A collection of functions for creating ATK menu's.
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
if (!defined("ATKMENUTOOLS_INC"))
{
  define("ATKMENUTOOLS_INC", 1);

  /**
   * Creates a new (sub)menu item or a new submenu
   * @param $name menu item or submenu name
   * @param $parent parent menu name (default is main)
   * @param $url the url the item points to
   * @param $enable enable or disable
   */
  function menuitem($name="", $parent="main", $url="", $enable=1)
  {
    if ($name == "-") $GLOBALS["g_menu"][$parent][] = "-";
    else $GLOBALS["g_menu"][$parent][] = array("name" => $name, "url" => $url, "enable" => $enable);
    if (empty($url)) $GLOBALS["g_menu_parent"][$name] = $parent;
  }

  /**
   * Creates multiple (sub)menu items and/or submenu(s)
   * @param $menu array with menu/submenu items
   */
  function menuitems($menu)
  {
    while (list($parent, $items) = each($menu))
      for ($i = 0; $i < count($items); $i++)
      {
        $GLOBALS["g_menu"][$parent][] = $items[$i];
        if (empty($item["url"])) $GLOBALS["g_menu_parent"][$items[$i]["name"]] = $parent;      
      }
  }

  /**
   * Generate a dispatch menu URL for use with nodes
   * and their specific actions.
   * @param $node the (module.)node name
   * @param $action the atk action the link will perform
   * @return url for the node with the action
   */
  function dispatch_url($node, $action, $target="")
  {
    /* create url */
    $url = "dispatch.php?atknodetype=".$node."&atkaction=".$action;
    if (!empty($target)) $url .= "&atktarget=".$target;
    
    /* return url */
    return $url;
  }
  
  /**
   * Check whatever someone has got permission
   * to view a certain node with a certain action
   * @param $node the (module.)node name
   * @param $action the atk action
   * @return boolean if allowed or not
   */
  function is_allowed($node, $action)
  {
    $g_securityManager = $GLOBALS["g_securityManager"];
    if (!is_object($g_securityManager)) return 1;
    else return $g_securityManager->allowed($node, $action);
  }
}
?>