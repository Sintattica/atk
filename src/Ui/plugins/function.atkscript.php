<?php

use Sintattica\Atk\Core\Tools;


/**
 * Implements the {atkscript} plugin for use in templates.
 *
 * The atkscript plugin registers a javascript file in the current page.
 * Useful for templates that have an associated javascript that should
 * be loaded each time the template is included.
 *
 * Params:
 * file   The path of the javascript, relative to the running scripts
 *        directory.
 * prefix The prefix for the path of the javascript file,
 *        for example $atkroot
 *
 * Example:
 * {atkscript file="javascript/default.js"}
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
function smarty_function_atkscript($params, &$smarty)
{
    $page = Tools::atkinstance('atk.ui.atkpage');
    $page->register_script($params["prefix"] . $params["file"]);
    return "";
}
