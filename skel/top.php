<?php

  $config_atkroot = "./";
  include_once("atk.inc"); 

  atksession();
  atksecure();   
  
  $page = &atknew("atk.ui.atkpage");  
  $ui = &atknew("atk.ui.atkui");  
  $theme = &atkTheme::getInstance();
  $output = &atkOutput::getInstance();
  
  $page->register_style($theme->stylePath("style.css"));
  
  $loggedin = text("loggedin_user", "", "atk").": <b>".$g_user["name"]."</b>";  
  $content = '<br>'.$loggedin.' &nbsp; <a href="app.php?atklogout=1" target="_top">.'.ucfirst(text("logout", "", "atk")).'.</a><br>&nbsp;';
  
  $box = $ui->renderBox(array("title"=>text("topframe"),
                                            "content"=>$content));
 
  $page->addContent($box);

  $output->output($page->render(text('txt_app_title'), true));
  
  $output->outputFlush();
?>
