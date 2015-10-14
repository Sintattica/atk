<?php
$config_atkroot = "./";
include_once("atk.php");

$atkserver = Atk_Tools::atkinstance("atk.interface.atkserver");
$atkserver->run();