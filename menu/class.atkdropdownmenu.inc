<?php

  class atkDropdownMenu extends atkmenuinterface
  {
    var $height;

    function atkDropdownMenu() 
    {
      $this->height = "108";
    }

    function render()
    {
      atkimport("atk.ui.atkpage");
      $page = &atkPage::getInstance();
      $ui =   &atknew("atk.ui.atkui");
      $theme = &atkTheme::getInstance();
      $smarty = &atkSmarty::getInstance();
      
      $page->register_style($ui->stylePath("dhtml_dropdown.css"));
      
      $user = getUser();
      
      $tplvars = array(
      "jscode"=>atkconfig("atkroot").'atk/javascript/dhtml_dropdown_events.js',
      "name"=>$user["name"],
      "menubanner"=>$theme->imgPath("menubanner.jpg"),
      "menulogo"=>$theme->imgPath("menulogo.gif"));
      
      $dhtml_dropdown = $ui->render("dhtml_menu.tpl", $tplvars);
      $page->addContent($dhtml_dropdown);
      
      $string = $page->render("Menu", true);
      $output = &atkOutput::getInstance();
      $output->output($string);
      $output->outputFlush();
    }

    function getHeight()
    {
      return $this->height;
    }	

    function getPosition()
    {
      return MENU_TOP;
    }

    function getScrollable()
    {
      return MENU_UNSCROLLABLE;
    }

		function getMultilevel()
		{
			return MENU_MULTILEVEL;
		}
  }

?>