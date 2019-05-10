<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Ui\Page;

/**
 * The ProfileAttribute is an attribute to edit a security profile.
 * The best way to use it is inside the class where you edit your
 * profile or usergroup records.
 *
 * @author Ivo Jansch <ivo@ibuildings.nl>
 */
class ProfileAttribute extends Attribute
{
    public $m_parentAttrName;
    public $m_accessField;

    /**
     * The database fieldtype: no storage, type undefined.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_UNSUPPORTED;

    /**
     * Constructor.
     *
     * @param string $name The name of the attribute
     * @param int $flags The flags of this attribute
     * @param string $parentAttrName
     * @return ProfileAttribute
     */
    public function __construct($name, $flags = 0, $parentAttrName = '')
    {
        $flags = $flags | self::AF_HIDE_SEARCH | self::AF_HIDE_LIST;
        parent::__construct($name, $flags);

        $this->m_parentAttrName = $parentAttrName;
        $this->m_accessField = Config::getGlobal('auth_accessfield');
        if (empty($this->m_accessField)) {
            $this->m_accessField = Config::getGlobal('auth_levelfield');
        }
    }

    /**
     * Load this record.
     *
     * @param Db $db The database object
     * @param array $record The record
     * @param string $mode
     *
     * @return array Array with loaded values
     */
    public function load($db, $record, $mode)
    {
        $query = $db->createQuery(Config::getGlobal('auth_accesstable'));
        $query->addField('node')->addField('action');
        $query->addCondition(
            Db::quoteIdentifier($this->m_accessField).'=:id',
            [':id' => $record[$this->m_ownerInstance->primaryKeyField()]]
        );

        $result = [];
        foreach($query->executeSelect() as $row) {
            $result[$row['node']][] = $row['action'];
        }

        return $result;
    }

    /**
     * Store the value of this attribute in the database.
     *
     * @param Db $db The database object
     * @param array $record The record which holds the values to store
     * @param string $mode The mode we're in
     *
     * @return bool True if succesfull, false if not
     */
    public function store($db, $record, $mode)
    {

        // Read the current actions available/editable and user rights before changing them
        $isAdmin = (SecurityManager::isUserAdmin() || $this->canGrantAll());
        $allActions = $this->getAllActions($record, false);
        $editableActions = $this->getEditableActions($record);

        $delquery = $db->createQuery(Config::getGlobal('auth_accesstable'));
        $delquery->addCondition(
            Db::quoteIdentifier($this->m_accessField).'= :id',
            [':id' => $record[$this->m_ownerInstance->primaryKeyField()]]
        );

        if ($delquery->executeDelete()) {
            $checked = $record[$this->fieldName()];

            $children = [];
            if (!empty($this->m_parentAttrName)) {
                $children = $this->getChildGroups($db, $record[$this->m_ownerInstance->primaryKeyField()]);
            }

            foreach ($checked as $node => $actions) {
                $actions = array_unique($actions);

                $nodeModule = Tools::getNodeModule($node);
                $nodeType = Tools::getNodeType($node);

                $validActions = [];

                if (is_array($allActions[$nodeModule][$nodeType])) {
                    $validActions = array_intersect($actions, $allActions[$nodeModule][$nodeType]);
                }

                // If you're not an admin, leave out all actions which are not editable (none if no editable actions available)
                if (!$isAdmin) {
                    $validActions = isset($editableActions[$nodeModule][$nodeType]) ? array_intersect($validActions,
                        $editableActions[$nodeModule][$nodeType]) : [];
                }

                foreach ($validActions as $action) {
                    $query = $db->createQuery(Config::getGlobal('auth_accesstable'));
                    $query->addFields([
                        'node' => $node,
                        'action' => $action,
                        $this->m_accessField => $record[$this->m_ownerInstance->primaryKeyField()]
                    ]);

                    if (!$query->executeInsert()) {
                        // error.
                        return false;
                    }
                }

                if (Tools::count($children) > 0 && Tools::count($validActions) > 0) {
                    $query = $db->createQuery(Config::getGlobal('auth_accesstable'));
                    $query->addCondition($query->inCondition(Db::quoteIdentifier($this->m_accessField), $children));
                    $query->addCondition('node = :node', [':node' => $node]);
                    $query->addCondition($query->notinCondition('action', $validActions));

                    if (!$query->executeDelete()) {
                        // error.
                        return false;
                    }
                }
            }
        }

        return true;
    }

