<?php
  /**
   * Builds a navigation menu for the ATK application. 
   * Items for the main application can be added within the
   * config.menu.inc file with the menuitem() method. Modules
   * can register menuitems in their constructor. The menu
   * has support for enabling/disabling items on a user profile base.
   *
   * For more information check the atkmoduletools.inc file!
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$   
   * $Log$
   * Revision 4.20  2001/11/19 14:51:52  peter
   * Fixed bug in menu.php (did not include the general menu stuff).
   * $config_menu_layout can now be anything and defaults to... "default".
   *
   */  
  include_once("atk.inc");  

  atksession();
  atksecure();

  /* get main menuitems */
  include_once("config.menu.inc");

  /* general menu stuff */
  include_once($config_atkroot."atk/menu/general.inc");  

  /* load menu layout */
  include_once($config_atkroot."atk/menu/".$config_menu_layout.".inc");
?>