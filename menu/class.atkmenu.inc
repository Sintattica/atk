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
   * Some defines
   */
  define("MENU_TOP", 1);
  define("MENU_LEFT", 2);
  define("MENU_BOTTOM", 1);
  define("MENU_RIGHT", 1);
  define("MENU_SCROLLABLE", 1);
  define("MENU_UNSCROLLABLE", 2);
	define("MENU_MULTILEVEL", 1); //More then 2 levels supported
	define("MENU_NOMULTILEVEL", 2);

  atkimport("atk.menu.atkmenuinterface");
  
  include_once(atkconfig("atkroot")."atk/atkmenutools.inc");

  /**
   * Menu utility class.
   *
   * This class is used to retrieve the instance of an atkMenuInterface-based
   * class, as defined in the configuration file.
   *
   * @author Ber Dohmen <ber@ibuildings.nl>
   * @author Sandy Pleyte <sandy@ibuildings.nl> 
   * @package atk
   * @subpackage menu
   */
  class atkMenu
  {   
    /**
     * Get new menu object
     * 
     * @return object Menu class object
     */
    function getMenu()
    {
      atkMenu::harvestMenus();
      // Check if the menu config is one of the defail atk menus
      if(!in_array(atkconfig("menu_layout"),array("plain","frames","outlook","dhtml")))
      { 
        // Check the application menu dir for a class.[menu_layout].inc file
        $menuclass = &atknew("menu.".atkconfig("menu_layout"));
        if(!is_object($menuclass)) echo "Error menu class.".atkconfig("menu_layout").".inc doesn't exist"; 
      }
      else
      {
        $menuclass = &atknew("atk.menu."."atk".atkconfig("menu_layout")."menu");
      }
      return $menuclass;
    }
    
    /**
     * Harvest for menus in all modules
     */
    function harvestMenus()
    {
      global $g_modules,$g_moduleflags;
      foreach ($g_modules as $modname => $modpath)
      {
        if (!isset($g_moduleflags[$modname])||!hasFlag($g_moduleflags[$modname], MF_NOMENU))
        {
          $mod = &getModule($modname);
          if (method_exists($mod,"getMenuItems")) $mod->getMenuItems();
        }
      }
    }
  }

?>