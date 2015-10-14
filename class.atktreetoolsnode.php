<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be 
 * included in the distribution.
 *
 * @package atk   
 * @todo The atktreetools should be moved to the utils subpackage.
 *
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Node class, represents a node in a tree.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk   
 */
class Atk_TreeToolsNode
{
    var $m_id;
    var $m_label; // DEPRECATED, use $m_object instead.
    var $m_object;
    var $m_img;
    var $m_sub = array();

    function atkTreeToolsNode($id, $object, $img = "")
    {
        $this->m_id = $id;
        $this->m_object = $object;
        $this->m_label = &$this->m_object; // DEPRECATED, but available for backwardcompatibility.
        $this->m_img = $img;
    }

}
