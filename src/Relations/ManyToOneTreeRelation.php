<?php

namespace Sintattica\Atk\Relations;

use Exception;
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
    public $m_current = '';
    public $m_level = '';

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
     * @throws Exception
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
        $recordset = $this->m_destInstance->select($this->m_destInstance->getTable() . '.' . $this->m_destInstance->m_primaryKey[0])->includes($tmp2)->getAllRows();
        $this->m_current = $this->m_ownerInstance->primaryKey($record[$this->fieldName()]);
        $result = '<select class="form-control" name="' . $this->getHtmlName($fieldprefix) . '">';

        if (!$this->hasFlag(self::AF_OBLIGATORY)) {
            // Relation may be empty, so we must provide an empty selectable
            $result .= '<option value="0">' . Tools::atktext('select_none');
        }
        $result .= $this->createdd($recordset);
        $result .= '</select>';

        return $result;
    }

    public function search($record, $extended = false, $fieldprefix = '', DataGrid $grid = null): string
    {
        $this->createDestination();
        if ($this->m_destinationFilter != '') {
            $sp = new StringParser($this->m_destinationFilter);
            $this->m_destInstance->addFilter($sp->parse($record));
        }
        $recordset = $this->m_destInstance->select()->includes(Tools::atk_array_merge($this->m_destInstance->descriptorFields(),
            $this->m_destInstance->m_primaryKey))->getAllRows();

        $result = '<select class="form-control form-control-sm" name="atksearch[' . $this->fieldName() . ']">';
        $result .= '<option value="">' . Tools::atktext('search_all', 'atk');
        $result .= $this->createdd($recordset);
        $result .= '</select>';

        return $result;
    }

    /**
     * Create all the options.
     *
     * @param array $recordset
     *
     * @return string The HTML code for the options
     */
    public function createdd($recordset)
    {
        $t = new TreeToolsTree();
        for ($i = 0; $i < Tools::count($recordset); ++$i) {
            $group = $recordset[$i];
            $t->addNode($recordset[$i][$this->m_destInstance->m_primaryKey[0]], $this->m_destInstance->descriptor($group),
                $recordset[$i][$this->m_destInstance->m_parent][$this->m_destInstance->m_primaryKey[0]]);
        }
        return $this->render($t->m_tree);
    }

    /**
     * Render the tree.
     *
     * @param array $tree Array of tree nodes
     * @param int $level
     *
     * @return string The rendered tree
     */
    public function render($tree = [], $level = 0)
    {
        $res = '';
        $i = 0;
        foreach ($tree as $objarr) {
            ++$i;
            if ($this->m_current != $this->m_destInstance->getTable() . '.' . $this->m_destInstance->m_primaryKey[0] . "='" . $objarr->m_id . "'") {
                $this->m_level = $level;
                $sel = '';
            } else {
                // if equal, select the option it and do not render childs (parent cannot be moved to a childnode of its own)
                $sel = 'SELECTED';
            }

            $res .= '<option value="' . $this->m_destInstance->getTable() . '.' . $this->m_destInstance->m_primaryKey[0] . "='" . $objarr->m_id . "'" . '" ' . $sel . '>' . str_repeat('-',
                    (2 * $level)) . ' ' . $objarr->m_label;

            if (Tools::count($objarr->m_sub) > 0 && $sel == '') {
                $res .= $this->render($objarr->m_sub, $level + 1);
            }
        }
        $this->m_level = 0;

        return $res;
    }
}