    /**
     * Checks whether the current user has the 'grantall' privilege (if such a
     * privilege exists; this is determined by the application by setting
     * $config_auth_grantall_privilege.
     *
     * @return bool
     */
    public function canGrantAll()
    {
        $privilege_setting = Config::getGlobal('auth_grantall_privilege');

        if ($privilege_setting != '') {
            $securityManager = SecurityManager::getInstance();

            list($mod, $node, $priv) = explode('.', $privilege_setting);

            return $securityManager->allowed($mod.'.'.$node, $priv);
        }

        return false;
    }

    /**
     * Retrieve all possible module/node actions.
     *
     * @param array $record The record
     * @param bool $splitPerSection
     *
     * @return array Array with actions
     */
    public function getAllActions($record, $splitPerSection = false)
    {
        $atk = Atk::getInstance();

        $result = [];

        // hierarchic groups, only return actions of parent (if this record has a parent)
        $parentAttr = $this->m_parentAttrName;
        if (!empty($parentAttr) && is_numeric($record[$parentAttr])) {
            $query = $this->getDb()->createQuery(Config::getGlobal('auth_accesstable'));
            $query->setDistinct(true);
            $query->addField('node')->addField('action');
            $query->addCondition(Db::quoteIdentifier($this->m_accessField).'=:id', [':id' => $record[$parentAttr]]);
            $rows = $query->executeSelect();

            foreach ($rows as $row) {
                $module = Tools::getNodeModule($row['node']);
                $node = Tools::getNodeType($row['node']);
                $result[$module][$module][$node][] = $row['action'];
            }
        } // non-hierarchic groups, or root
        else {

            // get nodes for each module
            foreach (array_keys($atk->g_modules) as $module) {
                $instance = $atk->atkGetModule($module);
                if (method_exists($instance, 'getNodes')) {
                    $instance->getNodes();
                }
            }

            // retrieve all actions after we registered all actions
            $result = $atk->g_nodes;
        }

        if (!$splitPerSection) {
            $temp = [];
            foreach ($result as $section => $modules) {
                foreach ($modules as $module => $nodes) {
                    if (!is_array($temp[$module])) {
                        $temp[$module] = [];
                    }

                    $temp[$module] = array_merge($temp[$module], $nodes);
                }
            }

            $result = $temp;
        }

        return $result;
    }

    /**
     * Returns a list of actions that should be editable by the user.
     *
     * @param array $record The record
     *
     * @return array Array with editable actions
     */
    public function getEditableActions($record)
    {
        $db = $this->getDb();
        $user = SecurityManager::atkGetUser();
        $levels = $user['level'];

        if (!is_array($levels)) {
            $levels = [$levels];
        }

        // retrieve editable actions by user's levels
        $rows = [];
        if ($levels) {
            $query = $this->getDb()->createQuery(Config::getGlobal('auth_accesstable'));
            $query->setDistinct(true);
            $query->addField('node')->addField('action');
            $query->addCondition($query->inCondition(Db::quoteIdentifier($this->m_accessField), $levels));
            $rows = $query->executeSelect();
        }

        $result = [];
        foreach ($rows as $row) {
            $module = Tools::getNodeModule($row['node']);
            $node = Tools::getNodeType($row['node']);
            $result[$module][$node][] = $row['action'];
        }

        return $result;
    }

