<?php

namespace Sintattica\Atk\Core;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Handlers\ActionHandler;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Utils\StringParser;

/**
 * Extension on the Node class. Here you will find all
 * functions for the tree view. If you want to use the treeview, you must define the TreeNode
 * instead of Node.
 * <b>Example:</b>
 * <code>
 * class classname extends TreeNode
 * {
 *      $this->atkTreeNode("nodeclass");.
 *
 * }
 * </code>
 *
 * @todo Documentation is outdated, and this class has not been ported yet
 *       to ATK5's new action handler mechanism, so it does not work.
 *
 * @author Martin Roest <martin@ibuildings.nl>
 * @author Sandy Pleyte <sandy@achievo.org>
 */
class TreeNode extends Node
{
    const NF_TREE_NO_ROOT_DELETE = self::NF_SPECIFIC_1; // No root elements can be deleted
    const NF_TREE_NO_ROOT_COPY = self::NF_SPECIFIC_2; // No root elements can be copied
    const NF_TREE_NO_ROOT_ADD = self::NF_SPECIFIC_3; // No root elements can be added
    const NF_TREE_AUTO_EXPAND = self::NF_SPECIFIC_4; // The tree is initially fully expanded

    public $m_tree = [];

    /*
     * var for giving the link for expanding/collapsing the tree extra params
     */
    public $xtraparams = '';

