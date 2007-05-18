<?PHP
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
   * Implementation of the plaintext menu.
   *
   * @author Ber Dohmen <ber@ibuildings.nl>
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage menu
   */
  class atkPlainMenu extends atkmenuinterface
  {
    var $m_height;

    function atkPlainMenu()
    {
      $this->m_height = "50";
    }

    function render()
    {
      $page = &atkinstance("atk.ui.atkpage");
      $theme = &atkinstance("atk.ui.atktheme");
      $page->addContent($this->getMenu());

      return $page->render("Menu", true);
    }

    function getMenu()
    {
       global $ATK_VARS, $g_menu, $g_menu_parent;
       $atkmenutop = $ATK_VARS["atkmenutop"];
       $theme = &atkinstance('atk.ui.atktheme');
       $page = &atkinstance('atk.atkpage');
       if($atkmenutop=="") $atkmenutop="main";

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

            /* submenu ? */
            else if (empty($url) && $enable)
            {
              $url = $theme->getAttribute('menufile',atkconfig("menufile",'menu.php')).'?atkmenutop='.$name;
              $menu .= href($url,$this->getMenuTranslation($name,$modname),SESSION_DEFAULT).$delimiter;
            }
            else if (empty($url) && !$enable)
            {
              //$menu .=text("menu_$name").$config_menu_delimiter;
            }
            /* normal menu item */
            else if ($enable) $menu .= href($url,$this->getMenuTranslation($name,$modname),SESSION_NEW,false,$theme->getAttribute('menu_params',atkconfig('menu_params','target="main"'))).$delimiter;
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
          $menu .= href($theme->getAttribute('menufile',atkconfig("menufile",'menu.php')).'?atkmenutop='.$parent,atktext("back_to", "atk").' '.$this->getMenuTranslation($parent, $modname),SESSION_DEFAULT).$delimiter;
        }
        $menu.=$this->getFooter($atkmenutop);
        $page->register_style($theme->stylePath("style.css"));
        $page->register_script(atkconfig("atkroot")."atk/javascript/menuload.js");
        $ui = &atkinstance("atk.ui.atkui");

        return $ui->renderBox(array("title"=>$this->getMenuTranslation($atkmenutop,$modname),
                                               "content"=>$menu,
                                               "menuitems"=>$menuitems),"menu");
    }

    function menu_cmp($a,$b)
    {
      if ($a["order"] == $b["order"]) return 0;
      return ($a["order"] < $b["order"]) ? -1 : 1;
    }


    function getHeight()
    {
      return $this->m_height;
    }

    function getPosition()
    {
      switch(atkconfig("menu_pos", "left"))
      {
        case "right": return MENU_RIGHT;
        case "top": return MENU_TOP;
        case "bottom": return MENU_BOTTOM;
      }
      return MENU_LEFT;
    }

    function getScrollable()
    {
      return MENU_SCROLLABLE;
    }

		function getMultilevel()
		{
			return MENU_MULTILEVEL;
		}
  }

?>
