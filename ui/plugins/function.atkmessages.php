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
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5442 $
   * $Id$
   */

  atkimport("atk.utils.atkmessagequeue");

  /**
   * Implements the {atkmessages} plugin for use in templates.
   *
   * The {atkmessages} tag does not output anything. Instead, it loads
   * the messages into the template variable {$atkmessages}, which is
   * an array of elements, each with a single message.
   *
   * <b>Example:</b>
   * <code>
   *   {atkmessages}
   *
   *   {foreach from=$atkmessages item=message}
   *     {$message.message}<br>
   *   {/foreach}
   * </code>
   *
   * @author Patrick van der Velden <patrick@ibuildings.nl>
   *
   */
  function smarty_function_atkmessages($params, &$smarty)
  {
    global $g_sessionManager;
    if (is_object($g_sessionManager))
    {
      $msgs =  atkMessageQueue::getMessages();
      $smarty->assign("atkmessages", $msgs);
      if (empty($msgs))
      {
        atkdebug("No messages in atkMessageQueue");
      }
      return "";
    }
    return "";
  }

?>