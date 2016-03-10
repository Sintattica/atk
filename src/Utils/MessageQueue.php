<?php

namespace Sintattica\Atk\Utils;

use Sintattica\Atk\Session\SessionManager;

/**
 * This class implements the ATK message queue for showing messages
 * at the top of a page.
 *
 * @author Patrick van der Velden <patrick@ibuildings.nl>
 */
class MessageQueue
{
    /**
     * Message queue flags.
     */
    const AMQ_GENERAL = 0;
    const AMQ_SUCCESS = 1;
    const AMQ_WARNING = 2;
    const AMQ_FAILURE = 3;

    /**
     * Retrieve the atkMessageQueue instance.
     *
     * @return MessageQueue The instance.
     */
    public static function &getInstance()
    {
        static $s_instance = null;
        if ($s_instance == null) {
            $sessionManager = SessionManager::getInstance();
            if (is_object($sessionManager)) { // don't bother to create if session has not yet been initialised
                $s_instance = new self();
            }
        }

        return $s_instance;
    }

    /**
     * Constructor.
     */
    public function __construct()
    {
    }

    /**
     * Add message to queue.
     *
     * @static
     *
     * @param string $txt
     * @param int $type
     *
     * @return bool Success
     */
    public function addMessage($txt, $type = self::AMQ_GENERAL)
    {
        $instance = self::getInstance();
        if (is_object($instance)) {
            return $instance->_addMessage($txt, $type);
        }

        return false;
    }

    /**
     * Get the name of the message type.
     *
     * @param int $type The message type
     *
     * @return string The name of the message type
     */
    public function _getTypeName($type)
    {
        if ($type == self::AMQ_SUCCESS) {
            return 'success';
        } else {
            if ($type == self::AMQ_FAILURE) {
                return 'failure';
            } else {
                if ($type == self::AMQ_WARNING) {
                    return 'warning';
                } else {
                    return 'general';
                }
            }
        }
    }

    /**
     * Add message to queue (private).
     *
     * @param string $txt
     * @param int $type
     *
     * @return bool Success
     */
    public function _addMessage($txt, $type)
    {
        $q = $this->getQueue();
        $q[] = array('message' => $txt, 'type' => $this->_getTypeName($type));

        return true;
    }

    /**
     * Get first message from queue and remove it.
     *
     * @static
     *
     * @return string message
     */
    public static function getMessage()
    {
        $instance = self::getInstance();
        if (is_object($instance)) {
            return $instance->_getMessage();
        }

        return '';
    }

    /**
     * Get first message from queue and remove it (private).
     *
     * @return string message
     */
    public function _getMessage()
    {
        $q = &$this->getQueue();

        return array_shift($q);
    }

    /**
     * Get all messages from queue and empty the queue.
     *
     * @return array messages
     */
    public static function getMessages()
    {
        $instance = self::getInstance();
        if (is_object($instance)) {
            return $instance->_getMessages();
        }

        return array();
    }

    /**
     * Get all messages from queue and empty the queue (private).
     *
     * @return array messages
     */
    public function _getMessages()
    {
        $q = &$this->getQueue();
        $queue_copy = $q;
        $q = array();

        return $queue_copy;
    }

    /**
     * Get the queue.
     *
     * @return array The message queue
     */
    public function &getQueue()
    {
        $sessionmgr = SessionManager::getInstance();
        $session = &$sessionmgr->getSession();
        if (!isset($session['atkmessagequeue'])) {
            $session['atkmessagequeue'] = array();
        }

        return $session['atkmessagequeue'];
    }
}
