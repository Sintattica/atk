<?php
  // Setup the system
  $config_atkroot = "./";
  include_once("atk.inc");

  atksession();
  
  $session = &atkSessionManager::getSession(); 
  
  if($ATK_VARS["atknodetype"]=="" || $session["login"]!=1)
  {
    // no nodetype passed, or session expired
    
    $page = &atknew("atk.ui.atkpage");  
    $ui = &atknew("atk.ui.atkui");  
    $theme = &atkTheme::getInstance();
    $output = &atkOutput::getInstance();
  
    $page->register_style($theme->stylePath("style.css"));
  
    $box = $ui->renderBox(array("title"=>text("title_session_expired"),
                                "content"=>"<br><br>".text("explain_session_expired")."<br><br><br><br>"));
 
    $page->addContent($box);

    $output->output($page->render(text("title_session_expired"), true));              
  }
  else
  {
    atksecure();

    atklock();

    // Create node
    $obj = &getNode($ATK_VARS["atknodetype"]);

    if (is_object($obj))
    {
      $obj->dispatch($ATK_VARS);
    }
    else
    {
      atkdebug("No object created!!?!");
    }
  }
  $output = &atkOutput::getInstance();
  $output->outputFlush();
?>
