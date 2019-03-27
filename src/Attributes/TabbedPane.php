<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Handlers\ViewEditBase;

/**
 * TabbedPane place regular attribute to the additional tabbed pane.
 *
 * @author Yury Golovnya <ygolovnya@gmail.com>
 */
class TabbedPane extends Attribute
{
    const AF_TABBEDPANE_NO_AUTO_HIDE_LABEL = 33554432;

    /**
     * The tabs list
     * @var array
     * @access private
     */
    public $m_tabsList = [];

    /**
     * The list of "attribute"=>"tab
     * @var array
     * @access private
     */
    public $m_attribsList = [];

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_UNSUPPORTED;

    private $m_defaultTab = '';

    /**
     * Constructor.
     *
     * @param string $name The name of the attribute
     * @param int $flags The flags for this attribute
     * @param array $tabs The arrays looks like array("tabname1"=>("attribname1,"attribname2),"tabname1"=>(..),..)
     */
    public function __construct($name, $flags = 0, $tabs = [])
    {
        // A TabbedPane attribute should be display only in edit/view mode
        $flags = $flags | self::AF_HIDE_SEARCH | self::AF_HIDE_LIST | self::AF_HIDE_SELECT;
        parent::__construct($name, $flags);

        $this->setStorageType(self::POSTSTORE);
        $this->setLoadType(self::NOLOAD);

        foreach ($tabs as $tab => $attribs) {
            foreach ($attribs as $attrib) {
                $this->add($attrib, $tab);
            }
        }
    }

    /**
     * Add attribute to tabbedpane.
     *
     * @param string $attrib The name of the attribute
     * @param string $tab The name of tab. If empty, attribute name used
     */
    public function add($attrib, $tab = '')
    {
        if (empty($tab)) {
            $tab = $attrib;
        }

        if (!in_array($tab, $this->m_tabsList)) {
            $this->m_tabsList[] = $tab;
        }
        $this->m_attribsList[$attrib] = $tab;
        if (is_object($this->m_ownerInstance)) {
            $p_attr = $this->m_ownerInstance->getAttribute($attrib);
            if (is_object($p_attr)) {
                $p_attr->addDisabledMode(self::DISABLED_VIEW | self::DISABLED_EDIT);
                $p_attr->setTabs($this->getTabs());
                $p_attr->setSections($this->getSections());
            }
        }
    }

    /**
     * Return list of all tabs, having in this attribute.
     *
     * @param string $action An action name. Don't use now
     *
     * @return array $tab The array name of tab.
     */
    public function getPaneTabs($action)
    {
        return $this->m_tabsList;
    }

    /**
     * Return default tab, now simply first tab.
     *
     * @return string The default tab name.
     */
    public function getDefaultTab()
    {
        global $ATK_VARS;

        //check in the postvars (when we are back from a nested node, eg: OneToMany)
        $postvars = $this->getOwnerInstance()->m_postvars;
        if (array_key_exists($this->fieldName(), $postvars) &&
            in_array($postvars[$this->fieldName()], $this->m_tabsList)
        ) {
            return $postvars[$this->fieldName()];
        }

        // check in the atktabbedpane (when we are back from a "save" action)
        $key = str_replace('.', '_', 'atktabbedpane_'.$this->getOwnerInstance()->atkNodeUri().'_'.$this->fieldName());
        if (isset($ATK_VARS[$key]) && in_array($ATK_VARS[$key], $this->m_tabsList)) {
            return $ATK_VARS[$key];
        }

        //return default Tab
        return in_array($this->m_defaultTab, $this->m_tabsList) ? $this->m_defaultTab : $this->m_tabsList[0];
    }

    public function setDefaultTab($tab)
    {
        $this->m_defaultTab = $tab;
    }

    public function store($db, $record, $mode)
    {
        global $g_stickyurl;
        $key = str_replace('.', '_', 'atktabbedpane_'.$this->getOwnerInstance()->atkNodeUri().'_'.$this->fieldName());
        $value = Tools::atkArrayNvl($record, $this->fieldName());
        if (!is_array($g_stickyurl)) {
            $g_stickyurl = [];
        }
        if (!array_key_exists($key, $g_stickyurl)) {
            $g_stickyurl[] = $key;
        }
        $GLOBALS[$key] = $value;

        return true;
    }

    /**
     * Post init function.
     */
    public function postInit()
    {
        foreach (array_keys($this->m_attribsList) as $attrib) {
            $p_attr = $this->m_ownerInstance->getAttribute($attrib);
            $p_attr->addDisabledMode(self::DISABLED_VIEW | self::DISABLED_EDIT);
            $p_attr->setTabs($this->getTabs());
            $p_attr->setSections($this->getSections());
        }
    }

