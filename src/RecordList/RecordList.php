<?php

namespace Sintattica\Atk\RecordList;

use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

abstract class RecordList
{
    /** recordlist flags */
    const RL_NO_SORT = 1; // recordlist is not sortable
    const RL_NO_SEARCH = 2; // recordlist is not searchable
    const RL_NO_EXTENDED_SEARCH = 4; // recordlist is not searchable
    const RL_EMBED = 8; // recordlist is embedded
    const RL_MRA = 16; // multi-record-actions enabled
    const RL_MRPA = 32; // multi-record-priority-actions enabled
    const RL_EXT_SORT = 128; // extended sort feature

    /* @var Node $m_node */
    public $m_node;

    public $m_flags = 0;
    public $m_actionloader;
    public $m_masternode = null;
    public $m_hasActionColumn = 0;
    public $m_actionSessionStatus = SessionManager::SESSION_NESTED;

    /**
     * @param Node $node
     */
    public function setNode(&$node)
    {
        $this->m_node = &$node;
    }

    /**
     * Sets the action session status for actions in the recordlist.
     * (Defaults to SessionManager::SESSION_NESTED).
     *
     * @param int $sessionStatus The session status (one of the SessionManager::SESSION_* constants)
     */
    public function setActionSessionStatus($sessionStatus)
    {
        $this->m_actionSessionStatus = $sessionStatus;
    }

    /**
     * Make the recordlist use a different masternode than the node than it is rendering.
     *
     * @param Node $masternode
     */
    public function setMasterNode(&$masternode)
    {
        $this->m_masternode = &$masternode;
    }

    /**
     * Converts the given node flags to recordlist flags where possible.
     *
     * @param int $flags
     * @static
     *
     * @return int
     */
    public function convertFlags($flags)
    {
        $result = Tools::hasFlag($flags, Node::NF_MRA) ? self::RL_MRA : 0;
        $result |= Tools::hasFlag($flags, Node::NF_MRPA) ? self::RL_MRPA : 0;
        $result |= Tools::hasFlag($flags, Node::NF_NO_SEARCH) ? self::RL_NO_SEARCH : 0;
        $result |= Tools::hasFlag($flags, Node::NF_NO_EXTENDED_SEARCH) ? self::RL_NO_EXTENDED_SEARCH : 0;
        $result |= Tools::hasFlag($flags, Node::NF_EXT_SORT) ? self::RL_EXT_SORT : 0;

        return $result;
    }

    /**
     * Get the masternode.
     *
     * @return Node The master node
     */
    public function getMasterNode()
    {
        if (is_object($this->m_masternode)) {
            return $this->m_masternode;
        }

        return $this->m_node; // treat rendered node as master
    }

    /**
     * Get the nodetype of the master node.
     *
     * @return string Modulename.nodename of the master node
     */
    public function getMasterNodeType()
    {
        $node = $this->getMasterNode();

        return $node->atkNodeUri();
    }
}
