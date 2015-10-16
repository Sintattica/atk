<?php

use Sintattica\Atk\Core\Module;

/**
 * Implements the {atkmoduledir} plugin for use in templates.
 *
 * The atkmoduledir plugin return module path
 * Useful for including custom templates  into other teplate.
 *
 * Params:
 * modulename   module name
 *
 * Example:
 * {atkmoduledir modulename="project"}
 *
 * @author Yury Golovnya <yury@achievo.org>
 *
 */
function smarty_function_atkmoduledir($params, &$smarty)
{
    return $smarty->assign("atkmoduledir", Module::moduleDir($params['modulename']));
}
