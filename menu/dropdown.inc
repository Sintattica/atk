<?php

  atkimport("atk.ui.atkpage");
  $page = &atkPage::getInstance();
  $ui =   &atknew("atk.ui.atkui");  
  $theme = &atkTheme::getInstance();  
  
  $page->register_style("../../platform/atk/style.php");
  $page->register_style($ui->stylePath("dhtml_dropdown.css"));
  
  $user = getUser();
  $name = $user["name"];
  
  $dhtml_dropdown = '<script language="JavaScript" src="'.atkconfig("atkroot").'atk/javascript/dhtml_dropdown_events.js" type="text/javascript"></script>';
  
  $dhtml_dropdown .= '
		<TABLE style="border: 1px solid black" width="100%" cellpadding="0" cellspacing="0">
			<TR>
				<TD style="border: 0px solid black; border-bottom-width: 1px" bgcolor="#2B98F7" height="26">
					<div align="center"><small style="color:#FFFFFF;font-size:11pt;font-family: Verdana,Helvetica,Arial"><b>'.strtoupper(text("app_title")).'</b></small></div>
				</TD>
			</TR>
			<TR>
				<TD bgcolor="#D6DDF7" height="22">
					<div align="left" style="margin-left: 15px"><small style="color:#666666;font-size:8pt;font-family: Verdana,Helvetica,Arial">Logged in as: '.$name.'</small></div>
				</TD>
			</TR>
		</TABLE>
	';
  
  $page->addContent($dhtml_dropdown);
  
  $string = $page->render("Menu", true);
  $output = &atkOutput::getInstance();
  $output->output($string);
  $output->outputFlush();
  
?>