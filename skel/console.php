<?php

  $config_atkroot = ("./");
  include_once("atk.inc");
  atksession();
 
  if($_SERVER['HTTP_USER_AGENT']=="")
  {
    atkConsoleController::run();
  }else
  {
    echo "This script can only be executed from a console, and not via browser";  
  }
  
?>