    /**
     * Get child groups.
     *
     * @param Db $db The database object
     * @param int $id The id to search for
     *
     * @return array
     */
    public function getChildGroups($db, $id)
    {
        $result = [];
        if (!is_numeric($id)) {
            return $result;
        }

        $query = $db->createQuery($this->m_ownerInstance->m_table);
        $query->addField($this->m_ownerInstance->primaryKeyField());
        $query->addCondition(Db::quoteIdentifier($this->m_parentAttrName).'=:id', [':id' => $id]);

        foreach ($query->executeSelect() as $row) {
            $id = $row[$this->m_ownerInstance->primaryKeyField()];
            $result = array_merge($result, array($id), $this->getChildGroups($db, $id));
        }

        return $result;
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">).
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        // get checks
        $checked = $record[$this->fieldName()];

        // rebuild hidden fields from checked boxes
        $rights = '';

        foreach ($checked as $key => $val) {
            for ($i = 0; $i <= Tools::count($val) - 1; ++$i) {
                $value = $key.'.'.$val[$i];
                $rights .= '<input type="hidden" name="rights[]" value="'.$value.'">';
            }
        }

        return $rights;
    }

    /**
     * Initially use an empty rights array.
     *
     * @return array initial rights
     */
    public function initialValue()
    {
        return [];
    }

    /**
     * Display rights.
     * It will only display the rights & nodes that are selected for the user.
     *
     * @param array $record
     * @param string $mode
     *
     * @return string Displayable string
     */
    public function display($record, $mode)
    {
        $page = Page::getInstance();
        $page->register_script(Config::getGlobal('assets_url').'javascript/profileattribute.js');
        $this->_restoreDivStates($page);

        $icons = "var ATK_PROFILE_ICON_OPEN = '".Config::getGlobal('icon_plussquare')."';";
        $icons .= "var ATK_PROFILE_ICON_CLOSE = '".Config::getGlobal('icon_minussquare')."';";
        $page->register_scriptcode($icons);

        $result = '';
        $isAdmin = (SecurityManager::isUserAdmin() || $this->canGrantAll());

        $allActions = $this->getAllActions($record, false);
        $editableActions = $this->getEditableActions($record);
        $selectedActions = $this->getSelectedActions($record);

        $showModule = Tools::count($allActions) > 1 && ($isAdmin || Tools::count($editableActions) > 1);

        $firstModule = true;

        foreach ($allActions as $module => $nodes) {
            $module_result = '';

            foreach ($nodes as $node => $actions) {
                $showBox = $isAdmin || Tools::count(array_intersect($actions,
                        (is_array($editableActions[$module][$node]) ? $editableActions[$module][$node] : array()))) > 0;
                $display_node_str = false;
                $display_tabs_str = false;
                $node_result = '';
                $permissions_string = '';
                $tab_permissions_string = '';

                foreach ($actions as $action) {
                    $isSelected = isset($selectedActions[$module][$node]) && in_array($action, $selectedActions[$module][$node]);

                    // If the action of a node is selected for this user we will show the node,
                    // otherwise we won't
                    if ($isSelected) {
                        $display_node_str = true;
                        if (substr($action, 0, 4) == 'tab_') {
                            $display_tabs_str = true;
                            $tab_permissions_string .= $this->permissionName($action, $node, $module).'&nbsp;&nbsp;&nbsp;';
                        } else {
                            $permissions_string .= $this->permissionName($action, $node, $module).'&nbsp;&nbsp;&nbsp;';
                        }
                    }
                }

                if ($showBox) {
                    $node_result .= '<b>'.Tools::atktext($node, $module).'</b><br>';
                    $node_result .= $permissions_string;
                    if ($display_tabs_str) {
                        $node_result .= '<br>Tabs:&nbsp;'.$tab_permissions_string;
                    }
                    $node_result .= "<br /><br />\n";
                } else {
                    $node_result .= $permissions_string;
                    if ($display_tabs_str) {
                        $node_result .= '<br>Tabs:&nbsp;'.$tab_permissions_string;
                    }
                }

                if ($display_node_str) {
                    $module_result .= $node_result;
                }
            }

            // If we have more then one module, split up the module results by collapsable div's
            if ($showModule) {
                if ($module_result) {
                    if ($firstModule) {
                        $firstModule = false;
                    } else {
                        $result .= '</div>';
                    }

                    $result .= sprintf("<span onclick=\"%s\" style=\"cursor: pointer; font-size: 110%%; font-weight: bold;display:block;\"><i class=\"%s\" id=\"img_div_$module\"></i> %s</span>",
                        "ATK.ProfileAttribute.profile_swapProfileDiv('div_$module'); return false;", Config::getGlobal('icon_plussquare'),
                        Tools::atktext(array("title_$module", $module), $module));
                    $result .= "<div id=\"div_$module\" name=\"div_$module\" style=\"display: none;padding-left: 15px;\">";
                    $result .= "<input type=\"hidden\" name=\"divstate['div_$module']\" id=\"divstate['div_$module']\" value=\"closed\" />";
                    $result .= $module_result;
                }
            }
        }

        return $result;
    }

