<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage menu
   *
   * Implementation of a dhtml menu.
   *
   * To use the dhtml menu, just set the following in your config.inc.php 
   * $config_menu_layout   = "dhtml";
   * $config_tabselectMode = "mouseover"; (can also be "click") 
   *
   * @author Wim Kosten <wim@ibuildings.nl>
   * @author Ber Dohmen <ber@ibuildings.nl>
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
  
  /**
   * @internal Includes etc.
   */  
  $config_platformroot = "../";
  $config_atkroot = "../";
  
  atkimport("atk.ui.atkpage");
  $page = &atkPage::getInstance();
  $ui =   &atknew("atk.ui.atkui");
  $theme = &atkTheme::getInstance();  

  include_once("../atk.inc");
  include_once($config_atkroot."atk/menu/general.inc");

  global $g_menu;

  $tabs = "";
  $divs = "";
  $tab  = 1;

  while (list ($name) = each ($g_menu))
  {
   $tabContent = "";
   $atkmenutop = $name;
   $tabName    = addslashes(text($atkmenutop, "menu"));
   $items      = 0;

   for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
   {
    $menu   = "";
    $name   = $g_menu[$atkmenutop][$i]["name"];
    $url    = session_url($g_menu[$atkmenutop][$i]["url"],SESSION_NEW);
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
    if ($g_menu[$atkmenutop][$i]["name"] == "-") $menu .= "";
    
    /* normal menu item */   
    else if ($enable)        
    {            
     if($g_menu[$atkmenutop][$i]["url"]!="")
     {    
      $tabContent .= "<a target='main' class='tablink' href='$url'>".text("$name", "menu")."</a>";

      if ($i < count($g_menu[$atkmenutop])-1)
      {
       $tabContent .= "&nbsp;|&nbsp;";
      }

      $items++;
     }
    }
   }

   if ($items > 0)
   {
    $tabs   .= '   rows[1]['.$tab.'] = "'.$tabName.'"'."\n";
    $divs   .= '<div id="T1'.$tab.'" class="tab-body">'.$tabContent.'</div>'."\n";
    $tab++;
   }
  }   

  // add options tab containing logout
  $tabs   .= '   rows[1]['.$tab.'] = "Opties"'."\n";
  $divs   .= '<div id="T1'.$tab.'" class="tab-body"><a class="tablink" href="index.php?atklogout=1" target="_top">'.text("logout").'</a></div>'."\n";  
   
  $page->register_style($theme->stylePath("style.css"));
  $page->register_style($theme->stylePath("dhtmlmenu.css"));
  $page->register_script(atkconfig("atkroot")."atk/javascript/atk_tabs.js");
   
  $code = 'var tabSelectMode = "'.$config_tabselectMode.'";'."\n";
 
  $code.= 'var rows     = new Array();
           var num_rows = 1;
           var top      = 0;
           var left     = 10;
           var width    = "100%";
           var tab_off  = "#198DE9";
           var tab_on   = "#EEEEE0";

           rows[1]      = new Array;';
  $code.="\n".$tabs."\n";
  
  $code.="\n".'generateTabs();'."\n";
    
  $page->register_scriptcode($code);  

  $page->addContent($divs);
   
  $page->addContent('<script language="JavaScript">
                        if (DOM) { currShow=document.getElementById(\'T11\');}
                        else if (IE) { currShow=document.all[\'T11\'];}
                        else if (NS4) { currShow=document.layers[\'T11\'];}'."\n".
                       'changeCont("11", "tab11");'."\n</script>");
   
  $string = $page->render("Menu", true);
  $output = &atkOutput::getInstance();
  $output->output($string);
  $output->outputFlush();

?>