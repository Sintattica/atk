<?php

  define("MENU_TOP", 1);
  define("MENU_LEFT", 2);
  define("MENU_SCROLLABLE", 1);
  define("MENU_UNSCROLLABLE", 2);
	define("MENU_MULTILEVEL", 1); //More then 2 levels supported
	define("MENU_NOMULTILEVEL", 2);

  atkimport("atk.menu.atkmenuinterface");

  class atkMenu
  {   
    var $menuclass;

    function getMenu()
    {
      $menuclass = &atknew("atk.menu."."atk".atkconfig("menu_layout")."menu");
      return $menuclass;
    }
  }
	
?>