    /**
     * Restore divs states.
     *
     * @param Page $page
     */
    public function _restoreDivStates($page)
    {
        $postvars = $this->m_ownerInstance->m_postvars;
        if (!isset($postvars['divstate']) || !is_array($postvars['divstate']) || sizeof($postvars['divstate']) == 0) {
            return;
        }

        $divstate = $postvars['divstate'];
        $onLoadScript = '';

        foreach ($divstate as $key => $value) {
            $key = substr($key, 2, -2);
            if ($value == 'opened') {
                $onLoadScript .= "ATK.ProfileAttribute.profile_swapProfileDiv('$key');";
            }
        }
        $page->register_loadscript($onLoadScript);
    }

    /**
     * Returns the currently selected actions.
     *
     * @param array $record The record
     *
     * @return array array with selected actions
     */
    public function getSelectedActions($record)
    {
        $selected = $record[$this->fieldName()];

        $result = [];
        foreach ($selected as $node => $actions) {
            $module = Tools::getNodeModule($node);
            $node = Tools::getNodeType($node);
            $result[$module][$node] = $actions;
        }

        return $result;
    }

    /**
     * Return the translated name of a permission.
     *
     * @param string $action The name of the action
     * @param string $nodename The name of the node
     * @param string $modulename The name of the module
     *
     * @return string The translated permission name
     */
    public function permissionName($action, $nodename = '', $modulename = '')
    {
        $keys = array(
            'permission_'.$modulename.'_'.$nodename.'_'.$action,
            'action_'.$modulename.'_'.$nodename.'_'.$action,
            'permission_'.$nodename.'_'.$action,
            'action_'.$nodename.'_'.$action,
            'permission_'.$action,
            'action_'.$action,
            $action,
        );

        // don't use text() function of attribute, because of auto module detection
        $label = Tools::atktext($keys, $modulename, $nodename);

        return $label;
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
     * @return string A piece of htmlcode for editing this attribute
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $page = Page::getInstance();

        $icons = "var ATK_PROFILE_ICON_OPEN = '".Config::getGlobal('icon_plussquare')."';";
        $icons .= "var ATK_PROFILE_ICON_CLOSE = '".Config::getGlobal('icon_minussquare')."';";
        $page->register_scriptcode($icons);
        $page->register_script(Config::getGlobal('assets_url').'javascript/profileattribute.js');

        $this->_restoreDivStates($page);

        $result = '<div align="right">[<a href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkAll(\''.$this->fieldName().'\'); return false;">'.Tools::atktext('check_all').'</a> | <a href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkNone(\''.$this->fieldName().'\'); return false;">'.Tools::atktext('check_none').'</a> | <a href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkInvert(\''.$this->fieldName().'\'); return false;">'.Tools::atktext('invert_selection').'</a>]</div>';

        $isAdmin = (SecurityManager::isUserAdmin() || $this->canGrantAll());
        $allActions = $this->getAllActions($record, true);
        $editableActions = $this->getEditableActions($record);
        $selectedActions = $this->getSelectedActions($record);

        foreach ($allActions as $section => $modules) {
            $result .= '<div class="profileSection">';

            $result .= "<span onclick=\"ATK.ProfileAttribute.profile_swapProfileDiv('div_$section');\" style=\"cursor: pointer; font-size: 110%; font-weight: bold\">";
            $result .= '  <i class="'.Config::getGlobal('icon_plussquare')."\" id=\"img_div_$section\"></i> ".Tools::atktext(array("title_$section", $section),
                    $section);
            $result .= '</span><br/>';

            $result .= "<div id='div_$section' name='div_$section' style='display: none; padding-left: 15px' class='checkbox'>";

            $result .= "  <input type='hidden' name=\"divstate['div_$section']\" id=\"divstate['div_$section']\" value='closed' />";

            $result .= '  <div style="font-size: 80%; margin-top: 4px; margin-bottom: 4px" >
                  [<a  style="font-size: 100%" href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkAllByValue(\''.$this->fieldName().'\',\''.$section.'.\'); return false;">'.Tools::atktext('check_all',
                    'atk').'</a> | <a  style="font-size: 100%" href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkNoneByValue(\''.$this->fieldName().'\',\''.$section.'.\'); return false;">'.Tools::atktext('check_none',
                    'atk').'</a> | <a  style="font-size: 100%" href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkInvertByValue(\''.$this->fieldName().'\',\''.$section.'.\'); return false;">'.Tools::atktext('invert_selection',
                    'atk').'</a>]';
            $result .= '  </div>';
            $result .= '  <br>';

            foreach ($modules as $module => $nodes) {
                foreach ($nodes as $node => $actions) {
                    $showBox = $isAdmin || Tools::count(array_intersect($actions,
                            (is_array($editableActions[$module][$node]) ? $editableActions[$module][$node] : array()))) > 0;

                    if ($showBox) {
                        $result .= '<b>'.Tools::atktext($node, $module).'</b><br>';
                    }

                    $tabs_str = '';
                    $display_tabs_str = false;

                    // Draw action checkboxes
                    foreach ($actions as $action) {
                        $temp_str = '';

                        $isEditable = $isAdmin || Tools::atk_in_array($action, $editableActions[$module][$node]);
                        $isSelected = isset($selectedActions[$module][$node]) && in_array($action, $selectedActions[$module][$node]);

                        if ($isEditable) {
                            if (substr($action, 0, 4) == 'tab_') {
                                $display_tabs_str = true;
                            }

                            $temp_str .= '<label>';
                            $temp_str .= '<input type="checkbox" name="'.$this->fieldName().'[]" '.$this->getCSSClassAttribute('atkcheckbox').' value="'.$section.'.'.$module.'.'.$node.'.'.$action.'" ';
                            $temp_str .= ($isSelected ? ' checked="checked"' : '').'>';
                            $temp_str .= ' '.$this->permissionName($action, $node, $module);
                            $temp_str .= '</label>';
                        }

                        if (substr($action, 0, 4) == 'tab_') {
                            $tabs_str .= $temp_str;
                        } else {
                            $result .= $temp_str;
                        }
                    }

                    if ($display_tabs_str) {
                        $result .= '<br>Tabs:&nbsp;';
                    }

                    $result .= $tabs_str;

                    if ($showBox) {
                        $result .= "<br /><br />\n";
                    }
                }
            }
            $result .= '  </div>'; // end div_$section
            $result .= '</div>'; // end profileSection
        }

        return $result;
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * For the regular Attribute, this means getting the field with the
     * same name as the attribute from the html posting.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     *
     * @return string The internal value
     */
    public function fetchValue($postvars)
    {
        $checkboxes = parent::fetchValue($postvars) ?? [];

        $actions = [];
        for ($i = 0; $i < Tools::count($checkboxes); ++$i) {
            $node = $action = null;
            $elems = explode('.', $checkboxes[$i]);
            if (Tools::count($elems) == 4) {
                $node = $elems[1].'.'.$elems[2];
                $action = $elems[3];
            } else {
                if (Tools::count($elems) == 3) {
                    $node = $elems[1];
                    $action = $elems[2];
                } else {
                    // never happens..
                    Tools::atkdebug('profileattribute encountered incomplete combination');
                }
            }
            if ($node && $action) {
                $actions[$node][] = $action;
            }
        }

        return $actions;
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * Note that not all modes may be supported by the database driver.
     * Compare this list to the one returned by the databasedriver, to
     * determine which searchmodes may be used.
     *
     * @return array List of supported searchmodes
     */
    public function getSearchModes()
    {
        // exact match and substring search should be supported by any database.
        // (the LIKE function is ANSI standard SQL, and both substring and wildcard
        // searches can be implemented using LIKE)
        // Possible values
        //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
        return [];
    }
}