    /**
     * Check if attribute is single on the tab.
     *
     * @param string $name The name of attribute
     *
     * @return bool True if single.
     *              $todo Take into accout self::AF_HIDE_VIEW,self::AF_HIDE_EDIT flag of attribute -
     *              attribute can be placed on tab, but only in edit action - 2 attribute when edit and 1  -if view
     */
    public function isAttributeSingleOnTab($name)
    {
        $result = false;

        if (!$this->hasFlag(self::AF_TABBEDPANE_NO_AUTO_HIDE_LABEL)) {
            $tab = $this->m_attribsList[$name];
            $friquency = array_count_values(array_values($this->m_attribsList));
            $result = ($friquency[$tab] == 1);
        }

        return $result;
    }

    /**
     * Adds the attribute's edit / hide HTML code to the edit array.
     *
     * @param string $mode the edit mode ("add" or "edit")
     * @param array $arr pointer to the edit array
     * @param array $defaults pointer to the default values array
     * @param array $error pointer to the error array
     * @param string $fieldprefix the fieldprefix
     *
     * @return array fields
     */
    public function _addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
        $node = $this->m_ownerInstance;
        $fields = [];

        //collecting output from attributes
        foreach ($this->m_attribsList as $name => $tab) {
            $p_attrib = $node->getAttribute($name);
            if (is_object($p_attrib)) {
                /* hide - nothing to do with tabbedpane, must be render on higher level */
                if (($mode == 'edit' && $p_attrib->hasFlag(self::AF_HIDE_EDIT)) || ($mode == 'add' && $p_attrib->hasFlag(self::AF_HIDE_ADD))) {
                    /* when adding, there's nothing to hide... */
                    if ($mode == 'edit' || ($mode == 'add' && !$p_attrib->isEmpty($defaults))) {
                        $arr['hide'][] = $p_attrib->hide($defaults, $fieldprefix, $mode);
                    }
                } /* edit */ else {
                    $entry = [
                        'name' => $p_attrib->m_name,
                        'obligatory' => $p_attrib->hasFlag(self::AF_OBLIGATORY),
                        'attribute' => &$p_attrib,
                    ];
                    $entry['id'] = $p_attrib->getHtmlId($fieldprefix);


                    /* label? */
                    $entry['label'] = $p_attrib->getLabel($defaults, $mode);
                    /* error? */
                    $entry['error'] = $p_attrib->getError($error);
                    // on which tab? - from tabbedpane properties
                    $entry['tabs'] = $tab;
                    /* the actual edit contents */
                    $entry['html'] = $p_attrib->getEdit($mode, $defaults, $fieldprefix);
                    $fields['fields'][] = $entry;
                }
            } else {
                Tools::atkerror("Attribute $name not found!");
            }
        }
        /* check for errors */
        $fields['error'] = $defaults['atkerror'];

