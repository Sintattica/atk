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
	
  global $g_menu, $atkmenutop;
	
  atkimport("atk.ui.atkpage");
  $page = &atkPage::getInstance();
  $ui =   &atknew("atk.ui.atkui");
  $theme = &atkTheme::getInstance();

  /* output html */
  $menu = "<div align='center'>"; 
  
  /* build menu */
  
  function menu_cmp($a,$b)
  {
    if ($a["order"] == $b["order"]) return 0;
    return ($a["order"] < $b["order"]) ? -1 : 1;
  }
  
  if (is_array($g_menu[$atkmenutop]))
  {
    usort($g_menu[$atkmenutop],"menu_cmp");   
  
    for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
    {
      if ($i==count($g_menu[$atkmenutop])-1)
      {
        $delimiter = "";
      }
      else
      {
        $delimiter = $config_menu_delimiter;
      }
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
      else if (empty($url) && $enable) $menu .= href('menu.php?atkmenutop='.$name,text("menu_$name"),SESSION_DEFAULT).$delimiter;
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
    }
  }
  /* previous */  
  if ($atkmenutop != "main")
  {
    $parent = $g_menu_parent[$atkmenutop];
    $menu .= $config_menu_delimiter;
    $menu .= href('menu.php?atkmenutop='.$parent,text("back_to").' '.text("menu_$parent"),SESSION_DEFAULT).'<br>';  
  }      
  
  $page->register_style($theme->stylePath("style.css"));
  $box = $ui->renderBox(array("title"=>text("menu_".$atkmenutop),
                                            "content"=>$menu));
 
  $page->addContent($box);
			
	$string = $page->render("Menu", true);
  $output = &atkOutput::getInstance();
  $output->output($string);
  $output->outputFlush();

?>
