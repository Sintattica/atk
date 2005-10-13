<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage menu
   * @author Peter Verhave <peter@ibuildings.nl>
   *
   * This file contains several utility functions for building the application
   * menu.
   *
   * @todo The global menu functions should be moved to a class (atkMenu?)
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Create a new menu item 
   *
   * Both main menu items, separators, submenus or submenu items can be
   * created, depending on the parameters passed.
   *
   * @param String $name The menuitem name. The name that is displayed in the
   *                     userinterface can be influenced by putting 
   *                     "menu_something" in the language files, where 'something'
   *                     is equal to the $name parameter.
   *                     If "-" is specified as name, the item is a separator.
   *                     In this case, the $url parameter should be empty.
   * @param String $url The url to load in the main application area when the
   *                    menuitem is clicked. If set to "", the menu is treated
   *                    as a submenu (or a separator if $name equals "-").
   *                    The dispatch_url() method is a useful function to
   *                    pass as this parameter.
   * @param String $parent The parent menu. If omitted or set to "main", the
   *                       item is added to the main menu.
   * @param mixed $enable This parameter supports the following options:
   *                      1: menuitem is always enabled
   *                      0: menuitem is always disabled
   *                         (this is useful when you want to use a function
   *                         call to determine when a menuitem should be
   *                         enabled. If the function returns 1 or 0, it can
   *                         directly be passed to this method in the $enable
   *                         parameter.
   *                      array: when an array is passed, it should have the 
   *                             following format:
   *                             array("node","action","node","action",...)
   *                             When an array is passed, the menu checks user
   *                             privileges. If the user has any of the
   *                             node/action privileges, the menuitem is
   *                             enabled. Otherwise, it's disabled.
   * @param int $order The order in which the menuitem appears. If omitted,
   *                   the items appear in the order in which they are added 
   *                   to the menu, with steps of 100. So, if you have a menu
   *                   with default ordering and you want to place a new
   *                   menuitem at the third position, pass 250 for $order.
   * @param $module The name of the module that added this menuitem. It is usually
   *                not necessary to pass this parameter, but is present for
   *                backwardscompatibility reasons.
   */
  function menuitem($name="", $url="", $parent="main", $enable=1, $order=0, $module="")
  { 
    global $g_menu, $g_menu_parent;
    static $order_value=100, $s_dupelookup;
    if($order==0) 
    {
      $order=$order_value;
      $order_value+=100;
    }
    
    $item = array("name" => $name, 
                  "url" => $url, 
                  "enable" => $enable, 
                  "order"=> $order,
                  "module"=> $module);
        
    if (isset($s_dupelookup[$parent][$name])&&$s_dupelookup[$parent][$name]!=""&&$name!="-")
    {
      $g_menu[$parent][$s_dupelookup[$parent][$name]] = $item;
    }
    else
    {    
      $s_dupelookup[$parent][$name] = isset($g_menu[$parent])?count($g_menu[$parent]):0;
      $g_menu[$parent][] = $item;
    }
    $g_menu_parent[$name] = $parent;
  }

  /**
   * Creates multiple (sub)menu items and/or submenu(s) at once.
   * @param array $menu Array with menu/submenu items, in the following 
   *                    format: array($parent=>array(0=>
   *                                              array("url"=>$url,
   *                                                    "name"=>$name)))
   */
  function menuitems($menu)
  {
    while (list($parent, $items) = each($menu))
      for ($i = 0; $i < count($items); $i++)
      {
        $GLOBALS["g_menu"][$parent][] = $items[$i];
        if (empty($items[$i]["url"])) $GLOBALS["g_menu_parent"][$items[$i]["name"]] = $parent;      
      }
  }

?>
