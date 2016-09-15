<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Handlers\ViewEditBase;

/**
 * TabbedPane place regular attribute to the additional tabbed pane.
 *
 * @author Yury Golovnya <ygolovnya@gmail.com>
 */
class TabbedPane extends Attribute
{
    const AF_TABBEDPANE_NO_AUTO_HIDE_LABEL = 33554432;

    /*
     * The tabs list
     * @var array
     * @access private
     */
    public $m_tabsList = [];

    /*
     * The list of "attribute"=>"tab
     * @var array
     * @access private
     */
    public $m_attribsList = [];

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

        foreach ($tabs as $tab => $attribs) {
            foreach ($attribs as $attrib) {
                $this->add($attrib, $tab);
            }
        }

        parent::__construct($name, $flags);
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
        //return first tab
        return $this->m_tabsList[0];
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
                    $entry = array(
                        'name' => $p_attrib->m_name,
                        'obligatory' => $p_attrib->hasFlag(self::AF_OBLIGATORY),
                        'attribute' => &$p_attrib,
                    );
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

    public function edit($record, $fieldprefix, $mode)
    {
        $node = $this->m_ownerInstance;
        $arr = array('hide' => array());
        //get data
        $data = $this->_addToEditArray($mode, $arr, $record, $record['atkerror'], $fieldprefix);

        // Handle fields
        // load images
        $reqimg = "<span class='required'></span>";

        /* display the edit fields */
        $fields = [];
        $tab = $this->getDefaultTab();

        for ($i = 0, $_i = count($data['fields']); $i < $_i; ++$i) {
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

            $tplfield['rowid'] = 'tabbedPaneAttr_'.($field['id'] != '' ? $field['id'] : Tools::getUniqueId('anonymousattribrows')); // The id of the containing row

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
                if ((isset($field['label']) && $field['label'] !== 'AF_NO_LABEL') && !$this->isAttributeSingleOnTab($field['name']) || !isset($field['label'])) {
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

        $content = $this->tabulate($mode, $result, $fieldprefix);

        return $content;
    }

    /**
     * Display a tabbed pane with attributes.
     *
     * @param array $record Array with fields
     * @param string $mode The mode
     *
     * @return string html code
     */
    public function display($record, $mode)
    {
        // get active tab
        $active_tab = $this->getDefaultTab();
        $fields = [];
        $tab = '';

        $node = $this->m_ownerInstance;
        $ui = $node->getUi();

        // For all attributes we use the display() function to display the
        // attributes current value. This may be overridden by supplying
        // an <attributename>_display function in the derived classes.
        foreach ($this->m_attribsList as $name => $tab) {
            $p_attrib = $node->getAttribute($name);
            if (is_object($p_attrib)) {
                $tplfield = [];
                if (!$p_attrib->hasFlag(self::AF_HIDE_VIEW)) {
                    $fieldtab = $this->m_attribsList[$name];

                    $tplfield['class'] = "tabbedPaneAttr tabbedPaneTab{$fieldtab}";
                    $tplfield['rowid'] = 'tabbedPaneAttr_'.Tools::getUniqueId('anonymousattribrows'); // The id of the containing row
                    $tplfield['tab'] = $tplfield['class']; // for backwards compatibility

                    $tplfield['initial_on_tab'] = ($fieldtab == $active_tab);

                    // An <attributename>_display function may be provided in a derived
                    // class to display an attribute. If it exists we will use that method
                    // else we will just use the attribute's display method.
                    $funcname = $p_attrib->m_name.'_display';
                    if (method_exists($node, $funcname)) {
                        $editsrc = $node->$funcname($record, 'view');
                    } else {
                        $editsrc = $p_attrib->display($record, 'view');
                    }

                    $tplfield['full'] = $editsrc;
                    $tplfield['widget'] = $editsrc; // in view mode, widget and full are equal
                    // The Label of the attribute (can be suppressed with self::AF_NOLABEL or self::AF_BLANKLABEL)
                    // For each attribute, a txt_<attributename> must be provided in the language files.
                    if (!$p_attrib->hasFlag(self::AF_NOLABEL) && !$this->isAttributeSingleOnTab($name)) {
                        if ($p_attrib->hasFlag(self::AF_BLANKLABEL)) {
                            $tplfield['label'] = '';
                        } else {
                            $tplfield['label'] = $p_attrib->label();
                        }
                    } else {
                        // Make the rest fill up the entire line
                        $tplfield['label'] = '';
                        $tplfield['line'] = $tplfield['full'];
                    }
                    $fields[] = $tplfield;
                }
            } else {
                Tools::atkerror("Attribute $name not found!");
            }
        }
        $innerform = $ui->render($node->getTemplate('view', $record, $tab), array('fields' => $fields));

        return $this->tabulate('view', $innerform);
    }

    /**
     * Tabulate.
     *
     * @param string $action
     * @param string $content
     * @param string $fieldprefix
     *
     * @return string The HTML content
     */
    public function tabulate($action, $content, $fieldprefix = '')
    {
        $activeTabName = 'tabbedPaneTab'.$this->getDefaultTab();
        $list = $this->getPaneTabs($action);
        if (count($list) > 0) {
            $node = $this->m_ownerInstance;

            $page = $node->getPage();
            $page->register_script(Config::getGlobal('assets_url').'javascript/class.atktabbedpane.js');
            $page->register_loadscript("ATK.TabbedPane.showTab('tabbedPane{$fieldprefix}{$this->m_name}', '$activeTabName');");

            $ui = $node->getUi();
            if (is_object($ui)) {
                $content = $ui->renderBox(array(
                    'tabs' => $this->buildTabs($action, $fieldprefix),
                    'paneName' => "tabbedPane{$fieldprefix}{$this->m_name}",
                    'activeTabName' => $activeTabName,
                    'content' => $content,
                ), 'panetabs');
            }
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
     *
     * @return array List of tabs
     *
     * @todo Make translation of tabs module aware
     */
    public function buildTabs($action = '', $fieldprefix = '')
    {
        $node = $this->m_ownerInstance;
        $result = [];

        // which tab is currently selected
        $active_tab = $this->getDefaultTab();

        foreach ($this->m_attribsList as $attrib => $tab) {
            $newtab = [];
            $newtab['title'] = Tools::atktext(array("tab_$tab", $tab), $node->m_module, $node->m_type);
            $newtab['attribute'] = $attrib;
            $newtab['selected'] = ($active_tab == $tab);
            $result["tabbedPaneTab{$tab}"] = $newtab;
        }

        return $result;
    }

    /**
     * No function, but is neccesary.
     *
     * @param array $record
     *
     * @return null
     */
    public function db2value($record)
    {
        return null;
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

    /**
     * Return the database field type of the attribute.
     *
     * @return string empty string
     */
    public function dbFieldType()
    {
        return '';
    }

    /**
     * Determine the load type of this attribute.
     *
     * @param string $mode The type of load (view,admin,edit etc)
     *
     * @return int NOLOAD     - nor load(), nor addtoquery() should be
     *             called (attribute can not be loaded from the
     *             database)
     */
    public function loadType($mode)
    {
        return self::NOLOAD;
    }

    /**
     * Determine the storage type of this attribute.
     *
     * @param string $mode The type of storage ("add" or "update")
     *
     * @return int NOSTORE    - nor store(), nor addtoquery() should be
     *             called (attribute can not be stored in the
     *             database)
     */
    public function storageType($mode = null)
    {
        return self::NOSTORE;
    }
}
