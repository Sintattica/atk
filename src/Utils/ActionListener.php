<?php

namespace Sintattica\Atk\Utils;

use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;

/**
 * The ActionListener baseclass for handling ATK events.
 *
 * The most useful purpose of the atkActionListener is to serve as a base
 * class for custom action listeners. Extend this class and override only
 * the notify($action, $record) method. Using Node::addListener you can
 * add listeners that catch evens such as records updates and additions.
 * This is much like the classic atk postUpdate/postAdd triggers, only much
 * more flexible.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ActionListener
{
    /*
     * The list of actions the action listener should listen to.
     * @access private
     * @var array
     */
    public $m_actionfilter = [];

    /*
     * The owning node of the listener.
     * @access private
     * @var Node
     */
    public $m_node = null;

    /**
     * Base constructor.
     *
     * @param array $actionfilter The list of actions to listen to
     *
     * @return ActionListener
     */
    public function __construct($actionfilter = array())
    {
        $this->m_actionfilter = $actionfilter;
    }

    /**
     * Set the owning node of the listener.
     *
     * When using Node::addListener to add a listener to a node it is not
     * necessary to call this method as addListener will do that for you.
     *
     * @param Node $node The node to set as owner
     */
    public function setNode($node)
    {
        $this->m_node = $node;
    }

    /**
     * Notify the listener of any action on a record.
     *
     * This method is called by the framework for each action called on a
     * node. Depending on the actionfilter passed in the constructor, the
     * call is forwarded to the actionPerformed($action, $record) method.
     *
     * @param string $action The action being performed
     * @param array $record The record on which the action is performed
     */
    public function notify($action, $record)
    {
        if (Tools::count($this->m_actionfilter) == 0 || Tools::atk_in_array($action, $this->m_actionfilter)) {
            Tools::atkdebug("Action $action performed on ".$this->m_node->atkNodeUri().' ('.$this->m_node->primaryKeyString($record).')');
            $this->actionPerformed($action, $record);
        }
    }

    /**
     * Notify the listener of an action on a record.
     *
     * This method should be overriden in custom action listeners, to catch
     * the action event.
     *
     * @abstract
     *
     * @param string $action The action being performed
     * @param array $record The record on which the action is performed
     */
    public function actionPerformed($action, $record)
    {
    }

    /**
     * Notify the listener of any action about to be performed on a record.
     *
     * This method is called by the framework for each action called on a
     * node. Depending on the actionfilter passed in the constructor, the
     * call is forwarded to the preActionPerformed($action, $record) method.
     *
     * @param string $action The action about to be performed
     * @param array $record The record on which the action is about to be performed
     */
    public function preNotify($action, &$record)
    {
        if (Tools::count($this->m_actionfilter) == 0 || Tools::atk_in_array($action, $this->m_actionfilter)) {
            Tools::atkdebug("Action $action to be performed on ".$this->m_node->atkNodeUri().' ('.$this->m_node->primaryKeyString($record).')');
            $this->preActionPerformed($action, $record);
        }
    }

    /**
     * Notify the listener of an action about to be performed on a record.
     *
     * This method should be overriden in custom action listeners, to catch
     * the action event.
     *
     * @abstract
     *
     * @param string $action The action about to be performed
     * @param array $record The record on which the action is about to be performed
     */
    public function preActionPerformed($action, &$record)
    {
    }
}
