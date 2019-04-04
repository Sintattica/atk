<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\State;
use Sintattica\Atk\Core\Node;
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
        $selector = $this->m_node->primaryKeyFromString($this->m_node->m_postvars['atkselector'] ?? '');
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
     * @param array $field
     * @param string $mode
     *
     * @return string Html
     */
    public function getSectionControl($field, $mode)
    {
        // label
        $label = self::getSectionLabel($this->m_node, $field['name']);

        // our name
        list($tab, $section) = explode('.', $field['name']);
        $name = "section_{$tab}_{$section}";

        $url = Tools::partial_url($this->m_node->atkNodeUri(), $mode, 'sectionstate', array('atksectionname' => $name));

        // create onclick statement.
        $onClick = " onClick=\"javascript:ATK.Tabs.handleSectionToggle(this,null,'{$url}'); return false;\"";
        $initClass = 'openedSection';

        //if the section is not active, we close it on load.
        $default = in_array($field['name'], $this->m_node->getActiveSections($tab, $mode)) ? 'opened' : 'closed';
        $sectionstate = State::get(array('nodetype' => $this->m_node->atkNodeUri(), 'section' => $name), $default);

        if ($sectionstate == 'closed') {
            $initClass = 'closedSection';
            $page = $this->getPage();
            $page->register_scriptcode("ATK.Tabs.addClosedSection('$name');");
        }

        // create the clickable link
        return '<span class="atksectionwr"><a href="javascript:void(0)" id="'.$name.'" class="atksection '.$initClass.'"'.$onClick.'>'.$label.'</a></span>';
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
            $tabTpl['section_'.$t] = $node->getTemplate($mode, $record, $t);
        }

        return $tabTpl;
    }

    /**
     * Render tabs using templates.
     *
     * @todo this method seems broken by design, read comments for more info!
     *
     * @param array $fields
     * @param array $tabTpl
     *
     * @return array with already rendering tabs
     */
    public function _renderTabs($fields, $tabTpl)
    {
        $ui = $this->getUi();
        $tabs = [];
        $perTpl = []; //per template array

        for ($i = 0, $_i = Tools::count($fields); $i < $_i; ++$i) {
            $allTabs = explode(' ',
                $fields[$i]['tab']); // should not use "tab" here, because it actually contains the CSS class names and not only the tab names
            $allMatchingTabs = array_values(array_intersect($allTabs,
                array_keys($tabTpl))); // because of the CSS thingee above we search for the first matching tab
            if (Tools::count($allMatchingTabs) == 0) {
                $allMatchingTabs = array_keys($tabTpl);
            } // again a workaround for this horribly broken method
            $tab = $allMatchingTabs[0]; // attributes can be part of one, more than one or all tabs, at the moment it seems only one or all are supported
            $perTpl[$tabTpl[$tab]]['fields'][] = $fields[$i]; //make field available in numeric array
            $perTpl[$tabTpl[$tab]][isset($fields[$i]['attribute'])?$fields[$i]['attribute']:null] = $fields[$i]; //make field available in associative array
            $perTpl[$tabTpl[$tab]]['attributes'][isset($fields[$i]['attribute'])?$fields[$i]['attribute']:null] = $fields[$i]; //make field available in associative array
        }

        // Add 'alltab' fields to all templates
        foreach ($fields as $field) {
            if (in_array('alltabs', explode(' ', $field['tab']))) {
                $templates = array_keys($perTpl);
                foreach ($templates as $tpl) {
                    if (!$perTpl[$tpl][$field['attribute']]) {
                        $perTpl[$tpl]['fields'][] = $field;
                        $perTpl[$tpl][$field['attribute']] = $field;
                    }
                }
            }
        }

        $tpls = array_unique(array_values($tabTpl));
        foreach ($tpls as $tpl) {
            $tabs[] = $ui->render($tpl, $perTpl[$tpl]);
        }

        return $tabs;
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
