<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   *
   * @copyright (c)2006 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * This class implements the ATK message queue for showing messages
   * at the top of a page.
   *
   * @author Patrick van der Velden <patrick@ibuildings.nl>
   * @package atk
   * @subpackage utils
   *
   */
  class atkMessageQueue
  {

    /**
    * Retrieve the atkMessageQueue instance
    *
    * @return atkMessageQueue The instance.
    */
    function &getInstance()
    {
      static $s_instance = NULL;
      if ($s_instance==NULL)
      {
        global $g_sessionManager;
        if (is_object($g_sessionManager)) // don't bother to create if session has not yet been initialised
        {
          $s_instance = new atkMessageQueue();
        }
      }
      return $s_instance;
    }

    /**
     * Constructor
     */
    function atkMessageQueue()
    {
    }

    /**
     * Add message to queue
     *
     * @static
     * @param string $txt
     * @return boolean Success
     */
    function addMessage($txt)
    {
      $instance = &atkMessageQueue::getInstance();
      if (is_object($instance))
      {
        return $instance->_addMessage($txt);
      }
      return false;
    }

    /**
     * Add message to queue (private)
     *
     * @param string $txt
     * @return boolean Success
     */
    function _addMessage($txt)
    {
      $q = &$this->getQueue();
      array_push($q, $txt);
      return true;
    }

    /**
     * Get first message from queue and remove it
     *
     * @static
     * @return string message
     */
    function getMessage()
    {
      $instance = &atkMessageQueue::getInstance();
      if (is_object($instance))
      {
        return $instance->_getMessage();
      }
      return "";
    }

    /**
     * Get first message from queue and remove it (private)
     *
     * @return string message
     */
    function _getMessage()
    {
      $q = &$this->getQueue();
      return array_shift($q);
    }

    /**
     * Get all messages from queue and empty the queue
     *
     * @return array messages
     */
    function getMessages()
    {
      $instance = &atkMessageQueue::getInstance();
      if (is_object($instance))
      {
        return $instance->_getMessages();
      }
      return array();
    }

    /**
     * Get all messages from queue and empty the queue (private)
     *
     * @return array messages
     */
    function _getMessages()
    {
      $q = &$this->getQueue();
      $queue_copy = $q;
      $q = array();
      return $queue_copy;
    }

    function &getQueue()
    {
      $sessionmgr = &atkGetSessionManager();
      $session = &$sessionmgr->getSession();
      if (!isset($session['atkmessagequeue']))
      {
        $session['atkmessagequeue'] = array();
      }
      return $session['atkmessagequeue'];
    }
  }

?>