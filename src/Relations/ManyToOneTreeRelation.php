<?php

namespace Sintattica\Atk\Relations;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\TreeToolsTree;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Utils\StringParser;

/**
 * Extension of the ManyToOneRelation, that is aware of the treestructure
 * (parent/child relation) in the destination node, and renders items in the
 * dropdown accordingly. You need to set the self::AF_PARENT flag to the parent
 * column in the destination node in order to make the tree rendering work.
 *
 * @author Sandy Pleyte <sandy@ibuildings.nl>
 */
class ManyToOneTreeRelation extends ManyToOneRelation
{
    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for the relation
     * @param string $destination Destination node for this relation
     *
     */
    public function __construct($name, $flags = 0, $destination)
    {
        parent::__construct($name, $flags, $destination);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string Piece of html code that can  be used in a form to edit this
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $this->createDestination();
        $tmp1 = Tools::atk_array_merge($this->m_destInstance->descriptorFields(), $this->m_destInstance->m_primaryKey);
        $tmp2 = Tools::atk_array_merge($tmp1, [$this->m_destInstance->m_parent]);
        if ($this->m_destinationFilter != '') {
            $sp = new StringParser($this->m_destinationFilter);
            $this->m_destInstance->addFilter($sp->parse($record));
        }
        $recordset = $this->m_destInstance->select()->includes($tmp2)->getAllRows();
        $result = '<select class="form-control" name="'.$this->getHtmlName($fieldprefix).'">';

        if ($this->hasFlag(self::AF_OBLIGATORY) == false) {
            // Relation may be empty, so we must provide an empty selectable..
            $result .= '<option value="0">'.Tools::atktext('select_none');
        }
        $parent = $record[$this->fieldName()][$this->m_destInstance->m_primaryKey[0]] ?? '';
        $currentid = $record[$this->m_ownerInstance->m_primaryKey[0]];
        $result .= $this->createdd($recordset, $parent, $currentid);
        $result .= '</select>';

        return $result;
    }

    public function search($record, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        $this->createDestination();
        if ($this->m_destinationFilter != '') {
            $sp = new StringParser($this->m_destinationFilter);
            $this->m_destInstance->addFilter($sp->parse($record));
        }
        $recordset = $this->m_destInstance->select()->includes(Tools::atk_array_merge($this->m_destInstance->descriptorFields(),
                $this->m_destInstance->m_primaryKey))->getAllRows();

        $result = '<select class="form-control" name="atksearch['.$this->fieldName().']">';
        $result .= '<option value="">'.Tools::atktext('search_all', 'atk');
        $result .= $this->createdd($recordset);
        $result .= '</select>';

        return $result;
    }

    /**
     * Create all the options.
     *
     * @param array $recordset
     * @param int $parent of owner's node
     * @param int $currentid of this node
     *
     * @return string The HTML code for the options
     */
    public function createdd($recordset, $parent = '', $currentid = '')
    {
        $t = new TreeToolsTree();
        foreach ($recordset as $record) {
            $t->addNode($record[$this->m_destInstance->m_primaryKey[0]], $this->m_destInstance->descriptor($record),
                $record[$this->m_destInstance->m_parent][$this->m_destInstance->m_primaryKey[0]]);
        }
        return $this->render($t->m_tree, $parent, $currentid);
    }

    /**
     * Render the tree.
     *
     * @param array $tree Array of tree nodes
     * @param int $currentid : owner node's id
     * @param int $level
     *
     * @return string The rendered tree
     */
    public function render($tree = [], $parent, $currentid, $level = 0)
    {
        $res = '';
        while (list(, $objarr) = each($tree)) {
            switch($objarr->m_id) {
            case $parent:
                $opt = ' SELECTED';
                break;
            case $currentid:
                // Disable the option it and do not render childs (parent cannot be moved to a childnode of its own)
                $opt = ' DISABLED';
                break;
            default:
                $opt = '';
                break;
             }

            $res .= '<option value="'.$objarr->m_id.'" '.$opt.'>'.str_repeat('-',
                    (2 * $level)).' '.$objarr->m_label;

            if (count($objarr->m_sub) > 0 && $opt != ' DISABLED') {
                $res .= $this->render($objarr->m_sub, $parent, $currentid, $level + 1);
            }
        }
        return $res;
    }
}
