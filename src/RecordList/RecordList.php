<?php namespace Sintattica\Atk\RecordList;

use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Ui\Theme;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Keyboard\Keyboard;
use Sintattica\Atk\Utils\StringParser;

/**
 * The recordlist class is used to render tables containing records.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage recordlist
 *
 */
class RecordList
{
    /** recordlist flags */
    const RL_NO_SORT = 1; // recordlist is not sortable
    const RL_NO_SEARCH = 2; // recordlist is not searchable
    const RL_NO_EXTENDED_SEARCH = 4; // recordlist is not searchable
    const RL_EMBED = 8; // recordlist is embedded
    const RL_MRA = 16; // multi-record-actions enabled
    const RL_MRPA = 32; // multi-record-priority-actions enabled
    const RL_LOCK = 64; // records can be locked
    const RL_EXT_SORT = 128; // extended sort feature

    /** @var Node $m_node */
    var $m_node;

    var $m_flags = 0;
    var $m_actionloader;
    var $m_masternode = null;
    var $m_hasActionColumn = 0;
    var $m_actionSessionStatus = SessionManager::SESSION_NESTED;

    /**
     * @access private
     * @param Node $node
     */
    function setNode(&$node)
    {
        $this->m_node = &$node;
    }

    /**
     * Sets the action session status for actions in the recordlist.
     * (Defaults to SessionManager::SESSION_NESTED).
     *
     * @param int $sessionStatus The session status (one of the SessionManager::SESSION_* constants)
     */
    function setActionSessionStatus($sessionStatus)
    {
        $this->m_actionSessionStatus = $sessionStatus;
    }

    /**
     * Make the recordlist use a different masternode than the node than it is rendering.
     *
     * @param Node $masternode
     */
    function setMasterNode(&$masternode)
    {
        $this->m_masternode = &$masternode;
    }

    /**
     * Converts the given node flags to recordlist flags where possible.
     *
     * @param int $flags
     * @static
     */
    function convertFlags($flags)
    {
        $result = Tools::hasFlag($flags, Node::NF_MRA) ? self::RL_MRA : 0;
        $result |= Tools::hasFlag($flags, Node::NF_MRPA) ? self::RL_MRPA : 0;
        $result |= Tools::hasFlag($flags, Node::NF_LOCK) ? self::RL_LOCK : 0;
        $result |= Tools::hasFlag($flags, Node::NF_NO_SEARCH) ? self::RL_NO_SEARCH : 0;
        $result |= Tools::hasFlag($flags, Node::NF_NO_EXTENDED_SEARCH) ? self::RL_NO_EXTENDED_SEARCH
            : 0;
        $result |= Tools::hasFlag($flags, Node::NF_EXT_SORT) ? self::RL_EXT_SORT : 0;
        return $result;
    }

    /**
     * Render the recordlist
     *
     * @param Node $node the node
     * @param array $recordset the list of records
     * @param array $actions the default actions array
     * @param Integer $flags recordlist flags (see the top of this file)
     * @param array $suppressList fields we don't display
     * @param string $formName if embedded the form name in which we are embedded
     * @param array $navigation Navigation links
     * @param string $embedprefix The prefix for embeded fields
     * @return String The rendered recordlist
     */
    function render(
        &$node,
        $recordset,
        $actions,
        $flags = 0,
        $suppressList = "",
        $formName = "",
        $navigation = array(),
        $embedprefix = ""
    ) {
        $data = $this->getRecordlistData($node, $recordset, $actions, $flags, $suppressList, $formName, $navigation,
            $embedprefix);
        $ui = $this->m_node->getUi();
        $res = $ui->render($node->getTemplate("admin"), array(
            "rows" => $data["rows"],
            "header" => $data["header"],
            "search" => $data["search"],
            "sort" => $data["sort"],
            "total" => $data["total"],
            "searchstart" => $data["searchstart"],
            "searchend" => $data["searchend"],
            "sortstart" => $data["sortstart"],
            "sortend" => $data["sortend"],
            "liststart" => $data["liststart"],
            "listend" => $data["listend"],
            "listid" => $data["listid"],
            "mra" => $data["mra"]
        ), $this->m_node->m_module);
        return $res;
    }

