<?php
  // Setup the system
  $config_atkroot = "./";
  require_once($config_atkroot."atk/class.atknode.inc");
  
  if (count($HTTP_POST_VARS)>0)
  {  
    atkDataDecode(&$HTTP_POST_VARS);   
    $ATK_VARS = $HTTP_POST_VARS;
  }
  else
  {
    atkDataDecode(&$HTTP_GET_VARS);   
    $ATK_VARS = $HTTP_GET_VARS;
  }
   
  atksession("admin");
  $g_sessionManager->session_read(&$ATK_VARS);
  atksecure();
   
  // Create node
  $obj = getNode($ATK_VARS["atknodetype"]); 

  if (is_object($obj))
  {
    $obj->dispatch($ATK_VARS);     
  }
  else
  {
    atkdebug("No object created!!?!");
  }

  $g_layout->outputFlush();
?>
