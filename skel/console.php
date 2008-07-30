<?php

  $config_atkroot = ("./");
  include_once("atk.inc");
  atksession();
 
  atkConsoleController::run();
  
?>