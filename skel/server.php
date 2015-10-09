<?php
$config_atkroot = "./";
include_once("atk.inc");

$atkserver = atkTools::atkinstance("atk.interface.atkserver");
$atkserver->run();
?>
