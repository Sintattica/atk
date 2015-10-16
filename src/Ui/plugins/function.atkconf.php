<?php
use Sintattica\Atk\Core\Config;

/**
 * Function for getting a configuration var from atkConfig and set this in a variable in smarty.
 *
 * @author Matthijs van den Bos <matthijs@ibuildings.nl>
 */
function smarty_function_atkconf($params, &$smarty)
{
    $val = Config::get($params['module'], $params['key']);
    return $val;
}
