<?php
  // Setup the system
  $config_atkroot = "./";
  require_once($config_atkroot."atk/class.atknode.inc");
  
  if (count($HTTP_POST_VARS)>0) $HTTP_GET_VARS = $HTTP_POST_VARS;

   atkDataDecode(&$HTTP_GET_VARS);
   $g_sessionManager->session_read(&$HTTP_GET_VARS);
   
   // Create node
   $obj = getNode($HTTP_GET_VARS["atknodetype"]); 

   if (is_object($obj))
   {
     $obj->dispatch($HTTP_GET_VARS);     
   }
   else
   {
     atkdebug("No object created!!?!");
   }

  $g_layout->outputFlush();
?>
