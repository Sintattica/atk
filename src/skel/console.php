<?php
$config_atkroot = ("./");
include_once("atk.php");
Atk_SessionManager::atksession();

if ($_SERVER['HTTP_USER_AGENT'] == "") {
    Atk_ConsoleController::run();
} else {
    echo "This script can only be executed from a console, and not via browser";
}