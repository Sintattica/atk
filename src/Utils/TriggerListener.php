<?php

namespace Sintattica\Atk\Utils;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;

/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 *
 * @copyright (c)2005 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 4362 $
 * $Id$
 */

/**
 * The atkTriggerListener base class for handling trigger events on records.
 *
 * The most useful purpose of the atkTriggerListener is to serve as a base
 * class for custom trigger listeners. Extend this class and implement
 * postUpdate, preDelete etc. functions that will automatically be called
 * when such a trigger occurs. For more flexibility, override only
 * the notify($trigger, $record) method which catches every trigger.
 * Using Node::addListener you can add listeners that catch evens such as
 * records updates and additions.
 * This is much like the classic atk postUpdate/postAdd triggers, only much
 * more flexible.
 *
 * @author Martin Roest <martin@ibuildings.nl>
 * @author Peter C. Verhage <peter@achievo.org>
 */
class TriggerListener
{
    /**
     * The owning node of the listener.
     * @access private
     * @var Node $m_node
     */
    public $m_node = null;

    /**
     * Base constructor.
     *
     * @return TriggerListener
     */
    public function __construct()
    {
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
     * @param string $trigger The trigger being performed
     * @param array $record The record on which the trigger is performed
     * @param string $mode The mode (add/update)
     *
     * @return bool Result of operation.
     */
    public function notify($trigger, &$record, $mode = null)
    {
        if (method_exists($this, $trigger)) {
            Tools::atkdebug('Call listener '.get_class($this)." for trigger $trigger on ".$this->m_node->atkNodeUri().' ('.$this->m_node->primaryKeyString($record).')');

            return $this->$trigger($record, $mode);
        } else {
            return true;
        }
    }
}
