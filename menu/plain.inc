<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage menu
   * @author Peter Verhage <peter@ibuildings.nl>
   *
   * @todo Move this to the menu class that wraps this code.
   *
   * Builds a plain text navigation menu for the ATK application. 
   * Items for the main application can be added within the
   * config.menu.inc file with the menuitem() method. Modules
   * can register menuitems in their constructor. The menu
   * has support for enabling/disabling items on a user profile base.
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */  
	
  /** @internal defines etc. */
  global $g_menu, $atkmenutop, $g_menu_parent;
	
  atkimport("atk.ui.atkpage");
  $page = &atkPage::getInstance();
  $ui =   &atknew("atk.ui.atkui");
  $theme = &atkTheme::getInstance();

  /* output html */
  $menu = "<div align='".atkconfig("menu_align", "center")."'>"; 
  
  /* build menu */
  
  /**
   * @ignore
   */
  function menu_cmp($a,$b)
  {
    if ($a["order"] == $b["order"]) return 0;
    return ($a["order"] < $b["order"]) ? -1 : 1;
  }
  
  if (is_array($g_menu[$atkmenutop]))
  {
    usort($g_menu[$atkmenutop],"menu_cmp");   
    $menuitems = array();
    for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
    {
      if ($i==count($g_menu[$atkmenutop])-1)
      {
        $delimiter = "";
      }
      else
      {
        $delimiter = atkconfig("menu_delimiter");
      }
      $name = $g_menu[$atkmenutop][$i]["name"];
      $menuitems[$i]["name"]=$name;
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

      $menuitems[$i]["enable"]=$enable;
  
      /* delimiter ? */
      if ($name == "-") $menu .= "<br>";
      
      /* submenu ? */
      else if (empty($url) && $enable)
      {
        $url = 'menu.php?atkmenutop='.$name;
        $menu .= href($url,text("menu_$name"),SESSION_DEFAULT).$delimiter;
      }
      else if (empty($url) && !$enable)
      {
        //$menu .=text("menu_$name").$config_menu_delimiter;
      }
      /* normal menu item */
      else if ($enable) $menu .= href($url,text("menu_$name"),SESSION_NEW,false,'target="main"').$delimiter;
      else 
      {
        //$menu .= text("menu_$name").$config_menu_delimiter;    
      }
      $menuitems[$i]["url"]=session_url($url);
    }
  }
  /* previous */  
  if ($atkmenutop != "main")
  {
    $parent = $g_menu_parent[$atkmenutop];
    $menu .= atkconfig("menu_delimiter");
    $menu .= href('menu.php?atkmenutop='.$parent,text("back_to").' '.text("menu_$parent"),SESSION_DEFAULT).'<br>';  
  }      
  
  $page->register_style($theme->stylePath("style.css"));
  $box = $ui->renderBox(array("title"=>text("menu_".$atkmenutop),
                                         "content"=>$menu,
                                         "menuitems"=>$menuitems),"menu");
 
  $page->addContent($box);
			
  $string = $page->render("Menu", true);
  $output = &atkOutput::getInstance();
  $output->output($string);
  $output->outputFlush();

?>
