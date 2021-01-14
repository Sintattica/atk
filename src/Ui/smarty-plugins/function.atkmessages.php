<?php

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\MessageQueue;
use Sintattica\Atk\Session\SessionManager;

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
 */
function smarty_function_atkmessages($params, $smarty)
{
    $sessionManager = SessionManager::getInstance();
    if (is_object($sessionManager)) {
        $msgs = MessageQueue::getMessages();
        $smarty->assign('atkmessages', $msgs);
        if (empty($msgs)) {
            Tools::atkdebug('No messages in MessageQueue');
        }

        return '';
    }

    return '';
}
