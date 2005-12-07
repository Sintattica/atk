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

    function load()
    {
      global $ATK_VARS, $g_menu, $g_menu_parent;
      $theme = &atkinstance("atk.ui.atktheme");
      $atkmenutop = $ATK_VARS["atkmenutop"];
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
            $delimiter = $theme->getAttribute("menu_delimeter");
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
            $url = 'menu.php?atkmenutop='.$name;
            $menu .= href($url,atktext("menu_$name",$modname),SESSION_DEFAULT).$delimiter;
          }
          else if (empty($url) && !$enable)
          {
            //$menu .=text("menu_$name").$config_menu_delimiter;
          }
          /* normal menu item */
          else if ($enable) $menu .= href($url,atktext("menu_$name",$modname),SESSION_NEW,false,'target="main"').$delimiter;
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
        $menu .= href('menu.php?atkmenutop='.$parent,atktext("back_to").' '.atktext("menu_$parent", $modname),SESSION_DEFAULT)."<br />\n";
      }
      $menu.=$this->getFooter($atkmenutop);
      $ui = &atkinstance("atk.ui.atkui");

      return $ui->renderBox(array("title"=>atktext("menu_".$atkmenutop,$modname),
                                  "content"=>$menu,
                                  "menuitems"=>$menuitems),"menu");
    }
    
    function registerfiles(&$page, &$theme)
    {
      $page->register_style($theme->stylePath("style.css"));
      $page->register_style($theme->stylePath("menu.css"));
      $page->register_script(atkconfig("atkroot")."atk/javascript/menuload.js");
    }
    
    function render()
    {
      $page = &atknew("atk.ui.atkpage");
      $theme = &atkinstance("atk.ui.atktheme");
      $this->registerfiles();

      $page->addContent($this->load());

      return $page->render("Menu", true);
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
      $theme = &atkinstance("atk.ui.atktheme");
      switch($theme->getAttribute("menu_pos"))
      {
        case "right":   return MENU_RIGHT;
        case "top":     return MENU_TOP;
        case "bottom":  return MENU_BOTTOM;
      }
      return MENU_LEFT;
    }

    function getScrollable()
    {
      return MENU_UNSCROLLABLE;
    }

		function getMultilevel()
		{
			return MENU_MULTILEVEL;
		}
  }

?>
