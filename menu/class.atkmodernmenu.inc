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
   * @copyright (c)2006-2007 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6320 $
   * $Id$
   */

  /**
   * Imports
   * @access private
   */
  atkimport("atk.menu.atkplainmenu");

  /**
   * Modern menu implementation
   * @package atk
   * @subpackage menu
   */
  class atkModernMenu extends atkPlainMenu
  {
    /**
     * Constructor
     *
     * @return atkModernMenu
     */
    function atkModernMenu()
    {
      $this->m_height = "130";
    }

    /**
     * Render the menu
     *
     * @return unknown
     */
    function render()
    {
      $page = &atkinstance('atk.ui.atkpage');
      $theme = &atkinstance("atk.ui.atktheme");
      if ($theme->getAttribute('modern_menu_compat'))
      {
        $page->addContent($this->getMenu());
        return $page->render("Menu", true);
      }
      else
      {
        $oldmenu = &atknew('atk.menu.atkplainmenu');
        return $oldmenu->render();
      }
    }

    /**
     * Get the menu
     *
     * @return string The menu
     */
    function getMenu()
    {
      $theme = &atkinstance("atk.ui.atktheme");
      if ($theme->getAttribute('modern_menu_compat'))
      {
        global $g_menu;
        $atkmenutop = (isset($_REQUEST['atkmenutop'])?$_REQUEST['atkmenutop']:sessionLoad('atkmenutop'));
        sessionStore('atkmenutop',$atkmenutop);

        $menuitems = $this->getMenuItems($g_menu,'main');

        $page = &atkInstance("atk.ui.atkpage");
        $page->register_style($theme->stylePath("style.css"));
        $page->register_style($theme->stylePath("menu.css"));
        $page->register_script(atkconfig("atkroot")."atk/javascript/menuload.js");

        $ui = &atkinstance("atk.ui.atkui");
        $atkmenutop=(isset($_REQUEST['atkmenutop'])?$_REQUEST['atkmenutop']:$atkmenutop);

        $box = $ui->renderBox(array("title"=>$this->getMenuTranslation(($atkmenutop?$atkmenutop:'main')),
        "menuitems"=>$menuitems,
        'menutop'=>$atkmenutop,
        'g_menu'=>$g_menu,
        'atkmenutop'=>$atkmenutop,
        'atkmenutopname'=>$this->getMenuTopName($menuitems,$atkmenutop)),"menu");

        return $box;
      }
      else
      {
        $oldmenu = &atknew('atk.menu.atkplainmenu');
        return $oldmenu->getMenu();
      }
    }

    /**
     * Get the menu top name
     *
     * @param array $menuitems
     * @param string $menutop
     * @return string The name of the menu top item
     */
    function getMenuTopName($menuitems,$menutop)
    {
      foreach ($menuitems as $menuitem)
      {
        if ($menuitem['id']==$menutop) return $menuitem['name'];
      }
    }

    /**
     * Get menuitems
     *
     * @param array $menu
     * @param string $menutop
     * @return array Array with menu items
     */
    function getMenuItems($menu, $menutop)
    {
      $menuitems = array();
      
      if (isset($menu[$menutop]) && is_array($menu[$menutop]))
      {
        usort($menu[$menutop],array("atkplainmenu","menu_cmp"));
        foreach ($menu[$menutop] as $menuitem)
        {
          $menuitem['id'] = $menuitem['name'];
          $menuitem['enable'] = $menuitem['name'] != '-' && $this->isEnabled($menuitem);
          
          $this->addSubMenuForMenuitem($menu,$menuitem);

          if ($menuitem['name']!=='-')
          {
            $menuitem['name'] = $this->getMenuTranslation($menuitem['name'],$menuitem['module']);
          }

          $theme = &atkinstance("atk.ui.atktheme");
          $menu_icon = $theme->iconPath($menutop.'_'.$menuitem['id'], "menu", $menuitem['module']);
          if ($menu_icon)
          {
            $menuitem['image'] = $menu_icon;
          }

          if (!empty($menuitem['url']))
          {
            $menuitem['url'] = session_url($menuitem['url']."&amp;atkmenutop={$menuitem['id']}", SESSION_NEW);
          }
          $menuitem['header']=$this->getHeader($menuitem['id']);
          $menuitems[] = $menuitem;
        }
      }
      return $menuitems;
    }

    /**
     * Add submenu for menu item
     *
     * @param array $menu
     * @param array $menuitem
     */
    function addSubMenuForMenuItem($menu,&$menuitem)
    {
      // submenu
      if (!isset($menu[$menuitem['name']])) return;

      $menuitem['submenu'] = $menu[$menuitem['name']];
      foreach ($menuitem['submenu'] as $submenukey=>$submenuitem)
      {
        $menuitem['submenu'][$submenukey]['id'] = $menuitem['submenu'][$submenukey]['name'];
        if ($menuitem['submenu'][$submenukey]['name']!=='-')
        {
          $menuitem['submenu'][$submenukey]['name'] = $this->getMenuTranslation($submenuitem['name'],$submenuitem['module']);
        }
        if (!empty($submenuitem['url']))
        {
          if (strpos($submenuitem['url'],"?")!==false)
          {
            $start = "&amp;";
          }
          else
          {
            $start = "?";
          }
          $url = $submenuitem['url'].$start."atkmenutop={$menuitem['id']}";

          $menuitem['submenu'][$submenukey]['enable'] = 
            $menuitem['submenu'][$submenukey]['name'] != '-' && 
            $this->isEnabled($menuitem['submenu'][$submenukey]);
          $menuitem['submenu'][$submenukey]['url'] = session_url($url, SESSION_NEW);
        }
        $theme = &atkinstance("atk.ui.atktheme");
        $menu_icon = $theme->iconPath($menuitem['id'].'_'.$submenuitem['name'], "menu", $submenuitem['module']);
        if ($menu_icon)
        {
          $menuitem['submenu'][$submenukey]['image'] = $menu_icon;
        }
      }
    }

    /**
     * Get the header
     *
     * @param string $atkmenutop
     * @return string Empty string
     */
    function getHeader($atkmenutop)
    {
      return '';
    }
  }

?>