    /**
     * Constructor.
     *
     * @param string $name Node name
     * @param int $flags Node flags
     */
    public function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags);
    }

    /**
     * Action "admin" handler method, we override this method because we don't want
     * an add form when the flag self::NF_TREE_NO_ROOT_ADD. Because the add form is only
     * used to add root elements.
     *
     * @param ActionHandler $handler
     * @param array $record
     */
    public function action_admin($handler, $record = '')
    {
        if ($this->hasFlag(self::NF_TREE_NO_ROOT_ADD)) {
            $this->m_flags |= self::NF_NO_ADD;
        }

        return $handler->action_admin($record);
    }

    /**
     * Build the tree.
     *
     * @return TreeToolsTree Tree object
     */
    public function buildTree()
    {
        Tools::atkdebug('treenode::buildtree() '.$this->m_parent);
        $recordset = $this->select(Tools::atkArrayNvl($this->m_postvars, 'atkfilter', ''))->excludes($this->m_listExcludes)->mode('admin')->fetchAll();

        $treeobject = new TreeToolsTree();
        for ($i = 0; $i < Tools::count($recordset); ++$i) {
            $treeobject->addNode($recordset[$i][$this->m_primaryKey[0]], $recordset[$i], $recordset[$i][$this->m_parent][$this->m_primaryKey[0]]);
        }

        return $treeobject;
    }

    /**
     * Admin page displays records and the actions that can be performed on
     * them (edit, delete) in a Treeview.
     *
     * @param ActionHandler $handler The action handler object
     */
    public function adminPage($handler)
    {
        global $g_maxlevel;

        $ui = $this->getUi();

        $content = '';

        $adminHeader = $handler->invoke('adminHeader');
        if ($adminHeader != '') {
            $content .= $adminHeader.'<br><br>';
        }

        Tools::atkdebug('Entering treeview page.');

        $t = $this->buildTree();

        $this->m_tree[0]['level'] = 0;
        $this->m_tree[0]['id'] = '';
        $this->m_tree[0]['expand'] = $this->hasFlag(self::NF_TREE_AUTO_EXPAND) ? 1 : 0;
        $this->m_tree[0]['colapse'] = 0;
        $this->m_tree[0]['isleaf'] = 1;
        $this->m_tree[0]['label'] = '';

        $this->treeToArray($t->m_tree);

        $g_maxlevel = $g_maxlevel + 2;

        $width = ($g_maxlevel * 16) + 600;
        $content .= '<table border="0" cellspacing=0 cellpadding=0 cols='.($g_maxlevel + 2).' width='.$width.">\n";

        if (!$this->hasFlag(self::NF_NO_ADD) && $this->hasFlag(self::NF_ADD_LINK) && $this->allowed('add')) {
            $addurl = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction=add&atkfilter='.rawurlencode($this->m_parent.'.'.$this->m_primaryKey[0]."='0'");
            if (Tools::atktext('txt_link_'.Tools::getNodeType($this->m_type).'_add', $this->m_module, '', '', '', true) != '') {
                // specific text
                $label = Tools::atktext('txt_link_'.Tools::getNodeType($this->m_type).'_add', $this->m_module);
            } else {
                // generic text
                $label = Tools::atktext('add', 'atk');
            }
            $cssClass = 'class="btn btn-default admin_link admin_link_add"';
            $content .= Tools::href($addurl, $label, SessionManager::SESSION_NESTED, false, $cssClass).'<br><br>';
        }

        $content .= $this->GraphTreeRender();
        $content .= '</table><br>';

        $adminFooter = $handler->invoke('adminFooter');
        if ($adminFooter != '') {
            $content .= '<br>'.$adminFooter;
        }

        Tools::atkdebug('Generated treeview');

        return $ui->renderBox(array(
            'title' => Tools::atktext('title_'.$this->m_type.'_tree', $this->m_module),
            'content' => $content,
        ));
    }

    /**
     * Recursive funtion whitch fills an array with all the items of the tree.
     * DEPRECATED, use treeToArray instead.
     *
     * @param mixed $tree Tree
     * @param int $level Level
     */
    public function Fill_tree($tree = '', $level = 0)
    {
        Tools::atkdebug('WARNING: use of deprecated function Fill_tree, use treeToArray instead');

        return $this->treeToArray($tree, $level);
    }

    /**
     * Recursive funtion whitch fills an array with all the items of the tree.
     *
     * @param mixed $tree Tree
     * @param int $level Level
     */
    public function treeToArray($tree = '', $level = 0)
    {
        static $s_count = 1;
        global $g_maxlevel, $exp_index;
        while (list($id, $objarr) = each($tree)) {
            $this->m_tree[$s_count]['level'] = $level + 1;
            // Store extra info in the record, so the recordActions override can make
            // use of some extra info to determine whether or not to show certain actions.
            if (is_array($objarr->m_label)) {
                $objarr->m_label['subcount'] = Tools::count($objarr->m_sub);
            }
            $this->m_tree[$s_count]['label'] = $objarr->m_label;
            $this->m_tree[$s_count]['img'] = $objarr->m_img;
            $this->m_tree[$s_count]['id'] = $objarr->m_id;
            $exp_index[$objarr->m_id] = $s_count;
            $this->m_tree[$s_count]['isleaf'] = 0;
            if ($this->m_tree[$s_count]['level'] > $g_maxlevel) {
                $g_maxlevel = $this->m_tree[$s_count]['level'];
            }

            ++$s_count;
            if (Tools::count($objarr->m_sub) > 0) {
                $this->treeToArray($objarr->m_sub, $level + 1);
            }
        }

        return '';
    }

    /**
     * Returns the full path to a tree icon from the current theme.
     *
     * @param string $name Name of the icon (for example "expand" or "leaf")
     *
     * @return string Path to the icon file
     */
    public function getIcon($name)
    {
        return $name;
    }

    /**
     * Recursive funtion which fills an array with all the items of the tree.
     *
     * @param bool $showactions Show actions?
     * @param bool $expandAll Expand all leafs?
     * @param bool $foldable Is this tree foldable?
     *
     * @return string
     */
    public function GraphTreeRender($showactions = true, $expandAll = false, $foldable = true)
    {
        global $g_maxlevel, $exp_index;

        // Return
        if (Tools::count($this->m_tree) == 1) {
            return '';
        }

        $img_expand = Config::getGlobal('icon_expand');
        $img_collapse = Config::getGlobal('icon_collapse');
        $img_leaf = Config::getGlobal('icon_leaf');

        $res = '';
        $lastlevel = 0;
        $explevels = [];
        if ($this->m_tree[0]['expand'] != 1 && $this->m_tree[0]['colapse'] != 1) { // normal operation
            for ($i = 0; $i < Tools::count($this->m_tree); ++$i) {
                if ($this->m_tree[$i]['level'] < 2) {
                    if ($this->m_tree[$i]['isleaf'] == 1 && $this->m_tree[$i]['level'] < 1) {
                        $expand[$i] = 1;
                        $visible[$i] = 1;
                    } else {
                        $expand[$i] = 0;
                        $visible[$i] = 1;
                    }
                } else {
                    $expand[$i] = 0;
                    $visible[$i] = 0;
                }
            }
            if ($this->m_postvars['atktree'] != '') {
                $explevels = explode('|', $this->m_postvars['atktree']);
            }
        } elseif ($this->m_tree[0]['expand'] == 1) { // expand all mode!
            for ($i = 0; $i < Tools::count($this->m_tree); ++$i) {
                $expand[$i] = 1;
                $visible[$i] = 1;
            }
            $this->m_tree[0]['expand'] = 0; // next time we are back in normal view mode!
        } elseif ($this->m_tree[0]['colapse'] == 1) { //  colapse all mode!
            for ($i = 0; $i < Tools::count($this->m_tree); ++$i) {
                if ($this->m_tree[$i]['level'] < 2) {
                    if ($this->m_tree[$i]['isleaf'] == 1 && $this->m_tree[$i]['level'] < 1) {
                        $expand[$i] = 1;
                        $visible[$i] = 1;
                    } else {
                        $expand[$i] = 0;
                        $visible[$i] = 1;
                    }
                }
            }
            $this->m_tree[0]['colapse'] = 0; // next time we are back in normal view mode!
        }
        /*         * ****************************************** */
        /*  Get Node numbers to expand               */
        /*         * ****************************************** */
        foreach ($explevels as $explevel)
        {
            $expand[$exp_index[$explevel]] = 1;
        }

        /*         * ****************************************** */
        /*  Determine visible nodes                  */
        /*         * ****************************************** */

        $visible[0] = 1;   // root is always visible
        foreach ($explevels as $explevel) {
            $n = $exp_index[$explevel];
            if (($visible[$n] == 1) && ($expand[$n] == 1)) {
                $j = $n + 1;
                while ($this->m_tree[$j]['level'] > $this->m_tree[$n]['level']) {
                    if ($this->m_tree[$j]['level'] == $this->m_tree[$n]['level'] + 1) {
                        $visible[$j] = 1;
                    }
                    ++$j;
                }
            }
        }

        $res .= '<tr>';
        // Make cols for max level
        $res .= str_repeat("<td width=23>&nbsp;</td>\n", $g_maxlevel);
        // Make the last text column
        $res .= '<td width=300>&nbsp;</td>';
        // Column for the functions
        if ($showactions) {
            $res .= '<td width=300>&nbsp;</td>';
        }
        $res .= "</tr>\n";
        $cnt = 0;
        while ($cnt < Tools::count($this->m_tree)) {
            if ($visible[$cnt]) {
                $currentlevel = (isset($this->m_tree[$cnt]['level']) ? $this->m_tree[$cnt]['level'] : 0);
                $nextlevel = (isset($this->m_tree[$cnt + 1]['level']) ? $this->m_tree[$cnt + 1]['level'] : 0);

                /****************************************/
                /* start new row                        */
                /****************************************/
                $res .= '<tr>';

                /****************************************/
                /* vertical lines from higher levels    */
                /****************************************/
                if ($cnt != 0) {
                   $res .= "<td style='text-align:center'> │ </td>\n";
                }

                if ($this->m_tree[$cnt]['level'] - 1 >= 1) {
                    $res .= str_repeat("<td style='text-align:center'> │ </td>\n", $this->m_tree[$cnt]['level'] - 1);
                }

                /*                 * ***************************************** */
                /* Node (with subtree) or Leaf (no subtree) */
                /*                 * ***************************************** */
                if ($nextlevel > $currentlevel) {
                    /*                     * ************************************* */
                    /* Create expand/collapse parameters    */
                    /*                     * ************************************* */
                    if ($foldable) {
                        $i = 0;
                        $params = 'atktree=';
                        while ($i < Tools::count($expand)) {
                            if (($expand[$i] == 1) && ($cnt != $i) || ($expand[$i] == 0 && $cnt == $i)) {
                                $params = $params.$this->m_tree[$i]['id'];
                                $params = $params.'|';
                            }
                            ++$i;
                        }
                        if (isset($this->extraparams)) {
                            $params = $params.$this->extraparams;
                        }
                        if ($expand[$cnt] == 0) {
                            $res .= '<td>'.Tools::href(Config::getGlobal('dispatcher').'?'.$params, '<i class="fa fa-'.$img_expand.'"></i>', SessionManager::SESSION_DEFAULT, false, 'class="btn btn-default"')."</td>\n";
                        } else {
                            $res .= '<td>'.Tools::href(Config::getGlobal('dispatcher').'?'.$params, '<i class="fa fa-'.$img_collapse.'"></i>', SessionManager::SESSION_DEFAULT, false, 'class="btn btn-default"')."</td>\n";
                        }
                    } else {
                        $res .= '<td><i class="fa fa-'.$img_collapse."\"></i></td>\n";
                    }
                } else {
                    /*                     * ********************** */
                    /* Tree Leaf             */
                    /*                     * ********************** */
                    $img = $img_leaf; // the image is a leaf image by default, but it can be overridden
                    // by putting img to something else
                    if ($this->m_tree[$cnt]['img'] != '') {
                        $imgname = $this->m_tree[$cnt]['img'];
                        $img = $$imgname;
                    }
                    $res .= '<td style="text-align:center"><i class="fa fa-'.$img."\"></i></td>\n";
                }

                /*                 * ************************************* */
                /* output item text                     */
                /*                 * ************************************* */
                // If there's an array inside the 'label' thingee, we have an entire record.
                // Else, it's probably just a textual label.
                if (is_array($this->m_tree[$cnt]['label'])) {
                    $label = $this->descriptor($this->m_tree[$cnt]['label']);
                } else {
                    $label = $this->m_tree[$cnt]['label'];
                }
                $res .= '<td colspan='.($g_maxlevel - $this->m_tree[$cnt]['level']).' nowrap><font size=2>'.$label."</font></td>\n";

                /*                 * ************************************* */
                /* end row   with the functions                      */
                /*                 * ************************************* */
                if ($showactions) {
                    $res .= '<td nowrap> ';
                    $actions = [];

                    if (!$this->hasFlag(self::NF_NO_ADD) && !($this->hasFlag(self::NF_TREE_NO_ROOT_ADD) && $this->m_tree[$cnt]['level'] == 0)) {
			$presetForm = '{'.json_encode($this->m_parent).':'.$this->primaryKeyString($this->m_tree[$cnt]).'}';
                        $actions['add'] = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction=add&atkforce='.urlencode($presetForm);
                    }
                    if ($cnt > 0) {
                        if (!$this->hasFlag(self::NF_NO_EDIT)) {
                            $actions['edit'] = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction=edit&atkselector='.urlencode($this->primaryKeyString($this->m_tree[$cnt]));
                        }
                        if (($this->hasFlag(self::NF_COPY) && $this->allowed('add') && !$this->hasFlag(self::NF_TREE_NO_ROOT_COPY)) || ($this->m_tree[$cnt]['level'] != 1 && $this->hasFlag(self::NF_COPY) && $this->allowed('add'))) {
                            $actions['copy'] = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction=copy&atkselector='.urlencode($this->primaryKeyString($this->m_tree[$cnt]));
                        }
                        if ($this->hasFlag(self::NF_NO_DELETE) || ($this->hasFlag(self::NF_TREE_NO_ROOT_DELETE) && $this->m_tree[$cnt]['level'] == 1)) {
                            // Do nothing
                        } else {
                            $actions['delete'] = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction=delete&atkselector='.urlencode($this->primaryKeyString($this->m_tree[$cnt]));
                        }
                    }

                    $res .= '</td>';
                }
                $res .= "</tr>\n";
            }
            ++$cnt;
        }

        return $res;
    }

    /**
     * Copies a record and the Childs if there are any.
     *
     * @param array $record The record to copy
     * @param string $mode The mode we're in (usually "copy")
     */
    public function copyDb(&$record, $mode = 'copy')
    {
        $oldparent = $record[$this->m_primaryKey[0]];

        parent::copyDb($record, $mode);

        if (!empty($this->m_parent)) {
            Tools::atkdebug('copyDb - Main Record added');
            $newparent = $record[$this->m_primaryKey[0]];
            Tools::atkdebug('CopyDbCopychildren('.$this->m_parent.'='.$oldparent.','.$newparent.')');
            $this->copyChildren($this->m_table.'.'.$this->m_parent.'='.$oldparent, $newparent, $mode);
        }

        return true;
    }

    /**
     * This is a recursive function to copy the children from a parent.
     *
     * @todo shouldn't we recursively call copyDb here? instead of ourselves
     *
     * @param string $selector Selector
     * @param int $parent Parent ID
     * @param string $mode The mode we're in
     */
    public function copyChildren($selector, $parent = '', $mode = 'copy')
    {
        $recordset = $this->select($selector)->mode($mode)->fetchAll();

        if (Tools::count($recordset) > 0) {
            for ($i = 0; $i < Tools::count($recordset); ++$i) {
                $recordset[$i][$this->m_parent] = array('' => '', $this->m_primaryKey[0] => $parent);
                $oldrec = $recordset[$i];
                parent::copyDb($recordset[$i], $mode);

                Tools::atkdebug('Child Record added');
                $newparent = $recordset[$i][$this->m_primaryKey[0]];
                Tools::atkdebug('CopyChildren('.$this->m_parent.'='.$oldrec[$this->m_primaryKey[0]].','.$newparent.')');
                $this->copyChildren($this->m_table.'.'.$this->m_parent.'='.$oldrec[$this->m_primaryKey[0]], $newparent);
            }
        } else {
            Tools::atkdebug("No records found with Selector: $selector - $parent");
        }

        return '';
    }

    /**
     * Delete record(s) and childrecord from the database.
     *
     * After deletion, the postDel() trigger in the node method is called, and
     * on any attribute that has the Attribute::AF_CASCADE_DELETE flag set, the delete()
     * method is invoked.
     *
     * NOTE: Does not commit your transaction! If you are using a database that uses
     * transactions you will need to call 'Db::getInstance()->commit()' manually.
     *
     * @todo There's a discrepancy between updateDb, addDb and deleteDb:
     *       There should be a deleteDb which accepts a record, instead
     *       of a selector.
     *
     * @param QueryPart $selector SQL expression used as where-clause that
     *                              indicates which records to delete.
     * @param bool $exectrigger wether to execute the pre/post triggers. Note that the preDelete
     *        trigger is execute after the deletion of all children nodes and before the deletion
     *        of selected nodes.
     * @param bool $failwhenempty determine whether to throw an error if there is nothing to delete
     * @returns boolean True if successful, false if not.
      */
    public function deleteDb($selector, $exectrigger = true, $failwhenempty = false)
    {
        Tools::atkdebug('Retrieve record');
        $recordset = $this->select($selector)->mode('delete')->getAllRows();
        // First, recursively deleting children (and stopping if it fails) :
        if ($this->m_parent != '') {
            foreach ($recordset as $record) {
                $condition = new QueryPart(Db::quoteIdentifier($this->m_table, $this->m_parent).'=:parent',
                    [':parent' => $record[$this->m_primaryKey[0]]]);
                if (!$this->deleteDb($condition, $exectrigger, false)) {
                    return false;
                }
            }
        }
        // Then deleting records :
        return parent::deleteDb($selector, $exectrigger, $failwhenempty);
    }
}
