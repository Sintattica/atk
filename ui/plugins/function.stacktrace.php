<?php

/**
 * Smarty plugin
 * -------------------------------------------------------------
 * Type:     function
 * Name:     stacktrace 
 * @Author:	 Ivo Jansch <ivo@achievo.org>
 *
 * @Version:  $Revision$ 
 *
 * The {stacktrace} tag does not output anything. Instead, it loads
 * a stacktrace into the template variables {$stacktrace}, which is
 * an array of elements, each with a 'title' and 'url' field.
 *
 * Example:
 *
 * {stacktrace}
 * 
 * {foreach from=$stacktrace item=item}
 *   <a href="{$item.url}">{$item.title}</a> 
 * {/foreach}
 * 
 * $Id$
 */
function smarty_function_stacktrace($params, &$smarty)
{
  global $g_sessionManager;
  if (is_object($g_sessionManager))
  {
    $smarty->assign("stacktrace",$g_sessionManager->stackTrace());
    return "";
  }
  return "";
}

/* vim: set expandtab: */

?>
