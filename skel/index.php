<?php
  $config_atkroot = "./";  
  include_once("atk.inc");

  if (atkconfig("fullscreen"))
  {
    // Fullscreen mode. Use index.php as launcher, and launch app.php fullscreen. 
    atksession();
    atksecure();
    
    $page = &atknew("atk.ui.atkpage");  
    $ui = &atknew("atk.ui.atkui");  
    $theme = &atkTheme::getInstance();
    $output = &atkOutput::getInstance();
  
    $page->register_style($theme->stylePath("style.css"));
    $page->register_script(atkconfig("atkroot")."atk/javascript/launcher.js");
    
    $content = '<script language="javascript">atkLaunchApp(); </script>';
    $content.= '<br><br><a href="#" onClick="atkLaunchApp()">'.text('app_reopen').'</a> &nbsp; '.
                      '<a href="#" onClick="window.close()">'.text('app_close').'</a><br><br>';  
    
    $box = $ui->renderBox(array("title"=>text("app_launcher"),
                                              "content"=>$content));
 
    $page->addContent($box);
    $output->output($page->render(text('app_launcher'), true));
  
    $output->outputFlush();      
  }
  else
  {
    // Regular mode. app.php can be included directly.
    include "app.php";
  }
  
?>