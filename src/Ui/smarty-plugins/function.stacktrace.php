<?php

use Sintattica\Atk\Session\SessionManager;

/**
 * Implements the {stacktrace} plugin for use in templates.
 *
 * The {stacktrace} tag does not output anything. Instead, it loads
 * a stacktrace into the template variables {$stacktrace}, which is
 * an array of elements, each with a 'title' and 'url' field.
 *
 * <b>Example:</b>
 * <code>
 *   {stacktrace}
 *
 *   {foreach from=$stacktrace item=item}
 *     <a href="{$item.url}">{$item.title}</a>
 *   {/foreach}
 * </code>
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
function smarty_function_stacktrace($params, $smarty)
{
    $sessionManager = SessionManager::getInstance();
    if (is_object($sessionManager)) {
        $smarty->assign('stacktrace', $sessionManager->stackTrace());

        return '';
    }

    return '';
}
