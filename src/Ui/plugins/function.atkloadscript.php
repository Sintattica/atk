<?php

use Sintattica\Atk\Ui\Page;

/**
 * Implements the {atkloadscript} plugin for use in templates.
 *
 * The atkloadscript plugin registers a javascript code in the current page
 * to be used in the onLoad of the body.
 *
 * Params:
 * 0/code The javascript code to load.
 *
 * Example:
 * {atkscript "alert('Hello World!');"}
 *
 * @author Boy Baukema <boy@achievo.org>
 */
function smarty_function_atkloadscript($params)
{
    $page = Page::getInstance();
    $page->register_loadscript($params[0] ? $params[0] : $params['code']);
}
