<?php

use Sintattica\Atk\Utils\JSON;

/**
 * Modifier to encode a Smarty/PHP variable to JSON.
 *
 * Example of usage:
 * {$var|@atkjson}
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
function smarty_modifier_atkjson($data)
{
    return JSON::encode($data);
}
