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
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 4122 $
   * $Id$
   */

  /**
   * The atkActionListener baseclass for handling ATK events.
   *
   * The most useful purpose of the atkActionListener is to serve as a base
   * class for custom action listeners. Extend this class and override only
   * the notify($action, $record) method. Using atkNode::addListener you can
   * add listeners that catch evens such as records updates and additions.
   * This is much like the classic atk postUpdate/postAdd triggers, only much
   * more flexible.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   */
  class atkActionListener
  {
    /**
     * The list of actions the action listener should listen to.
     * @access private
     * @var Array
     */
    var $m_actionfilter = array();

    /**
     * The owning node of the listener.
     * @access private
     * @var atkNode
     */
    var $m_node = NULL;

    /**
     * Base constructor
     *
     * @param array $actionfilter The list of actions to listen to
     * @return atkActionListener
     */
    function atkActionListener($actionfilter=array())
    {
      $this->m_actionfilter = $actionfilter;
    }

    /**
     * Set the owning node of the listener.
     *
     * When using atkNode::addListener to add a listener to a node it is not
     * necessary to call this method as addListener will do that for you.
     *
     * @param atkNode $node The node to set as owner
     */
    function setNode(&$node)
    {
      $this->m_node = &$node;
    }

    /**
     * Notify the listener of any action on a record.
     *
     * This method is called by the framework for each action called on a
     * node. Depending on the actionfilter passed in the constructor, the
     * call is forwarded to the actionPerformed($action, $record) method.
     *
     * @param String $action The action being performed
     * @param array $record The record on which the action is performed
     */
    function notify($action, $record)
    {
      if (count($this->m_actionfilter)==0 || atk_in_array($action, $this->m_actionfilter))
      {
        atkdebug("Action $action performed on ".$this->m_node->atkNodeType()." (".$this->m_node->primaryKey($record).")");
        $this->actionPerformed($action, $record);
      }
    }

    /**
     * Notify the listener of an action on a record.
     *
     * This method should be overriden in custom action listeners, to catch
     * the action event.
     * @abstract
     * @param String $action The action being performed
     * @param array $record The record on which the action is performed
     */
    function actionPerformed($action, $record)
    {
    }

    /**
     * Notify the listener of any action about to be performed on a record.
     *
     * This method is called by the framework for each action called on a
     * node. Depending on the actionfilter passed in the constructor, the
     * call is forwarded to the preActionPerformed($action, $record) method.
     *
     * @param String $action The action about to be performed
     * @param array $record The record on which the action is about to be performed
     */
    function preNotify($action, &$record)
    {
      if (count($this->m_actionfilter)==0 || atk_in_array($action, $this->m_actionfilter))
      {
        atkdebug("Action $action to be performed on ".$this->m_node->atkNodeType()." (".$this->m_node->primaryKey($record).")");
        $this->preActionPerformed($action, $record);
      }
    }

    /**
     * Notify the listener of an action about to be performed on a record.
     *
     * This method should be overriden in custom action listeners, to catch
     * the action event.
     * @abstract
     * @param String $action The action about to be performed
     * @param array $record The record on which the action is about to be performed
     */
    function preActionPerformed($action, &$record)
    {
    }

  }

?>
