<?php

$base = atkConfig::getGlobal('atkroot') . 'atk/ext/momentphp/src/Moment/';

include_once($base . 'Moment.php');
include_once($base . 'MomentException.php');
include_once($base . 'FormatsInterface.php');
include_once($base . 'CustomFormats/MomentJs.php');

class atkMomentphpProvider {


    public static function getFormatInstance()
    {
        static $s_instance = NULL;
        if ($s_instance == NULL) {
            atkdebug("Created a new \Moment\CustomFormats\MomentJs instance");
            $s_instance = new \Moment\CustomFormats\MomentJs();
        }
        return $s_instance;
    }
}