<?php
  $config_atkroot = "./";
  include_once("atk.inc");
  
  $page = &atknew("atk.ui.atkpage");  
  $ui = &atknew("atk.ui.atkui");  
  $theme = &atkTheme::getInstance();
  $output = &atkOutput::getInstance();
  
  $page->register_style($theme->stylePath("style.css"));
  $box = $ui->renderBox(array("title"=>text("app_shorttitle"),
                                            "content"=>"<br><br>".text("app_description")."<br><br>"));
 
  $page->addContent($box);
  $output->output($page->render(text('app_shorttitle'), true));
  
  $output->outputFlush();  

?>
