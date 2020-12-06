<?php

include_once __DIR__ . '/../vendor/autoload.php';

$env = getenv('APP_ENV');
if(!$env || !in_array($env, ['dev', 'staging', 'prod'])){
    die('APP_ENV must be set!');
}

$atk = new Sintattica\Atk\Core\Atk($env, __DIR__ . '/../');
$atk->run();
