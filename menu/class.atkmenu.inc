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
  define("MENU_SCROLLABLE", 1);
  define("MENU_UNSCROLLABLE", 2);
	define("MENU_MULTILEVEL", 1); //More then 2 levels supported
	define("MENU_NOMULTILEVEL", 2);

  atkimport("atk.menu.atkmenuinterface");

  /**
   * Menu utility class.
   *
   * This class is used to retrieve the instance of an atkMenuInterface-based
   * class, as defined in the configuration file.
   *
   * @author Ber Dohmen <ber@ibuildings.nl>
   * @package atk
   * @subpackage menu
   */
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