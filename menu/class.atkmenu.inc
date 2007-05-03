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
  define("MENU_BOTTOM", 3);
  define("MENU_RIGHT", 4);
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

    function layoutToClass($layout)
    {
      // Check if the menu config is one of the default atk menus (deprecated)
      if (in_array($layout, array("plain","frames","outlook","dhtml","modern","cook")))
      {
        $classname = "atk.menu.atk".$layout."menu";
      }

      // Application root menu directory (deprecated)
      elseif (strpos($layout, '.') === FALSE)
      {
        $classname = "menu.".$layout;
      }

      // Full class name with packages.
      else
      {
        $classname = $layout;
      }
      return $classname;
    }

    function getMenuClass()
    {
      // Get the configured layout class
      $classname =  atkMenu::layoutToClass(atkconfig("menu_layout"));
      atkdebug("Configured menu layout class: $classname");

      // Check if the class is compatible with the current theme, if not use a compatible menu.
      $theme = &atkinstance("atk.ui.atktheme");
      $compatiblemenus = $theme->getAttribute('compatible_menus');
      // If this attribute exists then retreive them
      if(is_array($compatiblemenus))
      {
        for($i=0,$_i=count($compatiblemenus); $i<$_i; $i++)
          $compatiblemenus[$i] = atkMenu::layoutToClass($compatiblemenus[$i]);
      }

      if (!empty($compatiblemenus) && is_array($compatiblemenus) && !in_array($classname, $compatiblemenus))
      {
        $classname = $compatiblemenus[0];
        atkdebug("Falling back to menu layout class: $classname");
      }

      // Return the layout class name
      return $classname;
    }

    /**
     * Get new menu object
     *
     * @return object Menu class object
     */
    function &getMenu()
    {
      static $s_instance = NULL;
      if ($s_instance==NULL)
      {
        atkdebug("Creating a new menu instance");
        $classname = atkMenu::getMenuClass();


        $filename = getClassPath($classname);
        if (file_exists($filename)) $s_instance = atknew($classname);
        else
        {
          atkerror('Failed to get menu object ('.$filename.' / '.$classname.')!');
          atkwarning('Please check your compatible_menus in themedef.inc and config_menu_layout in config.inc.php.');
          $s_instance = atknew('atk.menu.atkplainmenu');
        }
        atkHarvestModules("getMenuItems");
      }

      return $s_instance;
    }

  }

?>