        return $fields;
    }

    public function _addToViewArray($mode, &$arr, &$defaults, &$error)
    {
        $node = $this->m_ownerInstance;
        $fields = [];

        //collecting output from attributes
        foreach ($this->m_attribsList as $name => $tab) {
            $p_attrib = $node->getAttribute($name);
            if (is_object($p_attrib)) {
                /* hide - nothing to do with tabbedpane, must be render on higher level */
                if ($mode == 'view' && $p_attrib->hasFlag(self::AF_HIDE_VIEW)) {
                    $arr['hide'][] = $p_attrib->hide($defaults, '', $mode);
                } /* view */ else {

                        $entry = array(
                            'name' => $p_attrib->m_name,
                            'obligatory' => $p_attrib->hasFlag(self::AF_OBLIGATORY),
                            'attribute' => $p_attrib);

                        $entry['id'] = $p_attrib->getHtmlId('');

                        /* label? */
                        $entry['label'] = $p_attrib->getLabel($defaults, $mode);
                        // on which tab? - from tabbedpane properties
                        $entry['tabs'] = $tab;
                        //on which sections?
                        $entry['sections'] = $p_attrib->getSections();
                        /* the actual edit contents */
                        $entry['html'] = $p_attrib->getView($mode, $defaults);
                        $arr['fields'][] = $entry;

                        $fields['fields'][] = $entry;
                }
            } else {
                Tools::atkerror("Attribute $name not found!");
            }
        }
        /* check for errors */
        $fields['error'] = $defaults['atkerror'];

        return $fields;
    }


    public function edit($record, $fieldprefix, $mode)
    {
        $node = $this->m_ownerInstance;
        $arr = ['hide' => []];
        //get data
        $data = $this->_addToEditArray($mode, $arr, $record, $record['atkerror'], $fieldprefix);

        // Handle fields
        // load images
        $reqimg = "<span class='required'></span>";

        /* display the edit fields */
        $fields = [];
        $tab = $this->getDefaultTab();

        for ($i = 0, $_i = Tools::count($data['fields']); $i < $_i; ++$i) {
            $field = &$data['fields'][$i];
            $tplfield = [];

            $tplfield['tab'] = $field['tabs'];

            $tplfield['initial_on_tab'] = $tplfield['tab'] == $tab;

            $tplfield['class'] = "tabbedPaneAttr tabbedPaneTab{$field['tabs']}";

            /** @var Attribute $attr */
            $attr = $field['attribute'];

            // Check if there are attributes initially hidden on this tabbedpane
            if ($attr->isInitialHidden()) {
                $tplfield['class'] .= ' atkAttrRowHidden';
            }

            $tplfield['rowid'] = 'tabbedPaneAttr_'.($field['id'] != '' ? $field['id'] : Tools::getUniqueId(
                    'anonymousattribrows'
                )); // The id of the containing row

            /* check for separator */
            if ($field['html'] == '-' && $i > 0 && $data['fields'][$i - 1]['html'] != '-') {
                $tplfield['line'] = '<hr>';
            } /* double separator, ignore */ elseif ($field['html'] == '-') {
            } /* only full HTML */ elseif (isset($field['line'])) {
                $tplfield['line'] = $field['line'];
            } /* edit field */ else {
                if ($field['attribute']->m_ownerInstance->getNumbering()) {
                    ViewEditBase::_addNumbering($field, $tplfield, $i);
                }

                /* does the field have a label? */
                if ((isset($field['label']) && $field['label'] !== 'AF_NO_LABEL') && !$this->isAttributeSingleOnTab(
                        $field['name']
                    ) || !isset($field['label'])) {
                    if ($field['label'] == '') {
                        $tplfield['label'] = '';
                    } else {
                        $tplfield['label'] = $field['label'];
                        if ($field['error']) { // TODO KEES
                            $tplfield['error'] = $field['error'];
                        }
                    }
                } else {
                    $tplfield['label'] = 'AF_NO_LABEL';
                }

                /* obligatory indicator */
                if ($field['obligatory']) {
                    $tplfield['obligatory'] = $reqimg;
                }

                /* html source */
                $tplfield['widget'] = $field['html'];
                $editsrc = $field['html'];

                $tplfield['htmlid'] = $field['id'];
                $tplfield['id'] = str_replace('.', '_', $node->atkNodeUri().'_'.$field['id']);
                $tplfield['full'] = $editsrc;
            }
            $fields[] = $tplfield; // make field available in numeric array
            $params[$field['name']] = $tplfield; // make field available in associative array
        }

        $ui = $node->getUi();

        $result = '';

        foreach ($arr['hide'] as $hidden) {
            $result .= $hidden;
        }

        $params['activeTab'] = $tab;
        $params['panename'] = $this->m_name;
        $params['fields'] = $fields; // add all fields as an numeric array.

        $result .= $ui->render('tabbededitform.tpl', $params);

        $content = $this->tabulate($mode, $result, $fieldprefix, $tab);

        return $content;
    }


    public function display($record, $mode)
    {
        $node = $this->m_ownerInstance;
        $arr = ['hide' => []];
        //get data
        $data = $this->_addToViewArray($mode, $arr, $record, $record['atkerror']);

        // Handle fields
        // load images
        $reqimg = "<span class='required'></span>";

        /* display the edit fields */
        $fields = [];
        $tab = $this->getDefaultTab();

        for ($i = 0, $_i = Tools::count($data['fields']); $i < $_i; ++$i) {
            $field = &$data['fields'][$i];
            $tplfield = [];

            $tplfield['tab'] = $field['tabs'];

            $tplfield['initial_on_tab'] = $tplfield['tab'] == $tab;

            $tplfield['class'] = "tabbedPaneAttr tabbedPaneTab{$field['tabs']}";

            /** @var Attribute $attr */
            $attr = $field['attribute'];

            // Check if there are attributes initially hidden on this tabbedpane
            if ($attr->isInitialHidden()) {
                $tplfield['class'] .= ' atkAttrRowHidden';
            }

            $tplfield['rowid'] = 'tabbedPaneAttr_'.($field['id'] != '' ? $field['id'] : Tools::getUniqueId(
                    'anonymousattribrows'
                )); // The id of the containing row

            /* check for separator */
            if ($field['html'] == '-' && $i > 0 && $data['fields'][$i - 1]['html'] != '-') {
                $tplfield['line'] = '<hr>';
            } /* double separator, ignore */ elseif ($field['html'] == '-') {
            } /* only full HTML */ elseif (isset($field['line'])) {
                $tplfield['line'] = $field['line'];
            } /* edit field */ else {
                if ($field['attribute']->m_ownerInstance->getNumbering()) {
                    ViewEditBase::_addNumbering($field, $tplfield, $i);
                }

                /* does the field have a label? */
                if ((isset($field['label']) && $field['label'] !== 'AF_NO_LABEL') && !$this->isAttributeSingleOnTab(
                        $field['name']
                    ) || !isset($field['label'])) {
                    if ($field['label'] == '') {
                        $tplfield['label'] = '';
                    } else {
                        $tplfield['label'] = $field['label'];
                        if (isset($field['error']) && $field['error']) {
                            $tplfield['error'] = $field['error'];
                        }
                    }
                } else {
                    $tplfield['label'] = 'AF_NO_LABEL';
                }

                /* obligatory indicator */
                if ($field['obligatory']) {
                    $tplfield['obligatory'] = $reqimg;
                }

                /* html source */
                $tplfield['widget'] = $field['html'];
                $editsrc = $field['html'];

                $tplfield['htmlid'] = $field['id'];
                $tplfield['id'] = str_replace('.', '_', $node->atkNodeUri().'_'.$field['id']);
                $tplfield['full'] = $editsrc;
            }
            $fields[] = $tplfield; // make field available in numeric array
            $params[$field['name']] = $tplfield; // make field available in associative array
        }

        $ui = $node->getUi();

        $result = '';

        foreach ($arr['hide'] as $hidden) {
            $result .= $hidden;
        }

        $params['activeTab'] = $tab;
        $params['panename'] = $this->m_name;
        $params['fields'] = $fields; // add all fields as an numeric array.

        $result .= $ui->render('tabbedviewform.tpl', $params);

        $content = $this->tabulate($mode, $result, '', $tab);

        return $content;
    }

    /**
     * Tabulate.
     *
     * @param string $action
     * @param string $content
     * @param string $fieldprefix
     * @param string $activeTab
     *
     * @return string The HTML content
     */
    public function tabulate($action, $content, $fieldprefix = '', $activeTab)
    {
        $activeTabName = 'tabbedPaneTab'.$activeTab;
        $list = $this->getPaneTabs($action);
        if (Tools::count($list) > 0) {
            $node = $this->m_ownerInstance;

            $page = $node->getPage();
            $page->register_script(Config::getGlobal('assets_url').'javascript/tabbedpane.js');
            $page->register_loadscript("ATK.TabbedPane.showTab('tabbedPane{$fieldprefix}{$this->m_name}', '$activeTabName');");
            $ui = $node->getUi();

            $content = $ui->renderBox(
                [
                    'tabs' => $this->buildTabs($action, $fieldprefix, $activeTab),
                    'paneName' => "tabbedPane{$fieldprefix}{$this->m_name}",
                    'activeTabName' => $activeTabName,
                    'content' => $content,
                    'fieldName' => $this->fieldName(),
                ],
                'panetabs'
            );
        }

        return $content;
    }

    /**
     * Builds a list of tabs.
     *
     * This doesn't generate the actual HTML code, but returns the data for
     * the tabs (title, selected, urls that should be loaded upon click of the
     * tab etc).
     *
     * @param string $action The action for which the tabs should be generated.
     * @param string $fieldprefix The fieldprefix
     * @param string $activeTab
     *
     * @return array List of tabs
     *
     * @todo Make translation of tabs module aware
     */
    public function buildTabs($action = '', $fieldprefix = '', $activeTab)
    {
        $node = $this->m_ownerInstance;
        $result = [];

        foreach ($this->m_attribsList as $attrib => $tab) {
            $newtab = [];
            $newtab['title'] = Tools::atktext(["tab_$tab", $tab], $node->m_module, $node->m_type);
            $newtab['attribute'] = $attrib;
            $newtab['selected'] = ($activeTab == $tab);
            $result["tabbedPaneTab{$tab}"] = $newtab;
        }

        return $result;
    }

    /**
     * No function, but is necessary.
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        return '';
    }
}
