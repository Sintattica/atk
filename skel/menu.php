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
   *
   */  
  include_once("atk.inc");  

  atksession();
  atksecure();

  /* get main menuitems */
  include_once("config.menu.inc");
  
  if ($config_menu_layout=="outlook")
  {
    include_once($config_atkroot."atk/menu/outlook.inc");
  }
  else
  {
    include_once($config_atkroot."atk/menu/default.inc");
  }
  
?>