<?php
/**
 * atkFrontController plug-in.
 *
 * @param array $params
 * @param Smarty $smarty
 * @return string result
 */
function smarty_function_atkfrontcontroller($params, $smarty)
{
  atkimport('atk.front.atkfrontcontroller');
  return atkFrontController::dispatchRequest($params);
}