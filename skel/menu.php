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
   * Revision 4.7  2001/05/10 08:31:01  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.6  2001/05/07 15:13:49  ivo
   * Put config_atkroot in all files.
   *
   * Revision 4.5  2001/05/01 09:49:49  ivo
   * Replaced all require() and include() calls by require_once() and
   * include_once() calls. The if(!DEFINED)... inclusion protection in files
   * is now obsolete.
   *
   * Revision 4.4  2001/05/01 09:15:51  ivo
   * Initial session based atk version.
   *
   * Revision 4.3  2001/04/24 13:51:50  ivo
   * Fixed some small bugs, and updated the language files, improved the menu.
   *
   * Revision 4.2  2001/04/23 15:59:07  peter
   * Removed something that didn't belong there...
   *
   * Revision 4.1  2001/04/23 13:21:22  peter
   * Introduction of module support. An ATK application can now have zero
   * or more modules which can, but don't have to, contain ATK nodes.
   *
   */
  $config_atkroot = "./";
  require_once($config_atkroot."atk/class.atknode.inc");
  require_once($config_atkroot."atk/atkmenutools.inc");
  include_once($config_atkroot."config.menu.inc");

  /* first add module menuitems */
  for ($i = 0; $i < count($g_modules); $i++)
  {
    $module = new $g_modules[$i]();
    menuitems($module->getMenuItems());
  }

  if (!isset($atkmenutop)||$atkmenutop=="") $atkmenutop = "main";

  /* output html */
  $g_layout->output("<html>");
  $g_layout->head($txt_app_title);
  $g_layout->body();
  $g_layout->output("<br><div align='center'>"); 
  $g_layout->ui_top(text("menu_".$atkmenutop));
  $g_layout->output("<br>");

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
    else if (empty($url) && $enable) $menu .= href('menu.php?atkmenutop='.$name,text("menu_$name"),SESSION_NEW).'<br>';
    else if (empty($url) && !$enable) $menu .= text("menu_$name").'<br>';
      
    /* normal menu item */
    else if ($enable) $menu .= href($url,text("title_$name"),SESSION_NEW,false,'target="main"').'<br>';
    else $menu .= text("title_$name").'<br>';    
  }
  
  /* previous */
  if ($atkmenutop != "main")
  {
    $parent = $g_menu_parent[$atkmenutop];
    $menu .= $config_menu_delimiter;
    $menu .= href('menu.php?atkmenutop='.$parent,'<< '.text("menu_$parent"),SESSION_NEW).'<br>';  
  }

  /* bottom */
  $g_layout->output($menu);
  $g_layout->output("<br><br>");
  $g_layout->ui_bottom();
  $g_layout->output("</div></html>"); 
  $g_layout->outputFlush();
?>
