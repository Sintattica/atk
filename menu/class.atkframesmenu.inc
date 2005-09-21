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
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Implementation of the framestext menu.
   *
   * @author Ber Dohmen <ber@ibuildings.nl>
   * @package atk
   * @subpackage menu
   */
  atkimport("atk.menu.atkplainmenu");
  class atkFramesMenu extends atkPlainMenu
  {
    
    function render()
    {
       global $ATK_VARS,$g_menu, $g_menu_parent;
       $atkmenutop = $ATK_VARS["atkmenutop"];
       if($atkmenutop=="") $atkmenutop="main";
       
       $menu = "<div align='".atkconfig("menu_align", "center")."'>"; 
       $menu.=$this->getHeader($atkmenutop);
        if (is_array($g_menu[$atkmenutop]))
        {
          usort($g_menu[$atkmenutop],array("atkplainmenu","menu_cmp"));   
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
            
            else if ($enable) // don't show menu items we don't have access to.
            {
        
              $hassub = (is_array($g_menu[$g_menu[$atkmenutop][$i]["name"]]));
        
              /* submenu ? */
              if ($hassub)
              {
                if (empty($url)) // normal submenu
                {
                  $menu .= href('menu.php?atkmenutop='.$name,text("menu_$name")).$delimiter;
                }
                else // submenu AND a default url.
                {
                  $menuurl = session_url('menu.php?atkmenutop='.$name);
                  $mainurl = session_url($url,SESSION_NEW);
                  $menu.= '<a href="javascript:menuload(\''.$menuurl.'\', \''.$mainurl.'\');">'.text("menu_$name").'</a>'.$delimiter;
                }
              }
              else // normal menuitem
              {
                $menu .= href($url,text("menu_$name"),SESSION_NEW,false,'target="main"').$delimiter;
              }
            }
          }
        }
        /* previous */  
        if ($atkmenutop != "main")
        {
          $parent = $g_menu_parent[$atkmenutop];
          $menu .= atkconfig("menu_delimiter");
          $menu .= href('menu.php?atkmenutop='.$parent,text("back_to").' '.text("menu_$parent"),SESSION_DEFAULT).'<br>';  
        }      
        $menu.=$this->getFooter($atkmenutop);
        $page = &atknew("atk.ui.atkpage");
        $theme = &atkTheme::getInstance();  
        $page->register_style($theme->stylePath("style.css"));
        $page->register_script(atkconfig("atkroot")."atk/javascript/menuload.js");
        
        atkimport("atk.ui.atkui");
        $ui = &atkUI::getInstance();

        $box = $ui->renderBox(array("title"=>text("menu_".$atkmenutop),
                                               "content"=>$menu,
                                               "menuitems"=>$menuitems),"menu");
       
        $page->addContent($box);
      			
        return $page->render("Menu", true);  
             
    }
    

  }
?>