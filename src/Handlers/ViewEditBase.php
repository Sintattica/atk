<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\State;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Session\SessionStore;

/**
 * Handler class for the edit action of a node. The handler draws a
 * generic edit form for the given node.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
class ViewEditBase extends ActionHandler
{
    /**
     * Holds the record for the node. It is cached as long as the instance
     * exists, unless we force a reload.
     *
     * @var array
     */
    private $m_record = null;

    /**
     * Does the current node has sections ?
     *
     * @var bool
     */
    private $m_hasSections = false;

    /**
     * Active tab (fetched from session by default, but fixed if needed)
     *
     * @var string
     */
    private $m_activeTab = null;

    /**
     * Get the record to view/edit. It is cached as long as the instance
     * exists, unless we force a reload.
     *
     * @param bool $force Whether or not to force the fetching of the record
     *
     * @return array The record for viewing/editting
     */
    public function getRecord($force = false)
    {
        // if we are not forcing a fetch and we already have a cached record, return it
        if ($force === false && $this->m_record !== null) {
            return $this->m_record;
        }

        $record = $this->getRejectInfo(); // Check reject info first

        if ($record == null) { // If reject info not set -  do select
            $atkstoretype = '';
            $sessionmanager = SessionManager::getInstance();
            if ($sessionmanager) {
                $atkstoretype = $sessionmanager->stackVar('atkstore');
            }
            switch ($atkstoretype) {
                case 'session':
                    $record = $this->getRecordFromSession();
                    break;
                default:
                    $record = $this->getRecordFromDb();
                    break;
            }
        }

        // cache the record
        $this->m_record = $record;

        return $record;
    }

    /**
     * Get the record for the database with the current selector.
     *
     * @return array
     */
    protected function getRecordFromDb()
    {
        $selector = Tools::atkArrayNvl($this->m_node->m_postvars, 'atkselector', '');
        $record = $this->m_node->select($selector)->mode('edit')->getFirstRow();

        return $record;
    }

    /**
     * Get the current record from the database with the current selector.
     *
     * @return array
     */
    protected function getRecordFromSession()
    {
        $selector = Tools::atkArrayNvl($this->m_node->m_postvars, 'atkselector', '');

        return SessionStore::getInstance()->getDataRowForSelector($selector);
    }

    /**
     * Get section label.
     *
     * @param Node $node
     * @param string $rawName
     *
     * @return string label
     *
     * @static
     */
    public function getSectionLabel($node, $rawName)
    {
        list($tab, $section) = explode('.', $rawName);
        $strings = array("section_{$tab}_{$section}", "{$tab}_{$section}", "section_{$section}", $section);

        return $node->text($strings);
    }

    /**
     * Get tab label.
     *
     * @param Node $node
     * @param string $tab
     *
     * @return string label
     *
     * @static
     */
    public function getTabLabel($node, $tab)
    {
        $strings = array("tab_{$tab}", $tab);

        return $node->text($strings);
    }

    /**
     * Create the clickable label for the section.
     *
     * @param string $section name
     * @param string $mode
     * @param bool $sectionIsOpen initially
     *
     * @return string Html
     */
    public function getSectionControl($section, $mode, $sectionIsOpen = true)
    {
        // label
        $label = self::getSectionLabel($this->m_node, $section);

        // our name
        $name = "section_".str_replace('.', '_', $section);

        $url = Tools::partial_url($this->m_node->atkNodeUri(), $mode, 'sectionstate', array('atksectionname' => $section));

        // create onclick statement.
        $onClick = " onClick=\"javascript:ATK.Tabs.handleSectionToggle(this,null,'{$url}'); return false;\"";
        if ($sectionIsOpen) {
            $initIcon = 'icon_minussquare';
        } else {
            $initIcon = 'icon_plussquare';
            //if the section is not active, we close it on load.
            $page = $this->getPage();
            $page->register_scriptcode("ATK.Tabs.addClosedSection('$name');");
        }

        // create the clickable link
        return '<span class="atksectionwr"><a href="javascript:void(0)" id="'.$name.'" class="atksection"'.$onClick.'><i class="'.Config::getGlobal($initIcon).'" id="img_'.$name.'"></i> '.$label.'</a></span>';
    }

    /**
     * Based on the attributes that are part of this section we
     * check if this section should initially be shown or not.
     *
     * @param string $section section name
     * @param array $fields edit fields
     *
     * @return bool
     */
    public function isSectionInitialHidden($section, $fields)
    {
        foreach ($fields as $field) {
            if (is_array($field['sections']) && in_array($section, $field['sections']) && (!isset($field['initial_hidden']) || !$field['initial_hidden'])) {
                return false;
            }
        }

        return true;
    }

    /**
     * Adds numbering to the label of a field.
     *
     * @param array $field the currently handled attribute
     * @param array $tplfield the template data for the current attribute
     * @param int $i the counter being used to loop the node for each attribute
     */
    public static function _addNumbering(&$field, &$tplfield, &$i)
    {
        static $number, $subnumber;

        if (!$number && !$subnumber) {
            $number = $field['attribute']->m_ownerInstance->getNumbering();
        }
        if (!$subnumber) {
            if (strlen($number) == 1 || (floor($number) <= 9 && floor($number) >= -9 && floor($number) == $number)) {
                $subnumber = $number;
                $number = null;
            } else {
                $subnumber = substr($number, strrpos($number, '.') + 1);
                $number = substr($number, 0, strrpos($number, '.'));
            }
        }

        if ($field['label']) {
            if ($number) {
                $tplfield['label'] = "$number.$subnumber. ";
            } else {
                $tplfield['label'] = "$subnumber. ";
            }
            ++$subnumber;
        }
    }

    /**
     * Section state handler.
     */
    public function partial_sectionstate()
    {
        State::set(array(
            'nodetype' => $this->m_node->atkNodeUri(),
            'section' => $this->m_postvars['atksectionname'],
        ), $this->m_postvars['atksectionstate']);
        die;
    }

    /**
     * Get the state of a given section ('opened' or 'closed')
     *
     * @param string $section name
     *
     * @return bool
     */
    public function sectionIsOpen($section) {
        $defaultState = $this->m_node->isSectionDefaultExpanded($section) ? 'opened' : 'closed';
        $key = ['nodetype' => $this->m_node->atkNodeUri(), 'section' => $section];
        $state = State::get($key, $defaultState);
        return $state == 'opened';
    }

    /**
     * Tab state handler.
     */
    public function partial_tabstate()
    {
        if (Config::getGlobal('dhtml_tabs_stateful')) {
            State::set($this->m_node->atkNodeUri().'_tab', $this->m_postvars['atktab']);
        }
        die;
    }

    /**
     * Returns the currently active tab.
     *
     * It tries to fetch one from Session. If not found or empty, it tries
     * the default tab. If default tab is empty, it returns the first tab
     * of the first field.
     * The value is cached in $this->m_activeTab.
     *
     * @param array $fields with section/tabs information
     * @param string $action 'add', 'view', ...
     *
     * @return string The name of the currently visible tab.
     */
    public function getActiveTab($fields, $action = '')
    {
        if (!is_null($this->m_activeTab)) {
            return $this->m_activeTab;
        }

        // Computing tab list from fields:
        $tabList = [];
        foreach ($fields as $field) {
            $tabs = is_null($field['section']) ? $field['tabs'] :
                [$this->m_node->getTabFromSection($field['section'])];
            $tabList = array_merge($tabList, $tabs);
        }
        $tabList = array_filter($tabList, function($tab) { return $tab != 'alltabs'; });

        $sessionTab = State::get($this->m_node->atkNodeUri().'_tab');
        $defaultTab = $this->m_node->resolveTab('');
        // Let's try tab from session :
        if (in_array($sessionTab, $tabList)) {
            $this->m_activeTab = $sessionTab;
        // Let's try default tab :
        } elseif (in_array($defaultTab, $tabList)) {
            $this->m_activeTab = $defaultTab;
        // Else, let's take the first tab from the first field
        } elseif (count($tabList)) {
            $this->m_activeTab = $tabList[0];
            return $this->m_activeTab;
        }
        return $this->m_activeTab;
    }

    /**
     * Get array with tab name as key and tab template as value.
     *
     * @param object $node
     * @param array $tabs
     * @param string $mode
     * @param array $record
     *
     * @return array with tab=>template pear
     */
    public function _getTabTpl($node, $tabs, $mode, $record)
    {
        $tabTpl = [];
        foreach ($tabs as $t) {
            $tabTpl[$t] = $node->getTemplate($mode, $record, $t);
        }
        $tabTpl['alltabs'] = $node->getTemplate($mode, $record, 'alltabs');

        return $tabTpl;
    }

    /**
     * Render fields with the template corresponding to the record/tab/mode we're in
     *
     * @param array $fields from self::fieldsWithTabsAndSections
     * @param array $tabTpl array [string $tab1 => string $template1, ...]
     *
     * @return array with rendered tabs
     */
    public function _renderTabs($fields, $tabTpl)
    {
        $ui = $this->getUi();
        $tabs = [];
        $perTpl = []; //array [template1 => ['fields' => [$field1, $field2, ...]], ...]

        foreach ($fields as $field) {
            // We render each field based on the first tab he's listed in :
            $tab = $field['tabs'][0];
            $template = $tabTpl[$tab];
            if (!isset($perTpl[$template])) {
                $perTpl[$template] = [];
            }
            $perTpl[$template]['fields'][] = $field;
        }

        foreach ($perTpl as $tpl => $fieldsArray) {
            $tabs[] = $ui->render($tpl, $fieldsArray);
        }

        return $tabs;
    }

    /**
     * Group fields by tabs and sections, add 'sections' controls and set
     * initial visibility of fields ('initial_on_tab')
     *
     * @param $fields array returned by node::editArray
     * @param $mode we're in (add or edit)
     *
     * @return $fields array with sections elements 'initial_on_tab' for each values
     */
    protected function fieldsWithTabsAndSections($fields, $mode = 'edit')
    {
        $activeTab = $this->getActiveTab($fields, $mode);
        // First, sort fields by section and set aside those who are not
        // within a section (i.e. in the common part of the tab)
        $fieldsBySection = [];
        $fieldsWithoutSection = [];
        foreach ($fields as $field) {
            $section = $field['section'];
            if ($section == null) {
                $field['initial_on_tab'] = in_array($activeTab, $field['tabs']) || in_array('alltabs', $field['tabs']);
                $fieldsWithoutSection[] = $field;
            } else {
                $section = $this->m_node->resolveSection($section);
                if (!isset($fieldsBySection[$section])) {
                    $fieldsBySection[$section] = [];
                }
                $fieldsBySection[$section][] = $field;
            }
        }

        // We retain the information 'is there at least one section' (for tabulate function, later).
        $this->m_hasSections = !empty($fieldsBySection);

        // Let's put the fields outside sections in the beginning of the result
        $result = $fieldsWithoutSection;
        // Then, put the fields inside sections :
        foreach ($fieldsBySection as $section => $fieldList) {
            // Add 'section' element to the fields list :
            $tab = $this->m_node->getTabFromSection($section);
            $currentTabActive = ($tab == $activeTab);
            $sectionIsOpen = $this->sectionIsOpen($section);
            $result[] = [
                'class' => 'section_'.$tab,
                'initial_on_tab' => $currentTabActive,
                'tabs' => [$tab],
                'line' => $this->getSectionControl($section, $mode, $sectionIsOpen),
            ];
            $initial_on_tab = $currentTabActive && $sectionIsOpen;
            // Add fields element belonging to the section :
            foreach ($fieldList as $field) {
                $field['initial_on_tab'] = $initial_on_tab;
                $result[] = $field;
            }
        }
        return $result;
    }

    /**
     * Get tabs from the fields list returned by fieldsWithTabsAndSections
     *
     * @param array of fields, each field having a 'tabs' attribute
     *
     * @return array of tabs (strings)
     */
    public function getTabs($fields)
    {
        $tabs = [];
        foreach ($fields as $field) {
            $tabs = Tools::atk_array_merge($tabs, $field['tabs']);
            if (isset($field['section']) and !is_null($field['section'])) {
                $sections[] = $section;
            }
        }
        $tabs = array_unique($tabs);
        // Exclude 'alltabs' which is not a real tab :
        return array_filter($tabs, function($tab) { return $tab != 'alltabs'; });
    }

    /**
     * Render tabs header with their links.
     *
     * @param string $action The action for which the tabs are loaded.
     * @param array $fields as returned by node->editArray of node->viewArray()
     *
     * @return string The complete tabset with content.
     */
    public function tabulate($action, $fields)
    {
        // Collect tabs from fields shown in the form :
        $tabs = $this->getTabs($fields);
        $tabs = $this->m_node->sortTabs($action, $tabs);
        $tabCount = Tools::count($tabs);

        if (!$this->m_hasSections && $tabCount == 1) {
            return '';
        }

        $page = $this->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/tabs.js');

        if ($tabCount == 1) {
            return '';
        }
        $ui = $this->getUi();
        if (!is_object($ui)) {
            return '';
        }

        // which tab is currently selected
        $activeTab = $this->getActiveTab($fields, $action);
        // Building $tabList object for ui::renderTabs function:
        $tabList = [];
        foreach ($tabs as $tab) {
            $tabList[] = [
                'title' => $this->m_node->text(array("tab_{$tab}", $tab)),
                'tab' => $tab,
                'selected' => ($tab == $activeTab)
            ];
        }
        return $ui->renderTabs([
            'tabs' => $tabList,
            'tabstateUrl' => Config::getGlobal('dispatcher').'?atknodeuri='.$this->m_node->atkNodeUri().'&atkaction='.$action.'&atkpartial=tabstate&atktab=',
        ]);
    }

    /**
     * Attribute handler.
     *
     * @param string $partial full partial
     */
    public function partial_attribute($partial)
    {
        list(, $attribute, $partial) = explode('.', $partial);

        $attr = $this->m_node->getAttribute($attribute);
        if ($attr == null) {
            Tools::atkerror("Unknown / invalid attribute '$attribute' for node '".$this->m_node->atkNodeUri()."'");

            return '';
        }

        return $attr->partial($partial, $this->m_action);
    }
}
