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
     * Constructor.
     *
     * @param string $name The name of the attribute
     * @param int $flags The flags of this attribute
     * @param string $parentAttrName
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
        $query = 'SELECT *
                FROM ' . Config::getGlobal('auth_accesstable') . '
                WHERE ' . $this->m_accessField . "='" . $record[$this->m_ownerInstance->primaryKeyField()] . "'";

        $result = [];
        $rows = $db->getRows($query);
        for ($i = 0; $i < Tools::count($rows); ++$i) {
            $result[$rows[$i]['node']][] = $rows[$i]['action'];
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

        $delquery = 'DELETE FROM ' . Config::getGlobal('auth_accesstable') . '
                   WHERE ' . $this->m_accessField . "='" . $record[$this->m_ownerInstance->primaryKeyField()] . "'";

        if ($db->query($delquery)) {
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
                    $query = 'INSERT INTO ' . Config::getGlobal('auth_accesstable') . ' (node, action, ' . $this->m_accessField . ') ';
                    $query .= "VALUES ('" . $db->escapeSQL($node) . "','" . $db->escapeSQL($action) . "','" . $record[$this->m_ownerInstance->primaryKeyField()] . "')";

                    if (!$db->query($query)) {
                        // error.
                        return false;
                    }
                }

                if (Tools::count($children) > 0 && Tools::count($validActions) > 0) {
                    $query = 'DELETE FROM ' . Config::getGlobal('auth_accesstable') . ' ' . 'WHERE ' . $this->m_accessField . ' IN (' . implode(',',
                            $children) . ') ' . "AND node = '" . $db->escapeSQL($node) . "' " . "AND action NOT IN ('" . implode("','", $validActions) . "')";

                    if (!$db->query($query)) {
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

            return $securityManager->allowed($mod . '.' . $node, $priv);
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
            $db = $this->getDb();
            $query = 'SELECT DISTINCT node, action FROM ' . Config::getGlobal('auth_accesstable') . ' ' . 'WHERE ' . $this->m_accessField . ' = ' . $record[$parentAttr];
            $rows = $db->getRows($query);

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
                    if (!isset($temp[$module])) {
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

        $escapedLevels = [];
        foreach ($levels as $currLevel) {
            $escapedLevels[] = "'" . $db->escapeSQL($currLevel) . "'";
        }
        $levels = implode(',', $escapedLevels);

        // retrieve editable actions by user's levels
        $rows = [];
        if ($levels) {
            $db = $this->getDb();
            $query = 'SELECT DISTINCT node, action FROM ' . Config::getGlobal('auth_accesstable') . ' WHERE ' . $this->m_accessField . ' IN (' . $levels . ')';
            $rows = $db->getRows($query);
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

        $query = 'SELECT ' . $this->m_ownerInstance->primaryKeyField() . ' ' . 'FROM ' . $this->m_ownerInstance->m_table . ' ' . 'WHERE ' . $this->m_parentAttrName . " = $id";

        $rows = $db->getRows($query);
        foreach ($rows as $row) {
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
                $value = $key . '.' . $val[$i];
                $rights .= '<input type="hidden" name="rights[]" value="' . $value . '">';
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
     * @param array<string, mixed> $record
     * @param string $mode
     *
     * @return string Displayable string
     */
    public function display(array $record, string $mode): string
    {
        Page::getInstance()->register_script(Config::getGlobal('assets_url') . 'javascript/profileattribute.js');


        $isAdmin = (SecurityManager::isUserAdmin() || $this->canGrantAll());

        $allActions = $this->getAllActions($record, false);
        $editableActions = $this->getEditableActions($record);
        $selectedActions = $this->getSelectedActions($record);
        $showModule = Tools::count($allActions) > 1 && ($isAdmin || Tools::count($editableActions) > 1);

        $result = '<div class="row">';

        foreach ($allActions as $module => $nodes) {
            $module_result = '';
            $hasAnyPermissions = false;

            if ($showModule) {
                $module_result .= '<div class="col-12 col-lg-6 col-xl-3" id="div_' . $module . '">';

                $module_result .= '<div class="card card-default">';

                $module_result .= '<div class="card-header">
                                <h3 class="card-title">' . Tools::atktext(["title_$module", $module], $module) . '</h3>
                                <div class="card-tools">
                                    <button type="button" class="btn btn-tool" data-card-widget="collapse"><i class="fas fa-minus"></i></button>
                                </div>
                            </div>';

                $module_result .= '<div class="card-body">';

                $isFirstModule = true;
                foreach ($nodes as $node => $actions) {

                    $hasAnyEnabledPermission = false;
                    $permissionsResult = '';


                    $permissionsPills = '<div class="row justify-content-start">';
                    foreach ($actions as $action) {

                        // If the action of a node is selected for this user we will show the node otherwise we won't
                        if (isset($selectedActions[$module][$node]) && in_array($action, $selectedActions[$module][$node])) {
                            $hasAnyEnabledPermission = true;
                            $hasAnyPermissions = true;
                             $permissionsPills .= "<span class='mt-1 mr-1 badge-sm badge-pill badge-secondary text-nowrap'>" . $this->permissionName($action, $node, $module) . "</span>";
                        }

                    }

                    $permissionsPills .= '</div>'; //end-row

                    $permissionsResult .= '<div class="container-fluid">';
                    $permissionsResult .= '<div class="row"><strong>' . Tools::atktext($node, $module) . '</strong></div>';
                    $permissionsResult .= $permissionsPills;
                    $permissionsResult .= '</div>';

                    if($isFirstModule){
                        $permissionsResult .='<hr>';
                        $isFirstModule = false;
                    }

                    if ($hasAnyEnabledPermission) {
                        $module_result .= $permissionsResult;
                    }
                }

                $module_result .= '</div>'; //end card-body
                $module_result .= '</div>'; //end card
                $module_result .= '</div>'; //end-col

                $result .= $hasAnyPermissions ? $module_result : '';

            }

        }

        $result .= "</div>"; //end-row

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
            'permission_' . $modulename . '_' . $nodename . '_' . $action,
            'action_' . $modulename . '_' . $nodename . '_' . $action,
            'permission_' . $nodename . '_' . $action,
            'action_' . $nodename . '_' . $action,
            'permission_' . $action,
            'action_' . $action,
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
        Page::getInstance()->register_script(Config::getGlobal('assets_url') . 'javascript/profileattribute.js');

        $allActions = $this->getAllActions($record, true);
        $selectedActions = $this->getSelectedActions($record);

        $result = '<div class="row">';
        foreach ($allActions as $section => $modules) {
            $result .= '<div class="col-12 col-lg-6 col-xl-4 profileSection">';

            $result .= '<div class="card card-default collapsed-card">';

            $result .= '<div class="card-header">
                <h3 class="card-title">' . Tools::atktext(["title_$section", $section], $section) . '</h3>

                <div class="card-tools">
                  <button type="button" class="btn btn-tool" data-card-widget="collapse"><i class="fas fa-plus"></i></button>
                </div>
                <!-- /.card-tools -->
              </div>';


            $result .= '<div class="card-body">';

            $result .= '<div class="row mt-1 mb-3 btn-group w-100 justify-content-center no-gutters">';
            $result .= '<button class="btn btn-xs btn-default" onclick="ATK.ProfileAttribute.profile_checkAllByValue(\'' . $this->fieldName() . '\',\'' . $section . '.\'); return false;">' . Tools::atktext('pf_check_all', 'atk') . '</button>';
            $result .= '<button class="btn btn-xs btn-default" onclick="ATK.ProfileAttribute.profile_checkNoneByValue(\'' . $this->fieldName() . '\',\'' . $section . '.\'); return false;">' . Tools::atktext('pf_check_none', 'atk') . '</button>';
            $result .= '<button class="btn btn-xs btn-default" onclick="ATK.ProfileAttribute.profile_checkInvertByValue(\'' . $this->fieldName() . '\',\'' . $section . '.\'); return false;">' . Tools::atktext('pf_invert_selection', 'atk') . '</button>';
            $result .= '</div>';

            foreach ($modules as $module => $nodes) {

                $i = 0;
                foreach ($nodes as $node => $actions) {
                    // Draw action checkboxes
                    $result .= "<div class='row no-gutters'>";
                    $result .= '<div class="col-12 mt-1 mb-1"><strong>' . Tools::atktext($node, $module) . '</strong></div>';
                    foreach ($actions as $action) {
                        $isSelected = isset($selectedActions[$module][$node]) && in_array($action, $selectedActions[$module][$node]);

                        $result .= '<div class="d-flex">';
                        $result .= '<div class="form-check form-check-inline">';
                        $result .= '<input type="checkbox" name="' . $this->fieldName() . '[]" class="form-check-input" value="' . $section . '.' . $module . '.' . $node . '.' . $action . '" ';
                        $result .= ($isSelected ? ' checked="checked"' : '') . '>';
                        $result .= '<label class="form-check-label text-nowrap" for="' . $this->fieldName() . '[]">' . $this->permissionName($action, $node, $module) . "</label>";
                        $result .= '</div>';
                        $result .= '</div>';
                    }

                    $result .= '</div>';
                    if ($i < count($nodes) - 1) {
                        $result .= '<hr>';
                    }
                    $i++;
                }
            }

            $result .= '</div>'; // end card-body
            $result .= '</div>'; // end card
            $result .= '</div>'; // end profileSection

        }

        $result .= "</div>"; //end row

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
        $checkboxes = [];
        if (isset($postvars[$this->fieldName()])) {
            $checkboxes = $postvars[$this->fieldName()];
        }

        $actions = [];
        for ($i = 0; $i < Tools::count($checkboxes); ++$i) {
            $node = $action = null;
            $elems = explode('.', $checkboxes[$i]);
            if (Tools::count($elems) == 4) {
                $node = $elems[1] . '.' . $elems[2];
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

    /**
     * Return the database field type of the attribute.
     *
     * Note that the type returned is a 'generic' type. Each database
     * vendor might have his own types, therefor, the type should be
     * converted to a database specific type using $db->fieldType().
     *
     * If the type was read from the table metadata, that value will
     * be used. Else, the attribute will analyze its flags to guess
     * what type it should be. If self::AF_AUTO_INCREMENT is set, the field
     * is probaly "number". If not, it's probably "string".
     *
     * @return string The 'generic' type of the database field for this
     *                attribute.
     */
    public function dbFieldType()
    {
        return '';
    }
}
