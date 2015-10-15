<?php

use Sintattica\Atk\Front\FrontController;

/**
 * atkFrontController plug-in.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @param array $params
 * @param Smarty $smarty
 * @return string result
 */
function smarty_function_atkfrontcontroller($params, $smarty)
{
    return FrontController::dispatchRequest($params);
}
