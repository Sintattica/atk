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
   *
   */  

  /* output html */
  $g_layout->output("<html>");
  $g_layout->head($txt_app_title);
  $g_layout->body();
  $g_layout->output("<div align='center'>"); 
  $g_layout->ui_top(text("menu_".$atkmenutop));
  $g_layout->output("<br>");

  /* build menu */    
  
  $menu = "";  
  
  function menu_cmp($a,$b)
  {
    if ($a["order"] == $b["order"]) return 0;
    return ($a["order"] < $b["order"]) ? -1 : 1;
  }
  usort($g_menu[$atkmenutop],"menu_cmp");   

  for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
  {
    $name = $g_menu[$atkmenutop][$i]["name"];
    $url = $g_menu[$atkmenutop][$i]["url"];
    $enable = $g_menu[$atkmenutop][$i]["enable"];
    
    if (is_array($enable))
    {
      $enabled = false;
      for ($j=0;$j<(count($enable)/2);$j++)
      {
        $enabled |= is_allowed($enable[(2*$j)],$enable[(2*$j)+1]);
      }
      $enable = $enabled;
    }

    /* delimiter ? */
    if ($g_menu[$atkmenutop][$i]["name"] == "-") $menu .= "<br>";
    
    /* submenu ? */
    else if (empty($url) && $enable) $menu .= href('menu.php?atkmenutop='.$name,text("menu_$name"),SESSION_DEFAULT).$config_menu_delimiter;
    else if (empty($url) && !$enable)
    {
      //$menu .=text("menu_$name").$config_menu_delimiter;
    }
      
    /* normal menu item */
    else if ($enable) $menu .= href($url,text("menu_$name"),SESSION_NEW,false,'target="main"').$config_menu_delimiter;
    else 
    {
      //$menu .= text("menu_$name").$config_menu_delimiter;    
    }
  }
  
  /* previous */
  if ($atkmenutop != "main")
  {
    $parent = $g_menu_parent[$atkmenutop];
    $menu .= $config_menu_delimiter;
    $menu .= href('menu.php?atkmenutop='.$parent,text("back_to").' '.text("menu_$parent"),SESSION_DEFAULT).'<br>';  
  }

  /* bottom */
  $g_layout->output($menu);
  $g_layout->output("<br><br>");
  $g_layout->ui_bottom();
  $g_layout->output("</div></html>"); 
  $g_layout->outputFlush();
?>
