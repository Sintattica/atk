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
 *       to ATK5's new action handler mechanism, so it may not work.
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
                $label = Tools::atktext('link_'.Tools::getNodeType($this->m_type).'_add', $this->m_module);
            } else {
                // generic text
                $label = Tools::atktext(Tools::getNodeType($this->m_type), $this->m_module).' '.Tools::atktext('add', 'atk');
            }
            $content .= Tools::href($addurl, $label, SessionManager::SESSION_NESTED).'<br><br>';
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

        $img_expand = $this->getIcon('expand');
        $img_collapse = $this->getIcon('collapse');
        $img_line = $this->getIcon('vertline');
        $img_split = $this->getIcon('split');
        $img_plus = $this->getIcon('split_plus');
        $img_minus = $this->getIcon('split_minus');
        $img_end = $this->getIcon('end');
        $img_end_plus = $this->getIcon('end_plus');
        $img_end_minus = $this->getIcon('end_minus');
        $img_leaf = $this->getIcon('leaf');
        $img_leaflink = $this->getIcon('leaf_link');
        $img_spc = $this->getIcon('space');
        $img_extfile = $this->getIcon('extfile');

        $res = '';
        $lastlevel = 0;
        //echo $this->m_tree[0]["expand"]."--".$this->m_tree[0]["colapse"];
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
                $levels[$i] = 0;
            }
            if ($this->m_postvars['atktree'] != '') {
                $explevels = explode('|', $this->m_postvars['atktree']);
            }
        } elseif ($this->m_tree[0]['expand'] == 1) { // expand all mode!
            for ($i = 0; $i < Tools::count($this->m_tree); ++$i) {
                $expand[$i] = 1;
                $visible[$i] = 1;
                $levels[$i] = 0;
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
                $levels[$i] = 0;
            }
            $this->m_tree[0]['colapse'] = 0; // next time we are back in normal view mode!
        }
        /*         * ****************************************** */
        /*  Get Node numbers to expand               */
        /*         * ****************************************** */
        $i = 0;
        while ($i < Tools::count($explevels)) {
            //$expand[$explevels[$i]]=1;
            $expand[$exp_index[$explevels[$i]]] = 1;

            ++$i;
        }
        /*         * ****************************************** */
        /*  Find last nodes of subtrees              */
        /*         * ****************************************** */

        $lastlevel = $g_maxlevel;

        for ($i = Tools::count($this->m_tree) - 1; $i >= 0; --$i) {
            if ($this->m_tree[$i]['level'] < $lastlevel) {
                for ($j = $this->m_tree[$i]['level'] + 1; $j <= $g_maxlevel; ++$j) {
                    $levels[$j] = 0;
                }
            }
            if ($levels[$this->m_tree[$i]['level']] == 0) {
                $levels[$this->m_tree[$i]['level']] = 1;
                $this->m_tree[$i]['isleaf'] = 1;
            } else {
                $this->m_tree[$i]['isleaf'] = 0;
            }
            $lastlevel = $this->m_tree[$i]['level'];
        }
        /*         * ****************************************** */
        /*  Determine visible nodes                  */
        /*         * ****************************************** */

        $visible[0] = 1;   // root is always visible
        for ($i = 0; $i < Tools::count($explevels); ++$i) {
            $n = $exp_index[$explevels[$i]];
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

        for ($i = 0; $i < $g_maxlevel; ++$i) {
            $levels[$i] = 1;
        }

        $res .= '<tr>';
        // Make cols for max level
        for ($i = 0; $i < $g_maxlevel; ++$i) {
            $res .= "<td width=16>&nbsp;</td>\n";
        }
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
                $i = 0;
                while ($i < $this->m_tree[$cnt]['level'] - 1) {
                    if ($levels[$i] == 1) {
                        $res .= '<td><img src="'.$img_line."\" border=0></td>\n";
                    } else {
                        $res .= '<td><img src="'.$img_spc."\" border=0></td>\n";
                    }
                    ++$i;
                }

                /***************************************/
                /* corner at end of subtree or t-split */
                /***************************************/
                if ($this->m_tree[$cnt]['isleaf'] == 1 && $nextlevel < $currentlevel) {
                    if ($cnt != 0) {
                        $res .= '<td><img src="'.$img_end."\" border=0></td>\n";
                    }
                    $levels[$this->m_tree[$cnt]['level'] - 1] = 0;
                } else {
                    if ($expand[$cnt] == 0) {
                        if ($nextlevel > $currentlevel) {
                            /*                             * ************************************* */
                            /* Create expand/collapse parameters    */
                            /*                             * ************************************* */
                            $i = 0;
                            $params = 'atktree=';
                            while ($i < Tools::count($expand)) {
                                if (($expand[$i] == 1) && ($cnt != $i) || ($expand[$i] == 0 && $cnt == $i)) {
                                    $params = $params.$this->m_tree[$i]['id'];
                                    $params = $params.'|';
                                }
                                ++$i;
                            }
                            if ($this->extraparams) {
                                $params = $params.$this->extraparams;
                            }

                            if ($this->m_tree[$cnt]['isleaf'] == 1) {
                                if ($cnt != 0) {
                                    $res .= '<td>'.Tools::href(Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction='.$this->m_action.'&'.$params,
                                            '<img src="'.$img_end_plus.'" border=0>')."</td>\n";
                                }
                            } else {
                                if ($cnt != 0) {
                                    $res .= '<td>'.Tools::href(Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction='.$this->m_action.'&'.$params,
                                            '<img src="'.$img_plus.'" border=0>')."</td>\n";
                                }
                            }
                        } else {
                            $res .= '<td><img src="'.$img_split."\" border=0></td>\n";
                        }
                    } else {
                        if ($nextlevel > $currentlevel) {
                            /*                             * ************************************* */
                            /* Create expand/collapse parameters    */
                            /*                             * ************************************* */
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
                            if ($this->m_tree[$cnt]['isleaf'] == 1) {
                                if ($cnt != 0) {
                                    if ($foldable) {
                                        $res .= '<td>'.Tools::href(Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction='.$this->m_action.'&'.$params,
                                                '<img src="'.$img_end_minus.'" border=0>')."</td>\n";
                                    } else {
                                        $res .= '<td><img src="'.$img_end."\" border=0></td>\n";
                                    }
                                }
                            } else {
                                if ($cnt != 0) {
                                    if ($foldable) {
                                        $res .= '<td>'.Tools::href(Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction='.$this->m_action.'&'.$params,
                                                '<img src="'.$img_minus.'" border=0>')."</td>\n";
                                    } else {
                                        $res .= '<td><img src="'.$img_split."\" border=0></td>\n";
                                    }
                                }
                            }
                        } else {
                            $res .= '<td><img src="'.$img_split."\" border=0></td>\n";
                        }
                    }
                    if ($this->m_tree[$cnt]['isleaf'] == 1) {
                        $levels[$this->m_tree[$cnt]['level'] - 1] = 0;
                    } else {
                        $levels[$this->m_tree[$cnt]['level'] - 1] = 1;
                    }
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
                            $res .= '<td>'.Tools::href(Config::getGlobal('dispatcher').'?'.$params, '<img src="'.$img_expand.'" border=0>')."</td>\n";
                        } else {
                            $res .= '<td>'.Tools::href(Config::getGlobal('dispatcher').'?'.$params, '<img src="'.$img_collapse.'" border=0>')."</td>\n";
                        }
                    } else {
                        $res .= '<td><img src="'.$img_collapse."\" border=0></td>\n";
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
                    $res .= '<td><img src="'.$img."\"></td>\n";
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
                        $actions['add'] = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction=add&atkfilter='.$this->m_parent.'.'.$this->m_primaryKey[0].rawurlencode("='".$this->m_tree[$cnt]['id']."'");
                    }
                    if ($cnt > 0) {
                        if (!$this->hasFlag(self::NF_NO_EDIT)) {
                            $actions['edit'] = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction=edit&atkselector='.$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt]['id'];
                        }
                        if (($this->hasFlag(self::NF_COPY) && $this->allowed('add') && !$this->hasFlag(self::NF_TREE_NO_ROOT_COPY)) || ($this->m_tree[$cnt]['level'] != 1 && $this->hasFlag(self::NF_COPY) && $this->allowed('add'))) {
                            $actions['copy'] = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction=copy&atkselector='.$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt]['id'];
                        }
                        if ($this->hasFlag(self::NF_NO_DELETE) || ($this->hasFlag(self::NF_TREE_NO_ROOT_DELETE) && $this->m_tree[$cnt]['level'] == 1)) {
                            // Do nothing
                        } else {
                            $actions['delete'] = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkaction=delete&atkselector='.$this->m_table.'.'.$this->m_primaryKey[0].'='.$this->m_tree[$cnt]['id'];
                        }
                    }

                    // Look for custom record actions.
                    $recordactions = $actions;
                    $this->collectRecordActions($this->m_tree[$cnt]['label'], $recordactions, $dummy);

                    foreach ($recordactions as $name => $url) {
                        if (!empty($url)) {
                            /* dirty hack */
                            $atkencoded = strpos($url, '_1') > 0;

                            $url = str_replace('%5B', '[', $url);
                            $url = str_replace('%5D', ']', $url);
                            $url = str_replace('_1'.'5B', '[', $url);
                            $url = str_replace('_1'.'5D', ']', $url);

                            if ($atkencoded) {
                                $url = str_replace('[pk]', Tools::atkurlencode(rawurlencode($this->primaryKeyString($this->m_tree[$cnt]['label'])), false), $url);
                            } else {
                                $url = str_replace('[pk]', rawurlencode($this->primaryKeyString($this->m_tree[$cnt]['label'])), $url);
                            }

                            $stringparser = new StringParser($url);
                            $url = $stringparser->parse($this->m_tree[$cnt]['label'], true);

                            $res .= Tools::href($url, Tools::atktext($name), SessionManager::SESSION_NESTED).'&nbsp;';
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
    public function copyDb($record, $mode = 'copy')
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
     * delete record from the database also the childrecords.
     * todo: instead of delete, set the deleted flag.
     *
     * @param string $selector Selector
     */
    public function deleteDb($selector)
    {
        Tools::atkdebug('Retrieve record');
        $recordset = $this->select($selector)->mode('delete')->fetchAll();
        for ($i = 0; $i < Tools::count($recordset); ++$i) {
            foreach (array_keys($this->m_attribList) as $attribname) {
                $p_attrib = $this->m_attribList[$attribname];
                if ($p_attrib->hasFlag(Attribute::AF_CASCADE_DELETE)) {
                    $p_attrib->delete($recordset[$i]);
                }
            }
        }
        $parent = $recordset[0][$this->m_primaryKey[0]];
        if ($this->m_parent != '') {
            Tools::atkdebug('Check for child records');
            $children = $this->select($this->m_table.'.'.$this->m_parent.'='.$parent)->mode('delete')->fetchAll();

            if (Tools::count($children) > 0) {
                Tools::atkdebug('DeleteChildren('.$this->m_table.'.'.$this->m_parent.'='.$parent.','.$parent.')');
                $this->deleteChildren($this->m_table.'.'.$this->m_parent.'='.$parent, $parent);
            }
        }

        $db = $this->getDb();
        $query = 'DELETE FROM '.$this->m_table.' WHERE '.$selector;
        $db->query($query);

        for ($i = 0; $i < Tools::count($recordset); ++$i) {
            $this->postDel($recordset[$i]);
            $this->postDelete($recordset[$i]);
        }

        return $recordset;
        // todo: instead of delete, set the deleted flag.
    }

    /**
     * Recursive function whitch deletes all the child records of a parent.
     *
     * @param string $selector Selector
     * @param int $parent Id of the parent
     */
    public function deleteChildren($selector, $parent)
    {
        Tools::atkdebug('Check for child records of the Child');
        $recordset = $this->select($this->m_table.'.'.$this->m_parent.'='.$parent)->mode('delete')->fetchAll();
        for ($i = 0; $i < Tools::count($recordset); ++$i) {
            foreach (array_keys($this->m_attribList) as $attribname) {
                $p_attrib = $this->m_attribList[$attribname];
                if ($p_attrib->hasFlag(Attribute::AF_CASCADE_DELETE)) {
                    $p_attrib->delete($recordset[$i]);
                }
            }
        }

        if (Tools::count($recordset) > 0) {
            for ($i = 0; $i < Tools::count($recordset); ++$i) {
                $parent = $recordset[$i][$this->m_primaryKey[0]];
                Tools::atkdebug('DeleteChildren('.$this->m_table.'.'.$this->m_parent.'='.$recordset[$i][$this->m_primaryKey[0]].','.$parent.')');
                $this->deleteChildren($this->m_table.'.'.$this->m_parent.'='.$recordset[$i][$this->m_primaryKey[0]], $parent);
            }
        }

        $db = $this->getDb();
        $query = 'DELETE FROM '.$this->m_table.' WHERE '.$selector;
        $db->query($query);
    }
}
