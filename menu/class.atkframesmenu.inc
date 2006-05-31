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
       $atkmenutop = atkArrayNvl($ATK_VARS, "atkmenutop", "main");
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
            $modname = $g_menu[$atkmenutop][$i]["module"];

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
            if ($name == "-") $menu .= $delimiter;

            else if ($enable) // don't show menu items we don't have access to.
            {

              $hassub = isset($g_menu[$g_menu[$atkmenutop][$i]["name"]]);

              /* submenu ? */
              if ($hassub)
              {
                if (empty($url)) // normal submenu
                {
                  $menu .= href('menu.php?atkmenutop='.$name,$this->getMenuTranslation($name, $modname)).$delimiter;
                }
                else // submenu AND a default url.
                {
                  $menuurl = session_url('menu.php?atkmenutop='.$name);
                  $mainurl = session_url($url,SESSION_NEW);
                  $menu.= '<a href="javascript:menuload(\''.$menuurl.'\', \''.$mainurl.'\');">'.$this->getMenuTranslation($name,$modname).'</a>'.$delimiter;
                }
              }
              else // normal menuitem
              {
                $menu .= href($url,$this->getMenuTranslation($name,$modname),SESSION_NEW,false,'target="main"').$delimiter;
              }
            }
          }
        }
        /* previous */
        if ($atkmenutop != "main")
        {
          $parent = $g_menu_parent[$atkmenutop];
          $menu .= atkconfig("menu_delimiter");
          $menu .= href('menu.php?atkmenutop='.$parent,atktext("back_to", "atk").' '.$this->getMenuTranslation($parent, $modname),SESSION_DEFAULT).$delimiter;
        }
        $menu.=$this->getFooter($atkmenutop);
        $page = &atknew("atk.ui.atkpage");
        $theme = &atkinstance("atk.ui.atktheme");
        $page->register_style($theme->stylePath("style.css"));
        $page->register_script(atkconfig("atkroot")."atk/javascript/menuload.js");

        $ui = &atkinstance("atk.ui.atkui");

        $box = $ui->renderBox(array("title"=>$this->getMenuTranslation($atkmenutop,$modname),
                                               "content"=>$menu,
                                               "menuitems"=>$menuitems),"menu");

        $page->addContent($box);

        return $page->render("Menu", true);
    }
  }
?>
