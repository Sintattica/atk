<?php namespace Sintattica\Atk\Relations;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\TreeToolsTree;
use Sintattica\Atk\Utils\StringParser;

/**
 * Extension of the ManyToOneRelation, that is aware of the treestructure
 * (parent/child relation) in the destination node, and renders items in the
 * dropdown accordingly. You need to set the self::AF_PARENT flag to the parent
 * column in the destination node in order to make the tree rendering work.
 *
 * @author Sandy Pleyte <sandy@ibuildings.nl>
 * @package atk
 * @subpackage relations
 *
 */
class ManyToOneTreeRelation extends ManyToOneRelation
{
    var $m_current = "";
    var $m_level = "";

    /**
     * Constructor
     * @param string $name Name of the attribute
     * @param string $destination Destination node for this relation
     * @param int $flags Flags for the relation
     */
    function __construct($name, $destination, $flags = 0)
    {
        parent::__construct($name, $destination, $flags);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return Piece of html code that can  be used in a form to edit this
     */
    function edit($record = "", $fieldprefix = "", $mode = "")
    {
        $this->createDestination();
        $tmp1 = Tools::atk_array_merge($this->m_destInstance->descriptorFields(),
            $this->m_destInstance->m_primaryKey);
        $tmp2 = Tools::atk_array_merge($tmp1, array($this->m_destInstance->m_parent));
        if ($this->m_destinationFilter != "") {
            $sp = new StringParser($this->m_destinationFilter);
            $this->m_destInstance->addFilter($sp->parse($record));
        }
        $recordset = $this->m_destInstance->selectDb("", $this->m_destInstance->m_primaryKey[0], "", "", $tmp2);
        $this->m_current = $this->m_ownerInstance->primaryKey($record);
        $result = '<select class="form-control" name="' . $fieldprefix . $this->fieldName() . '">';

        if ($this->hasFlag(self::AF_OBLIGATORY) == false) {
            // Relation may be empty, so we must provide an empty selectable..
            $result .= '<option value="0">' . Tools::atktext('select_none');
        }
        $result .= $this->createdd($recordset);
        $result .= '</select>';
        return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to search
     * @param array $record Record
     * @return string Piece of html code that can  be used in a form to edit this
     */
    function search($record = "")
    {
        $this->createDestination();
        if ($this->m_destinationFilter != "") {
            $sp = new StringParser($this->m_destinationFilter);
            $this->m_destInstance->addFilter($sp->parse($record));
        }
        $recordset = $this->m_destInstance->selectDb("", "", "", "",
            Tools::atk_array_merge($this->m_destInstance->descriptorFields(), $this->m_destInstance->m_primaryKey));

        $result = '<select class="form-control" name="atksearch[' . $this->fieldName() . ']">';

        $pkfield = $this->m_destInstance->primaryKeyField();

        $result .= '<option value="">' . Tools::atktext("search_all", "atk");
        $result .= $this->createdd($recordset);
        $result .= '</select>';
        return $result;
    }

    /**
     * Create all the options
     *
     * @param array $recordset
     * @return string The HTML code for the options
     */
    function createdd($recordset)
    {
        $t = new TreeToolsTree();
        for ($i = 0; $i < count($recordset); $i++) {
            $group = $recordset[$i];
            $t->addNode($recordset[$i][$this->m_destInstance->m_primaryKey[0]],
                $this->m_destInstance->descriptor($group),
                $recordset[$i][$this->m_destInstance->m_parent][$this->m_destInstance->m_primaryKey[0]]);
        }
        $tmp = $this->render($t->m_tree);
        return $tmp;
    }

    /**
     * Render the tree
     *
     * @param array $tree Array of tree nodes
     * @param int $level
     * @return string The rendered tree
     */
    function render($tree = "", $level = 0)
    {
        $res = "";
        $i = 0;
        while (list($id, $objarr) = each($tree)) {
            $i++;
            if ($this->m_current != $this->m_destInstance->m_table . "." . $this->m_destInstance->m_primaryKey[0] . "='" . $objarr->m_id . "'") {
                $this->m_level = $level;
                $sel = "";
            } else {
                // if equal, select the option it and do not render childs (parent cannot be moved to a childnode of its own)
                $sel = "SELECTED";
            }

            $res .= '<option value="' . $this->m_destInstance->m_table . "." . $this->m_destInstance->m_primaryKey[0] . "='" . $objarr->m_id . "'" . '" ' . $sel . '>' . str_repeat("-",
                    (2 * $level)) . " " . $objarr->m_label;

            if (count($objarr->m_sub) > 0 && $sel == "") {
                $res .= $this->render($objarr->m_sub, $level + 1);
            }
        }
        $this->m_level = 0;
        return $res;
    }

}


