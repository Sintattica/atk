<?php
  /**
   * Builds a navigation menu for the ATK application. 
   * Items for the main application can be added within the
   * config.menu.inc file with the menuitem() method. Modules
   * can register menuitems in their constructor. The menu
   * has support for enabling/disabling items on a user profile base.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @author Ivo Jansch    <ivo@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$   
   */  
  $config_atkroot = "./";
  include_once("atk.inc");    

  atksession();
  atksecure();
  
  $output = &atkOutput::getInstance();
  $page = &atknew("atk.ui.atkpage");
  $theme = &atkTheme::getInstance();  
  $ui = &atknew("atk.ui.atkui");


  /* general menu stuff */
  include_once($config_atkroot."atk/menu/general.inc");  

  /* load menu layout */
  atkimport("atk.menu.atkmenu");
  $menu = &atkMenu::getMenu();
  
  $menu->render();

?>
