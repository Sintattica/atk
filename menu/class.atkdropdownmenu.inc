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
      
      $menubanner = $theme->imgPath("menubanner.jpg");
      $menulogo = $theme->imgPath("menulogo.gif");
      
      $page->register_style($ui->stylePath("dhtml_dropdown.css"));
      
      $user = getUser();
      $name = $user["name"];
      
      $dhtml_dropdown = '<script language="JavaScript" src="'.atkconfig("atkroot").'atk/javascript/dhtml_dropdown_events.js" type="text/javascript"></script>';
      
      $dhtml_dropdown .= '
		<table width="100%" cellpadding="0" cellspacing="0">
			<tr>
				<td height="26" bgcolor="#52006A"><img src="'.$menubanner.'" class="topbg"></td>
				<td align="right" bgcolor="#52006A"><img src="'.$menulogo.'" class="topbg"></td>
			</tr>
			<tr>
				<td height="22" align="left" class="menubg" style="background: #FF0000; color: #FFFFFF; font-family: verdana; font-size: 11px; padding-left: 12px;">Logged in as: <b>'.$name.'</b></td>
				<td class="menubg" bgcolor="#FF0000">&nbsp;</td>
			</tr>
		</table>
	';
      
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