<?php

/** @internal includes */

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Module;

require_once(__DIR__.'/adodb-time.php');

require_once(__DIR__ . '/basics.php');
require_once(__DIR__ . '/compatibility.php');
if (Config::getGlobal('autoload_classes', true)) {
    require_once(__DIR__ . '/autoload.php');
}
if (Config::getGlobal('use_atkerrorhandler', true)) {
    require_once(__DIR__ . '/errorhandler.php');
}

require_once(__DIR__ . '/security.php');
require_once(__DIR__ . '/debugging.php');

Module::atkPreloadModules();

