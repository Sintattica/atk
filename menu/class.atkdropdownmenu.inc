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
   * Implementation of the dhtml dropdown menu.
   *
   * @author Ber Dohmen <ber@ibuildings.nl>
   * @package atk
   * @subpackage menu
   */
  class atkDropdownMenu extends atkmenuinterface
  {
    var $height;

    function atkDropdownMenu() 
    {
      $this->height = "23";
    }

    function render()
    {
      atkimport("atk.ui.atkpage");
      $page = &atkPage::getInstance();
      $ui =   &atknew("atk.ui.atkui");
      $theme = &atkTheme::getInstance();
      
      $page->register_style($ui->stylePath("dhtml_dropdown.css"));
      
      $user = getUser();
      $error = $this->checkDataForErrors();
      
      $tplvars = array(
      "jscode"=>atkconfig("atkroot").'atk/javascript/dhtml_dropdown_events.js',
      "name"=>$user["name"],
      "menubanner"=>$theme->imgPath("menubanner.jpg"),
      "menulogo"=>$theme->imgPath("menulogo.gif"));
      if ($error) $tplvars["error"]= $error;      
      
      $dhtml_dropdown = $ui->render("dhtml_menu.tpl", $tplvars);
      $page->addContent($dhtml_dropdown);
      
      $string = $page->render("Menu", true);
      $output = &atkOutput::getInstance();
      $output->output($string);
      $output->outputFlush();
    }
    
    function checkDataForErrors()
    {
      $output=array();
      $content = exec("php -Cq ".atkconfig("atkroot")."atk/javascript/dhtml_dropdown_data.js.php externalinclude", $output);
      
      $error = "<b>".array_pop($output)."</b>";
      
      if (strstr($error,"error")) 
      {
        return $error;
      }
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
