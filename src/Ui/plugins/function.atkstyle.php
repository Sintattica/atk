<?php

use Sintattica\Atk\Ui\Page;

/**
 * Implements the {atkstyle} plugin for use in templates.
 *
 * The atkstyle plugin registers a stylesheet in the current page.
 * Useful for templates that have an associated stylesheet that should
 * be loaded each time the template is included.
 *
 * Params:
 * file   The path of the stylesheet, relative to the running scripts
 *        directory.
 * media  The stylesheet media.
 *
 * Example:
 * {atkstyle file="styles/default.css"}
 *
 * @author Ivo Jansch <ivo@achievo.org>
 *
 */
function smarty_function_atkstyle($params, &$smarty)
{
    $page = Page::getInstance();
    $page->register_style($params["file"], $params["media"]);
    return "";
}