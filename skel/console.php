<?php

  $config_atkroot = ("./");
  include_once("atk.inc");
  include_once("modules/nu/feed/class.rssfeedhandler.inc");
  atksession();
  
  atkConsoleController::run($argv); 
  
?>