    /**
     * Get records for a recordlist without actually rendering the recordlist.
     * @param Node $node the node
     * @param array $recordset the list of records
     * @param array $actions the default actions array
     * @param Integer $flags recordlist flags (see the top of this file)
     * @param array $suppressList fields we don't display
     * @param string $formName if embedded the form name in which we are embedded
     * @param array $navigation Navigation links
     * @param string $embedprefix The prefix for embeded fields
     * @return String The rendered recordlist
     */
    function getRecordlistData(
        &$node,
        $recordset,
        $actions,
        $flags = 0,
        $suppressList = array(),
        $formName = "",
        $navigation = array(),
        $embedprefix = ""
    ) {
        $this->setNode($node);
        $this->m_flags = $flags;

        $theme = Theme::getInstance();
        $sm = SessionManager::getInstance();
        $page = Page::getInstance();

        $listName = "rl_" . Tools::getUniqueId("normalRecordList");
        $page->register_script(Config::getGlobal("assets_url") . "javascript/recordlist.js");


        /* retrieve list array */
        $list = $this->listArray($recordset, $flags, "", $actions, $suppressList, $embedprefix);

        /* Check if some flags are still valid or not... */
        if (Tools::hasFlag($flags, self::RL_MRA) && (count($list["mra"]) == 0 || count($list["rows"]) == 0)) {
            $flags ^= self::RL_MRA;
        }
        if (!Tools::hasFlag($flags, self::RL_NO_SEARCH) && count($list["search"]) == 0) {
            $flags |= self::RL_NO_SEARCH;
        }
        if (Tools::hasFlag($flags,
                self::RL_MRPA) && (count($this->m_node->m_priority_actions) == 0 || count($list["rows"]) == 0)
        ) {
            $flags ^= self::RL_MRPA;
        } elseif (Tools::hasFlag($flags, self::RL_MRPA)) {
            $flags = ($flags | self::RL_MRA | self::RL_MRPA) ^ self::RL_MRA;
            if ($this->m_node->m_priority_max == 0) {
                $this->m_node->m_priority_max = $this->m_node->m_priority_min + count($list["rows"]) - 1;
            }
        }

        $orientation = Config::getGlobal('recordlist_orientation');

        $ui = $this->m_node->getUi();


        if (!is_object($ui) || !is_object($page)) {
            return null;
        }

        /**************/
        /* HEADER ROW */
        /**************/
        $headercols = array();

        if ($this->_hasActionColumn($list) && count($list["rows"]) == 0) {
            if ($orientation == "left" || $orientation == "both") {
                // empty cell above search button, if zero rows
                // if $orientation is empty, no search button is shown, so no empty cell is needed
                $headercols[] = array("content" => "&nbsp;");
            }
        }
        if (Tools::hasFlag($flags, self::RL_MRA) || Tools::hasFlag($flags, self::RL_MRPA)) {
            $headercols[] = array("content" => ""); // Empty leader on top of mra action list.
        }
        if (Tools::hasFlag($flags, self::RL_LOCK)) {
            $headercols[] = array("content" => '<img src="' . Config::getGlobal("assets_url") . 'images/lock_head.gif">');
        }
        if (($orientation == "left" || $orientation == "both") && ($this->_hasActionColumn($list) && count($list["rows"]) > 0)) {
            $headercols[] = array("content" => "");
        }
        //Todo: For speedup we must move hasFlag($this->m_flags, self::RL_EMBED out of cycle or to listArray()
        foreach (array_values($list["heading"]) as $head) {
            // make old recordlist compatible with new order specification
            if (!empty($head["order"])) {
                global $ATK_VARS;
                $head["url"] = $sm->sessionUrl(Tools::atkSelf() . '?atknodetype=' . $ATK_VARS["atknodetype"] . '&atkaction=' . $ATK_VARS["atkaction"] . '&atkorderby=' . rawurlencode($head["order"]));
            }

            if (Tools::hasFlag($this->m_flags, self::RL_EMBED) && !empty($head["url"])) {
                $head["url"] = str_replace("atkorderby=", "atkorderby{$embedprefix}=", $head["url"]);
            }

            if (empty($head["url"])) {
                $headercols[] = array("content" => $head["title"]);
            } else {
                $headercols[] = array("content" => Tools::href($head["url"], $head["title"]));
            }
        }

        if (($orientation == "right" || $orientation == "both") && ($this->_hasActionColumn($list) && count($list["rows"]) > 0)) {
            $headercols[] = array("content" => "");
        }

        if ($this->_hasActionColumn($list) && count($list["rows"]) == 0) {
            if ($orientation == "right" || $orientation == "both") {
                // empty cell above search button, if zero rows
                // if $orientation is empty, no search button is shown, so no empty cell is needed
                $headercols[] = array("content" => "&nbsp;");
            }
        }


        /**************/
        /*  SORT ROW  */
        /**************/
        $sortcols = array();
        $sortstart = "";
        $sortend = "";
        if (Tools::hasFlag($flags, self::RL_EXT_SORT)) {
            $button = '<input type="submit" value="' . Tools::atktext("sort") . '">';
            if (Tools::hasFlag($flags, self::RL_MRA) || Tools::hasFlag($flags, self::RL_MRPA)) {
                $sortcols[] = array("content" => ""); // Empty leader on top of mra action list.
            }
            if (Tools::hasFlag($flags, self::RL_LOCK)) {
                $sortcols[] = array("content" => "");
            }
            if ($orientation == "left" || $orientation == "both") {
                $sortcols[] = array("content" => $button);
            }

            $sortstart = '<a name="sortform"></a>' .
                '<form action="' . Tools::atkSelf() . '?' . SID . '" method="get">' .
                $sm->formState() .
                '<input type="hidden" name="atkstartat" value="0">'; // reset atkstartat to first page after a new sort

            foreach (array_keys($list["heading"]) as $key) {
                if (isset($list["sort"][$key])) {
                    $sortcols[] = array("content" => $list["sort"][$key]);
                }
            }

            $sortend = '</form>';

            if ($orientation == "right" || $orientation == "both") {
                $sortcols[] = array("content" => $button);
            }
        }

        /*             * *********** */
        /* SEARCH ROW */
        /*             * *********** */

        $searchcols = array();
        $searchstart = "";
        $searchend = "";
        if (!Tools::hasFlag($flags, self::RL_NO_SEARCH)) {
            $button = '<input type="submit" class="btn btn-default btn_search" value="' . Tools::atktext("search") . '">';
            if (!Tools::hasFlag($flags,
                    self::RL_NO_EXTENDED_SEARCH) && !$this->m_node->hasFlag(Node::NF_NO_EXTENDED_SEARCH)
            ) {
                $button .= ' ' . Tools::href(Tools::atkSelf() . "?atknodetype=" . $this->getMasterNodeType() . "&atkaction=" . $node->getExtendedSearchAction(),
                        "(" . Tools::atktext("search_extended") . ")", SessionManager::SESSION_NESTED);
            }

            $button = '<div class="search-buttons">'.$button.'</div>';

            $searchstart = '<a name="searchform"></a>';
            if (!Tools::hasFlag($this->m_flags, self::RL_EMBED)) {
                $searchstart .= '<form action="' . Tools::atkSelf() . '?' . SID . '" method="get">' . $sm->formState();
                $searchstart .= '<input type="hidden" name="atknodetype" value="' . $this->getMasterNodeType() . '">' .
                    '<input type="hidden" name="atkaction" value="' . $this->m_node->m_action . '">' . '<input type="hidden" name="atksmartsearch" value="clear">' .
                    '<input type="hidden" name="atkstartat" value="0">'; // reset atkstartat to first page after a new search;
            }

            if (Tools::hasFlag($flags, self::RL_MRA) || Tools::hasFlag($flags, self::RL_MRPA)) {
                $searchcols[] = array("content" => "");
            }
            if (Tools::hasFlag($flags, self::RL_LOCK)) {
                $searchcols[] = array("content" => "");
            }
            if ($orientation == "left" || $orientation == "both") {
                $searchcols[] = array("content" => $button);
            }

            foreach (array_keys($list["heading"]) as $key) {
                if (isset($list["search"][$key])) {
                    $searchcols[] = array("content" => $list["search"][$key]);
                } else {
                    $searchcols[] = array("content" => "");
                }
            }
            if ($orientation == "right" || $orientation == "both") {
                $searchcols[] = array("content" => $button);
            }

            $searchend = "";
            if (!Tools::hasFlag($this->m_flags, self::RL_EMBED)) {
                $searchend = '</form>';
            }
        }

        /*             * **************************************** */
        /* MULTI-RECORD-(PRIORITY-)ACTIONS FORM DATA */
        /*             * **************************************** */
        $liststart = "";
        $listend = "";
        if (Tools::hasFlag($flags, self::RL_MRA) || Tools::hasFlag($flags, self::RL_MRPA)) {
            $page->register_script(Config::getGlobal("assets_url") . "javascript/formselect.js");

            if (!Tools::hasFlag($flags, self::RL_EMBED)) {
                if (empty($formName)) {
                    $formName = $listName;
                }
                $liststart = '<form id="' . $formName . '" name="' . $formName . '" method="post">' .
                    $sm->formState(SessionManager::SESSION_DEFAULT) .
                    '<input type="hidden" name="atknodetype" value="' . $this->getMasterNodeType() . '">' .
                    '<input type="hidden" name="atkaction" value="' . $this->m_node->m_action . '">';
                $listend = '</form>';
            }

            if (Tools::hasFlag($flags, self::RL_MRA)) {
                $liststart .= '<script language="javascript" type="text/javascript">var ' . $listName . ' = new Object();</script>';
            }
        }

        /********/
        /* ROWS */
        /********/
        $records = array();
        $keys = array_keys($actions);
        $actionurl = (count($actions) > 0) ? $actions[$keys[0]] : '';
        $actionloader = "rl_a['" . $listName . "'] = {};";
        $actionloader .= "\nrl_a['" . $listName . "']['base'] = '" . $sm->sessionVars($this->m_actionSessionStatus,
                1, $actionurl) . "';";
        $actionloader .= "\nrl_a['" . $listName . "']['embed'] = " . (Tools::hasFlag($flags, self::RL_EMBED)
                ? 'true' : 'false') . ";";

        if (isset($navigation["next"]) && isset($navigation["next"]["url"])) {
            $actionloader .= "\nrl_a['" . $listName . "']['next'] = '" . $navigation["next"]["url"] . "';";
        }
        if (isset($navigation["previous"]) && isset($navigation["previous"]["url"])) {
            $actionloader .= "\nrl_a['" . $listName . "']['previous'] = '" . $navigation["previous"]["url"] . "';";
        }

        for ($i = 0, $_i = count($list["rows"]); $i < $_i; $i++) {
            $record = array();

            /* Special rowColor method makes it possible to change the row color based on the record data.
             * the method can return a simple value (which will be used for the normal row color), or can be
             * an array, in which case the first element will be the normal row color, and the second the mouseover
             * row color, example: function rowColor(&$record, $num) { return array('red', 'blue'); }
             */
            $method = "rowColor";
            $bgn = "";
            $bgh = "";
            if (method_exists($this->m_node, $method)) {
                $bgn = $this->m_node->$method($recordset[$i], $i);
                if (is_array($bgn)) {
                    list($bgn, $bgh) = $bgn;
                }
            }


            /* alternate colors of rows */
            $record["background"] = $bgn;
            $record["highlight"] = $bgh;
            $record["rownum"] = $i;
            $record["id"] = $listName . '_' . $i;
            $record["type"] = $list["rows"][$i]["type"];

            /* multi-record-priority-actions -> priority selection */
            if (Tools::hasFlag($flags, self::RL_MRPA)) {
                $select = '<select name="' . $listName . '_atkselector[]">' .
                    '<option value="' . rawurlencode($list["rows"][$i]["selector"]) . '"></option>';
                for ($j = $this->m_node->m_priority_min; $j <= $this->m_node->m_priority_max; $j++) {
                    $select .= '<option value="' . $j . '">' . $j . '</option>';
                }
                $select .= '</select>';
                $record["cols"][] = array("content" => $select, "type" => "mrpa");
            } /* multi-record-actions -> checkbox */ elseif (Tools::hasFlag($flags, self::RL_MRA)) {
                if (count($list["rows"][$i]["mra"]) > 0) {
                    $record["cols"][] = array(
                        "content" => '<input type="checkbox" name="' . $listName . '_atkselector[]" value="' . htmlentities($list["rows"][$i]["selector"]) . '" class="atkcheckbox" onclick="if (this.disabled) this.checked = false">' .
                            '<script language="javascript"  type="text/javascript">' . $listName . '["' . htmlentities($list["rows"][$i]["selector"]) . '"] = new Array("' . implode($list["rows"][$i]["mra"],
                                '","') . '");</script>',
                        "type" => "mra"
                    );
                } else {
                    $record["cols"][] = array("content" => "");
                }
            }

            /* locked? */
            if (Tools::hasFlag($flags, self::RL_LOCK)) {
                if (is_array($list["rows"][$i]["lock"])) {
                    $alt = $list["rows"][$i]["lock"]["user_id"] . " / " . $list["rows"][$i]["lock"]["user_ip"];
                    $record["cols"][] = array(
                        "content" => '<img src="' . Config::getGlobal("assets_url") . 'images/lock.gif" alt="' . $alt . '" title="' . $alt . '" border="0">',
                        "type" => "lock"
                    );
                } else {
                    $record["cols"][] = array("content" => "");
                }
            }

            $str_actions = "<span class=\"actions\">";
            $actionloader .= "\nrl_a['" . $listName . "'][" . $i . "] = {};";
            $icons = (Config::getGlobal('recordlist_icons',
                $theme->getAttribute("recordlist_icons")) === false ||
            Config::getGlobal('recordlist_icons', $theme->getAttribute("recordlist_icons")) === 'false'
                ? false : true);

            foreach ($list["rows"][$i]["actions"] as $name => $url) {
                if (substr($url, 0, 11) == 'javascript:') {
                    $call = substr($url, 11);
                    $actionloader .= "\nrl_a['{$listName}'][{$i}]['{$name}'] = function() { $call; };";
                } else {
                    $actionloader .= "\nrl_a['{$listName}'][{$i}]['{$name}'] = '$url';";
                }

                if ($icons == true) {
                    $icon = $theme->iconPath(strtolower($name), "recordlist", $this->m_node->m_module);
                    $link = sprintf('<img class="recordlist" border="0" src="%1$s" alt="%2$s" title="%2$s">', $icon,
                        Tools::atktext($name, $this->m_node->m_module, $this->m_node->m_type));
                } else {
                    $link = Tools::atktext($name, $this->m_node->m_module, $this->m_node->m_type);
                }

                $confirmtext = "false";
                if (Config::getGlobal("recordlist_javascript_delete") && $name == "delete") {
                    $confirmtext = "'" . $this->m_node->confirmActionText($name) . "'";
                }
                $str_actions .= '<a href="' . "javascript:rl_do('$listName',$i,'$name',$confirmtext);" . '">' . $link . '</a>&nbsp;';
            }

            $str_actions .= "</span>";
            /* actions (left) */
            if ($orientation == "left" || $orientation == "both") {
                if (!empty($list["rows"][$i]["actions"])) {
                    $record["cols"][] = array("content" => $str_actions, "type" => "actions");
                } else {
                    if ($this->_hasActionColumn($list)) {
                        $record["cols"][] = array("content" => "");
                    }
                }
            }

            /* columns */
            foreach ($list["rows"][$i]["data"] as $html) {
                $record["cols"][] = array("content" => $html, "type" => "data");
            }

            /* actions (right) */
            if ($orientation == "right" || $orientation == "both") {
                if (!empty($list["rows"][$i]["actions"])) {
                    $record["cols"][] = array("content" => $str_actions, "type" => "actions");
                } else {
                    if ($this->_hasActionColumn($list)) {
                        $record["cols"][] = array("content" => "");
                    }
                }
            }

            $records[] = $record;
        }

        $page->register_loadscript($actionloader);
        $this->m_actionloader = $actionloader;

        /*             * ********** */
        /* TOTAL ROW */
        /*             * ********** */
        $totalcols = array();

        if (count($list["total"]) > 0) {
            if (Tools::hasFlag($flags, self::RL_MRA) || Tools::hasFlag($flags, self::RL_MRPA)) {
                $totalcols[] = array("content" => "");
            }
            if (Tools::hasFlag($flags, self::RL_LOCK)) {
                $totalcols[] = array("content" => "");
            }
            if (($orientation == "left" || $orientation == "both") && ($this->_hasActionColumn($list) && count($list["rows"]) > 0)) {
                $totalcols[] = array("content" => "");
            }

            foreach (array_keys($list["heading"]) as $key) {
                $totalcols[] = array(
                    "content" => (isset($list["total"][$key])
                        ? $list["total"][$key] : "")
                );
            }

            if (($orientation == "right" || $orientation == "both") && ($this->_hasActionColumn($list) && count($list["rows"]) > 0)) {
                $totalcols[] = array("content" => "");
            }
        }

        /*             * ********************************************** */
        /* MULTI-RECORD-PRIORITY-ACTION FORM (CONTINUED) */
        /*             * ********************************************** */
        $mra = "";
        if (Tools::hasFlag($flags, self::RL_MRPA)) {
            $target = $sm->sessionUrl(Tools::atkSelf() . '?atknodetype=' . $this->getMasterNodeType(),
                SessionManager::SESSION_NESTED);

            /* multiple actions -> dropdown */
            if (count($this->m_node->m_priority_actions) > 1) {
                $mra = '<select name="' . $listName . '_atkaction">' .
                    '<option value="">' . Tools::atktext("with_selected") . ':</option>';

                foreach ($this->m_node->m_priority_actions as $name) {
                    $mra .= '<option value="' . $name . '">' . Tools::atktext($name) . '</option>';
                }

                $mra .= '</select>&nbsp;' . $this->getCustomMraHtml() .
                    '<input type="button" class="btn" value="' . Tools::atktext("submit") . '" onclick="atkSubmitMRPA(\'' . $listName . '\', this.form, \'' . $target . '\')">';
            } /* one action -> only the submit button */ else {
                $mra = $this->getCustomMraHtml() . '<input type="hidden" name="' . $listName . '_atkaction" value="' . $this->m_node->m_priority_actions[0] . '">' .
                    '<input type="button" class="btn" value="' . Tools::atktext($this->m_node->m_priority_actions[0]) . '" onclick="atkSubmitMRPA(\'' . $listName . '\', this.form, \'' . $target . '\')">';
            }
        }


        /*             * ************************************* */
        /* MULTI-RECORD-ACTION FORM (CONTINUED) */
        /*             * ************************************* */ elseif (Tools::hasFlag($flags, self::RL_MRA)) {
            $target = $sm->sessionUrl(Tools::atkSelf() . '?atknodetype=' . $this->m_node->atkNodeType() . '&atktarget=' . $this->m_node->m_postvars['atktarget'] . '&atktargetvar=' . $this->m_node->m_postvars['atktargetvar'] . '&atktargetvartpl=' . $this->m_node->m_postvars['atktargetvartpl'],
                SessionManager::SESSION_NESTED);

            $mra = (count($list["rows"]) > 1 ?
                '<a href="javascript:updateSelection(\'' . $listName . '\', document.forms[\'' . $formName . '\'], \'all\')">' . Tools::atktext("select_all") . '</a> / ' .
                '<a href="javascript:updateSelection(\'' . $listName . '\', document.forms[\'' . $formName . '\'], \'none\')">' . Tools::atktext("deselect_all") . '</a> / ' .
                '<a href="javascript:updateSelection(\'' . $listName . '\', document.forms[\'' . $formName . '\'], \'invert\')">' . Tools::atktext("select_invert") . '</a> '
                :
                '');

            /* multiple actions -> dropdown */
            if (count($list["mra"]) > 1) {
                $mra .= '<select name="' . $listName . '_atkaction" onchange="javascript:updateSelectable(\'' . $listName . '\', this.form)">' .
                    '<option value="">' . Tools::atktext("with_selected") . ':</option>';

                foreach ($list["mra"] as $name) {
                    if ($this->m_node->allowed($name)) {
                        $mra .= '<option value="' . $name . '">' . Tools::atktext($name,
                                $this->m_node->m_module, $this->m_node->m_type) . '</option>';
                    }
                }

                $mra .= '</select>&nbsp;' . $this->getCustomMraHtml() .
                    '<input type="button" class="btn" value="' . Tools::atktext("submit") . '" onclick="atkSubmitMRA(\'' . $listName . '\', this.form, \'' . $target . '\')">';
            } /* one action -> only the submit button */ else {
                if ($this->m_node->allowed($list["mra"][0])) {
                    $mra .= '&nbsp; <input type="hidden" name="' . $listName . '_atkaction" value="' . $list["mra"][0] . '">' .
                        $this->getCustomMraHtml() .
                        '<input type="button" class="btn" value="' . Tools::atktext($list["mra"][0],
                            $this->m_node->m_module,
                            $this->m_node->m_type) . '" onclick="atkSubmitMRA(\'' . $listName . '\', this.form, \'' . $target . '\')">';
                }
            }
        }

        if (Config::getGlobal("use_keyboard_handler")) {
            $kb = Keyboard::getInstance();
            $kb->addRecordListHandler($listName, '', count($records));
        }

        $recordListData = array(
            "rows" => $records,
            "header" => $headercols,
            "search" => $searchcols,
            "sort" => $sortcols,
            "total" => $totalcols,
            "searchstart" => $searchstart,
            "searchend" => $searchend,
            "sortstart" => $sortstart,
            "sortend" => $sortend,
            "liststart" => $liststart,
            "listend" => $listend,
            "listid" => $listName,
            "mra" => $mra
        );

        return $recordListData;

    }

