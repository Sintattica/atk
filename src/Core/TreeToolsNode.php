<?php namespace Sintattica\Atk\Core;

/**
 * Node class, represents a node in a tree.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 */
class TreeToolsNode
{
    var $m_id;
    var $m_label; // DEPRECATED, use $m_object instead.
    var $m_object;
    var $m_img;
    var $m_sub = array();

    function __construct($id, $object, $img = "")
    {
        $this->m_id = $id;
        $this->m_object = $object;
        $this->m_label = $this->m_object; // DEPRECATED, but available for backwardcompatibility.
        $this->m_img = $img;
    }

}
