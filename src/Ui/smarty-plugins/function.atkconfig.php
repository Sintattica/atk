<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 *
 * @copyright (c)2004 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 4599 $
 * $Id$
 */
use Sintattica\Atk\Core\Config;

/**
 * Function for getting a configuration variable and set this in a variable in smarty.
 *
 * @author Lineke Kerckhoffs-Willems <lineke@ibuildings.nl>
 */
function smarty_function_atkconfig($params, $smarty)
{
    $smarty->assign($params['smartyvar'], Config::getGlobal($params['var']));
}
