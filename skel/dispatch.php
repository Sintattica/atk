<?php
  // Setup the system  
  
  include_once("atk.inc");
  
  atksession();
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
