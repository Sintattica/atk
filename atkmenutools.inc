<?php

/**
 *
 * A collection of functions for creating ATK menu's.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 *
 * $Id$
 *
 */

  /**
   * Creates a new (sub)menu item or a new submenu
   * @param $name menu item or submenu name
   * @param $parent parent menu name (default is main)
   * @param $url the url the item points to
   * @param $enable enable or disable
   * @param $order the order in which the menu appears
   * @param $module the name of the module that added this menuitem
   */
  function menuitem($name="", $url="", $parent="main", $enable=1, $order=0, $module="")
  { 
    static $order_value=100;
    if($order==0) 
    {
      $order=$order_value;
      $order_value+=100;
    }
//    if ($name == "-") $GLOBALS["g_menu"][$parent][] = "-"; else
    $GLOBALS["g_menu"][$parent][] = array("name" => $name, 
                                          "url" => $url, 
                                          "enable" => $enable, 
                                          "order"=> $order,
                                          "module"=> $module);
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

?>