    /**
     * Checks wether the recordlist should display a column which holds the actions.
     *
     * @access private
     * @param array $list The recordlist data
     * @return bool Wether the list should display an extra column to hold the actions
     */
    function _hasActionColumn($list)
    {
        if ($this->m_hasActionColumn == 0) {
            // when there's a search bar, we always need an extra column (for the button)
            if (!Tools::hasFlag($this->m_flags, self::RL_NO_SEARCH)) {
                $this->m_hasActionColumn = true;
            } // when there's an extended sort bar, we also need the column (for the sort button)
            else {
                if (Tools::hasFlag($this->m_flags, self::RL_EXT_SORT)) {
                    $this->m_hasActionColumn = true;
                } else {
                    // otherwise, it depends on whether one of the records has actions defined.
                    $this->m_hasActionColumn = false;

                    foreach ($list["rows"] as $record) {
                        if (!empty($record['actions'])) {
                            $this->m_hasActionColumn = true;
                            break;
                        }
                    }
                }
            }
        }
        return $this->m_hasActionColumn;
    }

    /**
     * Get custom mra HTML code
     *
     * @return string The custom HTML
     */
    function getCustomMraHtml()
    {
        if (method_exists($this->m_node, "getcustommrahtml")) {
            $output = $this->m_node->getCustomMraHtml();
            return $output;
        }
        return null;
    }

