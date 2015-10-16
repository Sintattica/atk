<?php

use Sintattica\Atk\Core\Controller;

/**
 * Returns the dispatch file.
 *
 * @author Sandy Pleyte <sandy@achievo.org>
 *
 */
function smarty_function_atkdispatchfile($params, &$smarty)
{
    $c = Controller::getInstance();
    return $c->getPhpFile();
}
