<?php

namespace Sintattica\Atk\Core;

/**
 * Node class, represents a node in a tree.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class TreeToolsNode
{
    public $m_id;
    public $m_label; // DEPRECATED, use $m_object instead.
    public $m_object;
    public $m_img;
    public $m_sub = [];

    public function __construct($id, $object, $img = '')
    {
        $this->m_id = $id;
        $this->m_object = $object;
        $this->m_label = $this->m_object; // DEPRECATED, but available for backwardcompatibility.
        $this->m_img = $img;
    }
}
