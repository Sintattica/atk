<?php

  $config_atkroot = "./";
  include_once("atk.inc");
  
  $atkserver = atkinstance("atk.interface.atkserver");
  $atkserver->run();

?>