    /**
     * Function outputs an array with all information necessary to output a recordlist.
     *
     * @param array $recordset List of records that need to be displayed
     * @param Integer $flags Recordlist flags
     * @param string $prefix Prefix for each column name (used for subcalls)
     * @param array $actions List of default actions for each record
     * @param array $suppress An array of fields that you want to hide
     * @param string $embedprefix The prefix for embeded fields
     *
     * The result array contains the following information:
     *  "heading"  => for each visible column an array containing: "title" {, "url"}
     *  "search"   => for each visible column HTML input field(s) for searching
     *  "rows"     => list of rows, per row: "data", "actions", "mra", "record"
     *  "totalraw" => for each totalisable column the sum value field(s) (raw)
     *  "total"    => for each totalisable column the sum value (display)
     *  "mra"      => list of all multi-record actions
     *
     * @return array see above
     */
    function listArray(
        &$recordset,
        $flags = 0,
        $prefix = "",
        $actions = array(),
        $suppress = array(),
        $embedprefix = ""
    ) {
        if (!is_array($suppress)) {
            $suppress = array();
        }
        $result = array(
            "heading" => array(),
            "search" => array(),
            "rows" => array(),
            "totalraw" => array(),
            "total" => array(),
            "mra" => array()
        );

        if (Tools::hasFlag($this->m_flags, self::RL_EMBED) && $embedprefix) {
            $prefix = $embedprefix . "][";
        }

        $columnConfig = $this->m_node->getColumnConfig();

        /* get the heading and search columns */
        $atksearchpostvar = isset($this->m_node->m_postvars["atksearch"]) ? $this->m_node->m_postvars["atksearch"]
            : null;
        if (!Tools::hasFlag($flags, self::RL_NO_SEARCH)) {
            $this->m_node->setAttribSizes();
        }
        foreach (array_keys($this->m_node->m_attribIndexList) as $r) {
            $name = $this->m_node->m_attribIndexList[$r]["name"];
            if (!in_array($name, $suppress)) {
                $attribute = $this->m_node->m_attribList[$name];
                $attribute->addToListArrayHeader($this->m_node->m_action, $result, $prefix, $flags, $atksearchpostvar,
                    $columnConfig);
            }
        }

        /* actions array can contain multi-record-actions */
        if (count($actions) == 2 && count(array_diff(array_keys($actions), array("actions", "mra"))) == 0) {
            $mra = $actions["mra"];
            $actions = $actions["actions"];
        } else {
            $mra = $this->m_node->hasFlag(Node::NF_NO_DELETE) ? array() : array("delete");
        }

        /* get the rows */
        for ($i = 0, $_i = count($recordset); $i < $_i; $i++) {
            $result["rows"][$i] = array(
                "columns" => array(),
                "actions" => $actions,
                "mra" => $mra,
                "record" => &$recordset[$i],
                "data" => array()
            );
            $result["rows"][$i]["selector"] = $this->m_node->primaryKey($recordset[$i]);
            $result["rows"][$i]["type"] = "data";
            $row = &$result["rows"][$i];

            /* locked */
            if (Tools::hasFlag($flags, self::RL_LOCK)) {
                $result["rows"][$i]["lock"] = $this->m_node->m_lock->isLocked($result["rows"][$i]["selector"],
                    $this->m_node->m_table);
                if (is_array($result["rows"][$i]["lock"])) {
                    unset($row["actions"]["edit"]);
                    unset($row["actions"]["delete"]);
                    $row["mra"] = array();
                }
            }

            /* actions / mra */
            $this->m_node->collectRecordActions($row["record"], $row["actions"], $row["mra"]);
            $result["mra"] = array_merge($result["mra"], $row["mra"]);
            foreach ($row["actions"] as $name => $url) {
                if (!empty($url) && $this->m_node->allowed($name, $row["record"])) {
                    /* dirty hack */
                    $atkencoded = strpos($url, "_15B") > 0;

                    $url = str_replace("%5B", "[", $url);
                    $url = str_replace("%5D", "]", $url);
                    $url = str_replace("_1" . "5B", "[", $url);
                    $url = str_replace("_1" . "5D", "]", $url);

                    if ($atkencoded) {
                        $url = str_replace('[pk]', Tools::atkurlencode(rawurlencode($row["selector"]), false),
                            $url);
                    } else {
                        $url = str_replace('[pk]', rawurlencode($row["selector"]), $url);
                    }

                    $parser = new StringParser($url);
                    $url = $parser->parse($row["record"], true);
                    $row["actions"][$name] = $url;
                } else {
                    unset($row["actions"][$name]);
                }
            }

            /* columns */
            foreach (array_keys($this->m_node->m_attribIndexList) as $r) {
                $name = $this->m_node->m_attribIndexList[$r]["name"];
                if (!in_array($name, $suppress)) {
                    $attribute = $this->m_node->m_attribList[$name];
                    $attribute->addToListArrayRow($this->m_node->m_action, $result, $i, $prefix, $flags);
                }
            }
        }

        if (Tools::hasFlag($flags, self::RL_EXT_SORT) && $columnConfig->hasSubTotals()) {
            $totalizer = new Totalizer($this->m_node, $columnConfig);
            $result["rows"] = $totalizer->totalize($result["rows"]);
        }

        if (Tools::hasFlag($flags, self::RL_MRA)) {
            $result["mra"] = array_values(array_unique($result["mra"]));
        }

        return $result;
    }

    /**
     * Get the masternode
     *
     * @return Node The master node
     */
    function getMasterNode()
    {
        if (is_object($this->m_masternode)) {
            return $this->m_masternode;
        }
        return $this->m_node; // treat rendered node as master
    }

    /**
     * Get the nodetype of the master node
     *
     * @return string Modulename.nodename of the master node
     */
    function getMasterNodeType()
    {
        $node = $this->getMasterNode();
        return $node->atkNodeType();
    }

}
