<?php

  /**
   * This file is part of the Achievo ATK distribution.
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
   * @version $Revision$
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
   * @package atk
   * @subpackage ui
   *
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

?>
