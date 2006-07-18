<?php

atkimport("atk.menu.atkplainmenu");

class atkModernMenu extends atkPlainMenu
{
  function atkModernMenu()
  {
    $this->m_height = "130";
  }
  
  function render()
  {
    $theme = &atkinstance("atk.ui.atktheme");
    if ($theme->getAttribute('modern_menu_compat'))
    {
      global $g_menu;
      $atkmenutop="main";

      $menuitems = $this->getMenuItems($g_menu,$atkmenutop);

      $page = &atknew("atk.ui.atkpage");
      $page->register_style($theme->stylePath("style.css"));
      $page->register_style($theme->stylePath("menu.css"));
      $page->register_script(atkconfig("atkroot")."atk/javascript/menuload.js");

      $ui = &atkinstance("atk.ui.atkui");
      $atkmenutop=($_REQUEST['atkmenutop']?$_REQUEST['atkmenutop']:$atkmenutop);

      $box = $ui->renderBox(array("title"=>$this->getMenuTranslation($atkmenutop),
      "menuitems"=>$menuitems,
      'menutop'=>$atkmenutop,
      'g_menu'=>$g_menu,
      'atkmenutop'=>$atkmenutop,
      'atkmenutopname'=>$this->getMenuTopName($menuitems,$atkmenutop)),"menu");

      $page->addContent($box);

      return $page->render("Menu", true);
    }
    else
    {
      $oldmenu = &atknew('atk.menu.atkplainmenu');
      return $oldmenu->render();
    }
  }

  function getMenuTopName($menuitems,$menutop)
  {
    foreach ($menuitems as $menuitem)
    {
      if ($menuitem['id']==$menutop) return $menuitem['name'];
    }
  }

  function getMenuItems($menu, $menutop)
  {
    $menuitems = array();
    if (is_array($menu[$menutop]))
    {
      usort($menu[$menutop],array("atkplainmenu","menu_cmp"));
      foreach ($menu[$menutop] as $menuitem)
      {
        $this->setEnabled($menuitem);

        $this->addSubMenuForMenuitem($menu,$menuitem);
        if ($menuitem['name']!=='-')
        {
          $menuitem['id'] = $menuitem['name'];
          $menuitem['name'] = $this->getMenuTranslation($menuitem['name'],$menuitem['module']);
        }
        if (!empty($menuitem['url']))
        {
          $menuitem['url'] = session_url($menuitem['url'], SESSION_NEW);
        }
        $menuitem['header']=$this->getHeader($menuitem['id']);
        $menuitems[] = $menuitem;
      }
    }
    return $menuitems;
  }

  function addSubMenuForMenuItem($menu,&$menuitem)
  {
    // submenu
    if ($menu[$menuitem['name']])
    {
      $enablesubmenu=false;
      for ($i=0;$i<count($menu[$menuitem['name']]);$i++)
      {
        if ($this->setEnabled($menu[$menuitem['name']][$i]))
        {
          $enablesubmenu = true;
        }
      }
      if ($enablesubmenu)
      {
        $menuitem['submenu'] = $menu[$menuitem['name']];
        foreach ($menuitem['submenu'] as $submenukey=>$submenuitem)
        {
          if ($menuitem['submenu'][$submenukey]['name']!=='-')
          {
            $menuitem['submenu'][$submenukey]['name'] = $this->getMenuTranslation($submenuitem['name'],$submenuitem['module']);
          }
          if (!empty($submenuitem['url']))
          {
            $menuitem['submenu'][$submenukey]['url'] = session_url($submenuitem['url'], SESSION_NEW);
          }
        }
      }
    }
  }

  function setEnabled(&$menuitem)
  {
    if (is_array($menuitem["enable"]))
    {
      $enabled = false;
      for ($j=0;$j<(count($menuitem["enable"])/2);$j++)
      {
        $enabled |= is_allowed($menuitem["enable"][(2*$j)],$menuitem["enable"][(2*$j)+1]);
      }
      $menuitem["enable"]=$enabled;
    }
    else
      $enabled = $menuitem["enable"];

    return ($menuitem["name"]==='-'?false:$enabled);
  }

  function getHeader($atkmenutop)
  {
    return '';
  }
}

?>