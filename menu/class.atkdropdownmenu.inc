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
  atkimport('atk.menu.atkplainmenu');

  /**
   * Implementation of the Dropdowntext menu.
   *
   * @author Ber Dohmen <ber@ibuildings.nl>
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage menu
   */
  class atkDropdownMenu extends atkPlainMenu
  {
    function render()
    {
      $page = &atkinstance("atk.ui.atkpage");
      $menu = $this->load();
      $page->addContent($menu);

      return $page->render("Menu", true);
    }

    function load()
    {
      global $ATK_VARS,$g_menu;

      $page = &atkinstance('atk.ui.atkpage');
      $theme = &atkinstance("atk.ui.atktheme");
      $page->register_script(atkconfig('atkroot').'atk/javascript/dropdown_menu.js');
      $page->register_style($theme->stylePath("atkdropdownmenu.css"));
      $page->m_loadscripts[] = "new DHTMLListMenu('nav');";

      $atkmenutop = $ATK_VARS["atkmenutop"];
      if($atkmenutop=="") $atkmenutop="main";
      if (!is_array($g_menu[$atkmenutop]))
        $g_menu[$atkmenutop] = array();
      usort($g_menu[$atkmenutop],array("atkplainmenu","menu_cmp"));

      $menu.= "<div id='nav'>\n";
      $menu.=$this->getHeader($atkmenutop);

      $menu.="  <ul>\n";
      foreach ($g_menu[$atkmenutop] as $menuitem)
      {
        $menu .= $this->getMenuItem($menuitem, "    ");
      }
      
      if (atkconfig("menu_logout_link"))
      {
        $menu.="    <li><a href='app.php?atklogout=1'>".atktext('logout')."</a></li>\n";
      }
      
      $menu.="  </ul>\n";

      $menu.=$this->getFooter($atkmenutop);
      $menu.="</div>\n";
      return $menu;
    }

    function getMenuItem($menuitem, $indentation="")
    {
      global $g_menu;

      $enable = $this->getEnabled($menuitem);
      if ($enable)
      {
        if (!$menuitem['url'] && $g_menu[$menuitem['name']])
        {
          $submenu=$indentation."<ul>\n";
          foreach ($g_menu[$menuitem['name']] as $submenuitem)
          {
            $submenu.=$this->getMenuItem($submenuitem,$indentation."  ",$submenuname='',$menuitem['name']);
          }
          $submenu.=$indentation."</ul>\n";
          $menu.=$indentation.$this->getItemHtml($menuitem, "\n".$submenu.$indentation);
        }
        else
        {
          $menu.=$indentation.$this->getItemHtml($menuitem);
        }
      }
      return $menu;
    }

    function getEnabled($menuitem)
    {
      global $g_menu;
      
      $enable = $menuitem['enable'];
      if (is_array($enable))
      {
        $enabled = false;
        for ($j=0;$j<(count($enable)/2);$j++)
        {
          $enabled = $enabled || is_allowed($enable[(2*$j)],$enable[(2*$j)+1]);
        }
        $enable = $enabled;
      }
      elseif (is_array($g_menu[$menuitem['name']]))
      {
        $enabled = false;
        foreach ($g_menu[$menuitem['name']] as $item)
          $enabled = $enabled || $this->getEnabled($item);       
        $enable = $enabled;
      }
      
      return $enable;
    }

    function getItemHtml($menuitem, $submenu="",$submenuname='')
    {
      $delimiter = atkconfig("menu_delimiter");

      $name = $this->getMenuTranslation($menuitem['name'],$menuitem['module']);
      if ($menuitem['name']=='-') return "<li><a href='#'>-</a></li>\n";
      if ($menuitem['url']) $href = href($menuitem['url'],$this->getMenuTranslation($menuitem['name'],$menuitem['module']), SESSION_NEW);
      else $href = '<a href ="#">'.$name.'</a>';

      return "<li id='{$menuitem['module']}.{$menuitem['name']}' class='$submenuname'>".$href.$delimiter.$submenu."</li>\n";
    }
  }

?>
