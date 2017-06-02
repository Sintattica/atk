<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage ui
 *
 * @copyright (c)2000-2004 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 1684 $
 * $Id$
 */

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
 *
 */
function smarty_function_stacktrace($params, &$smarty)
{
    if (atkConfig::getGlobal('stacktrace')) {
        global $g_sessionManager;
        if (is_object($g_sessionManager)) {
            $smarty->assign("stacktrace", $g_sessionManager->stackTrace());
            return "";
        }
    }
    return "";
}

