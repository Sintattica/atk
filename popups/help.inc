<?php

 /* help.php
  *
  * descr.  : Opens a new page in the same atk-template style as the atk-application, in a new pop-up screen and shows a help page.
  * input   : $node     -> name of the to be included text file ( help.[node].inc) 
  * example : help.php?node=[node]
  *
  * @author : Rene Bakx (rene@ibuildings.nl)
  * @version: $Revision$
  *
  * $Id$
  *
  */
  
  include_once($config_atkroot."atk.inc");  
  
  atksession();
  atksecure();  

//  Renders the help screen
  $language = strtok(atkconfig("languagefile"),".");
  $title1 ="txt_title_".$node;
  $title = $$title1;
  $file   = $config_atkroot."help/".$language."/help.".$node.".inc";
  $data = '<div align="left">';
  $data .= implode("<br>",file($file));
  $data .='</div>';
    
  $page = &atknew("atk.ui.atkpage");  
  $ui = &atknew("atk.ui.atkui");  
  $theme = &atkTheme::getInstance();
  $output = &atkOutput::getInstance();
  
  $page->register_style($theme->stylePath("style.css"));
  
  $output.= $ui->renderBox(array("title"=>$title,
                                 "content"=>$data));
  $output.='<br><div align="right"><a href="javascript:window.close();">'.text("close").'</a></div>';
  
  $page->addContent($output);

  $output->output($page->render(text('txt_app_title').' - '.text('help'), true));
  
  $output->outputFlush();

?>


