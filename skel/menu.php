<?php
  /**
   * Builds a navigation menu for the ATK application. 
   * Items for the main application can be added within the
   * config.menu.inc file with the menuitem() method. Modules
   * can register menuitems in their constructor. The menu
   * has support for enabling/disabling items on a user profile base.
   *
   * For more information check the atkmoduletools.inc file!
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$   
   * $Log$
   * Revision 4.1  2001/04/23 13:21:22  peter
   * Introduction of module support. An ATK application can now have zero
   * or more modules which can, but don't have to, contain ATK nodes.
   *
   */
  require "atk/class.atknode.inc"; 
  require "atk/atkmenutools.inc";  
  include "config.menu.inc";
  include "theme.inc";

  /* first add module menuitems */
  for ($i = 0; $i < count($g_modules); $i++)
  {
    $module = new $g_modules[$i]();
    menuitems($module->getMenuItems());
  }

  /* output html */
  $g_layout->output("<html>");
  $g_layout->head($txt_app_title);
  $g_layout->body();
  $g_layout->output("<br><div align='center'>"); 
  $g_layout->ui_top("Menu");
  $g_layout->output("<br>");

  /* top (sub)menu */
  if (!isset($atkmenutop)) $atkmenutop = "main";

  /* build menu */
  $menu = "";  
  for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
  {
    $name = $g_menu[$atkmenutop][$i]["name"];
    $url = $g_menu[$atkmenutop][$i]["url"];
    $enable = $g_menu[$atkmenutop][$i]["enable"];

    /* delimiter ? */
    if ($g_menu[$atkmenutop][$i] == "-") $menu .= $config_menu_delimiter;
    
    /* submenu ? */
    else if (empty($url) && $enable) $menu .= '<a href="menu.php?atkmenutop='.$name.'">'.text("menu_$name").'</a><br>';
    else if (empty($url) && !$enable) $menu .= text("menu_$name").'</a><br>';
      
    /* normal menu item */
    else if ($enable) $menu .= '<a href="'.$url.'" target="main">'.text("title_$name").'</a><br>';
    else $menu .= text("title_$name").'</a><br>';    
  }
  
  /* previous */
  if ($atkmenutop != "main")
  {
    $parent = $g_menu_parent[$atkmenutop];
    $menu .= $config_menu_delimiter;
    $menu .= '<a href="menu.php?atkmenutop='.$parent.'"><< '.text("menu_$parent").'</a><br>';  
  }

  /* bottom */
  $g_layout->output($menu);
  $g_layout->output("<br><br>");
  $g_layout->ui_bottom();
  $g_layout->output("</div></html>"); 
  $g_layout->outputFlush();
?>