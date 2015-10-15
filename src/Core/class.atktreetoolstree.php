<?php namespace Sintattica\Atk\Core;

include_once(Atk_Config::getGlobal("atkroot") . "atk/class.atktreetoolsnode.php");

/**
 * Tree class, used to build trees of nodes.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 */
class Atk_TreeToolsTree
{
    var $m_tree = array();
    var $m_allnodes = array();
    var $m_parentless = array(); // Array to keep stuff that can not yet be inserted into the array. 

    function addNode($id, $naam, $parent = 0, $img = "")
    {
        $n = new Atk_TreeToolsNode($id, $naam, $img);
        $this->m_allnodes[$id] = &$n;

        if (array_key_exists($id, $this->m_parentless) && is_array($this->m_parentless[$id])) {
            // In the parentless array, there are children that belong to this new record.
            $n->m_sub = &$this->m_parentless[$id];
            unset($this->m_parentless[$id]);
        }

        if (empty($parent)) {
            $this->m_tree[] = &$n;
        } else {
            $tmp = &$this->m_allnodes[$parent];
            if (is_object($tmp)) {
                $tmp->m_sub[] = &$n;
            } else {
                // Dangling thingee.
                $this->m_parentless[$parent][] = &$n;
            }
        }
    }

    /**
     * Example render function. Implement your own.
     */
    function render($tree = "", $level = 0)
    {
        // First time: root tree..
        if ($tree == "") {
            $tree = $this->m_tree;
        }
        $res = "";
        while (list($id, $objarr) = each($tree)) {
            $res .= '<tr><td>' . str_repeat("-", (2 * $level)) . " " . $objarr->m_label . '</td></tr>';
            if (count($objarr->m_sub) > 0) {
                $res .= $this->render($objarr->m_sub, $level + 1);
            }
        }
        return $res;
    }

    /**
     * Pops tree's on the session
     */
    function sessionTree()
    {
        global $ATK_VARS;
        $postTree = $ATK_VARS["atktree"];
        $sessionTree = Atk_SessionManager::sessionLoad("atktree");
        if ($postTree != "" && $sessionTree != $postTree) {
            Atk_SessionManager::sessionStore("atktree", $postTree); // new in the session
            $realTree = $postTree;
        } else {
            $realTree = $sessionTree; // use the last known tree
        }
        $ATK_VARS["atktree"] == $realTree; // postvars now should contain the last Knowtree
    }

}


