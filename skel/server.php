<?php
$config_atkroot = "./";
include_once("atk.php");

$atkserver = atkTools::atkinstance("atk.interface.atkserver");
$atkserver->run();