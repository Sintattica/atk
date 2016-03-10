<?php

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\StringParser;

/**
 * function to get multilanguage strings.
 *
 * This is actually a wrapper for ATK's Tools::atktext() method, for
 * use in templates.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 *
 * Example: {atktext id="users.userinfo.description"}
 *          {atktext id="userinfo.description" module="users"}
 *          {atktext id="description" module="users" node="userinfo"}
 */
function smarty_function_atktext($params, &$smarty)
{
    if (!isset($params['id'])) {
        $params['id'] = $params[0];
    }
    switch (substr_count($params['id'], '.')) {
        case 1: {
            list($module, $id) = explode('.', $params['id']);
            $str = Tools::atktext($id, $module, isset($params['node']) ? $params['node'] : '');
            break;
        }
        case 2: {
            list($module, $node, $id) = explode('.', $params['id']);
            $str = Tools::atktext($id, $module, $node);
            break;
        }
        default:
            $str = Tools::atktext($params['id'], Tools::atkArrayNvl($params, 'module', ''), Tools::atkArrayNvl($params, 'node', ''),
                Tools::atkArrayNvl($params, 'lng', ''));
    }

    if (isset($params['filter'])) {
        $fn = $params['filter'];
        $str = $fn($str);
    }

    // parse the rest of the params in the string
    $parser = new StringParser($str);

    return $parser->parse($params);
}
