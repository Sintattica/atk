<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Utils\Json;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Session\SessionManager;

/**
 * Handler class for the edit action of a node. The handler draws a
 * generic edit form for the given node.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
class EditHandler extends ViewEditBase
{
    public $m_buttonsource = null;

    /**
     * Update action.
     *
     * @var string
     */
    private $m_updateAction = 'update';
    private $m_updateSessionStatus = SessionManager::SESSION_NESTED;

    /**
     * The action handler method.
     */
    public function action_edit()
    {
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        }

        $node = $this->m_node;

        $record = $this->getRecord();

        if ($record === null) {
            $location = $node->feedbackUrl('edit', self::ACTION_FAILED, $record);
            $node->redirect($location);
        }

        // allowed to edit record?
        if (!$this->allowed($record)) {
            $this->renderAccessDeniedPage();

            return;
        }

        $record = $this->mergeWithPostvars($record);

        $this->notify('edit', $record);
        $res = $this->invoke('editPage', $record);

        $page = $this->getPage();
        $page->addContent($node->renderActionPage('edit', $res));
    }

    /**
     * Returns the update action, which is called when posting the edit form.
     *
     * Defaults to the 'update' action.
     *
     * @return string update action
     */
    public function getUpdateAction()
    {
        return $this->m_updateAction;
    }

    /**
     * Sets the update action which should be called when posting the edit form.
     *
     * @param string $action action name
     */
    public function setUpdateAction($action)
    {
        $this->m_updateAction = $action;
    }

    /**
     * check if there are postvars set that overwrite the record contents, this can
     * happen when a new selection is made using the select handler etc.
     *
     * @param array $record The record
     *
     * @return array Record The merged record
     */
    public function mergeWithPostvars($record)
    {
        $fetchedRecord = $this->m_node->updateRecord('', null, null, true);

        /*
         * If any of the attributes is set to need a reload, we don't merge
         * with te postvars for that attribute
         */
        foreach ($fetchedRecord as $attrName => $value) {
            if ($attr = $this->m_node->getAttribute($attrName)) {
                if ($attr->needsReload()) {
                    unset($fetchedRecord[$attrName]);
                }
            }
        }

        if (is_array($record)) {
            $record = array_merge($record, $fetchedRecord);
        }

        return $record;
    }

    /**
     * Register external files.
     */
    public function registerExternalFiles()
    {

    }

    /**
     * Render the edit page.
     *
     * @param array $record The record to edit
     * @return string HTML code for the edit page
     */
    public function editPage($record)
    {
        $result = $this->getEditPage($record);

        if ($result !== false) {
            return $result;
        }

        return '';
    }

    /**
     * Get the params for the edit page.
     *
     * @param array $record The record to edit
     *
     * @return array Array with parameters
     */
    public function getEditParams($record)
    {
        $node = $this->m_node;

        $params = $node->getDefaultActionParams();
        $params['title'] = $node->actionTitle('edit', $record);
        $params['formstart'] = $this->getFormStart();
        $params['header'] = $this->invoke('editHeader', $record);
        $params['content'] = $this->getContent($record);
        $params['buttons'] = $this->getFormButtons($record);
        $params['formend'] = $this->getFormEnd();

        return $params;
    }

    /**
     * This method draws a generic edit-page for a given record.
     *
     * @param array $record The record to edit.
     *
     * @return string The rendered page as a string.
     */
    public function getEditPage($record)
    {
        $this->registerExternalFiles();

        $params = $this->getEditParams($record);

        if ($params === false) {
            return false;
        }

        return $this->renderEditPage($record, $params);
    }

    /**
     * Get the content.
     *
     * @param array $record
     *
     * @return string The content
     */
    public function getContent($record)
    {
        $node = $this->m_node;

        $forceList = [];
        if (isset($node->m_postvars['atkforce'])) {
            $forceList = json_decode($node->m_postvars['atkforce']);
        }

        $suppressList = [];
        if (isset($node->m_postvars['atksuppress'])) {
            $suppressList = $node->m_postvars['atksuppress'];
        }

        $form = $this->editForm('edit', $record, $forceList, $suppressList, $node->getEditFieldPrefix());

        return $node->tabulate('edit', $form);
    }

    /**
     * Render the edit page.
     *
     * @param array $record
     * @param array $params
     *
     * @return string The rendered edit page
     */
    public function renderEditPage($record, $params)
    {
        $node = $this->m_node;
        $ui = $node->getUi();

        if (is_object($ui)) {
            $this->getPage()->setTitle(Tools::atktext('app_shorttitle').' - '.$node->actionTitle('edit', $record));

            $output = $ui->render('action.tpl', $params, $node->m_module);
            $this->addRenderBoxVar('title', $node->actionTitle('edit', $record));
            $this->addRenderBoxVar('content', $output);

            $total = $ui->renderBox($this->m_renderBoxVars, $this->m_boxTemplate);

            return $total;
        }

        return '';
    }

    /**
     * Returns the current update session status.
     *
     * @see EditHandler::setUpdateSessionStatus
     *
     * @return int session status
     */
    public function getUpdateSessionStatus()
    {
        return $this->m_updateSessionStatus;
    }

    /**
     * Sets the session status in which the update action gets executed.
     * By default the update action is called nested in the session stack.
     *
     * @param int $sessionStatus session status (e.g. SessionManager::SESSION_NESTED, SessionManager::SESSION_DEFAULT etc.)
     */
    public function setUpdateSessionStatus($sessionStatus)
    {
        $this->m_updateSessionStatus = $sessionStatus;
    }

    /**
     * Get the start of the form.
     *
     * @return string HTML The forms' start
     */
    public function getFormStart()
    {
        $sm = SessionManager::getInstance();

        $formstart = '<form id="entryform" name="entryform" enctype="multipart/form-data" action="'.Config::getGlobal('dispatcher').'"'.' method="post" onsubmit="return ATK.globalSubmit(this,false)" class="form-horizontal" role="form" autocomplete="off">'.$sm->formState($this->getUpdateSessionStatus());

        $formstart .= '<input type="hidden" name="'.$this->getNode()->getEditFieldPrefix().'atkaction" value="'.$this->getUpdateAction().'" />';
        $formstart .= '<input type="hidden" name="'.$this->getNode()->getEditFieldPrefix().'atkprevaction" value="'.$this->getNode()->m_action.'" />';
        $formstart .= '<input type="hidden" name="'.$this->getNode()->getEditFieldPrefix().'atkcsrftoken" value="'.$this->getCSRFToken().'" />';
        $formstart .= '<input type="hidden" class="atksubmitaction" />';

        return $formstart;
    }

    /**
     * Get the end of the form.
     *
     * @return string HTML The forms' end
     */
    public function getFormEnd()
    {
        return '</form>';
    }

    /**
     * Get the buttons for the current action form.
     *
     * @param array $record
     *
     * @return array Array with buttons
     */
    public function getFormButtons($record = null)
    {
        // If no custom button source is given, get the default
        if ($this->m_buttonsource === null) {
            $this->m_buttonsource = $this->m_node;
        }

        return $this->m_buttonsource->getFormButtons('edit', $record);
    }

    /**
     * Create template field array for the given edit field.
     *
     * @param array $fields all fields
     * @param int $index field index
     * @param string $mode mode (add/edit)
     * @param string $tab active tab
     *
     * @return array template field
     */
    public function createTplField(&$fields, $index, $mode, $tab)
    {
        $field = &$fields[$index];

        // visible sections, both the active sections and the tab names (attribute that are
        // part of the anonymous section of the tab)
        $visibleSections = array_merge($this->m_node->getActiveSections($tab, $mode), $this->m_node->getTabs($mode));

        $tplfield = [];

        $classes = [];
        if(isset($field['class'])){
            $classes = is_array($field['class'])?$field['class']:explode(' ', $field['class']);
        }

        if ($field['sections'] == '*') {
            $classes[] = 'alltabs';
        } else {
            if (isset($field['html']) && $field['html'] == 'section') {
                // section should only have the tab section classes
                foreach ($field['tabs'] as $section) {
                    $classes[] = 'section_'.str_replace('.', '_', $section);
                }
                if ($this->isSectionInitialHidden($field['name'], $fields)) {
                    $classes[] = 'atkAttrRowHidden';
                }
            } else {
                if (is_array($field['sections'])) {
                    foreach ($field['sections'] as $section) {
                        $classes[] = 'section_'.str_replace('.', '_', $section);
                    }
                }
            }
        }

        if (isset($field['initial_hidden']) && $field['initial_hidden']) {
            $classes[] = 'atkAttrRowHidden';
        }

        $tplfield['class'] = implode(' ', $classes);
        $tplfield['tab'] = $tplfield['class']; // for backwards compatibility
        // Todo fixme: initial_on_tab kan er uit, als er gewoon bij het opstarten al 1 keer showTab aangeroepen wordt (is netter dan aparte initial_on_tab check)
        // maar, let op, die showTab kan pas worden aangeroepen aan het begin.
        $tplfield['initial_on_tab'] = ($field['tabs'] == '*' || in_array($tab,
                    $field['tabs'])) && (!is_array($field['sections']) || Tools::count(array_intersect($field['sections'], $visibleSections)) > 0);

        // ar_ stands for 'attribrow'.
        $tplfield['rowid'] = 'ar_'.(!empty($field['id']) ? $field['id'] : Tools::getUniqueId('anonymousattribrows')); // The id of the containing row
        // check for separator
        if (isset($field['html']) && $field['html'] == '-' && $index > 0 && $fields[$index - 1]['html'] != '-') {
            $tplfield['type'] = 'line';
            $tplfield['line'] = '<hr>';
        } /* double separator, ignore */ elseif (isset($field['html']) && $field['html'] == '-') {
        } /* sections */ elseif (isset($field['html']) && $field['html'] == 'section') {
            $tplfield['type'] = 'section';
            list($tab, $section) = explode('.', $field['name']);
            $tplfield['section_name'] = "section_{$tab}_{$section}";
            $tplfield['line'] = $this->getSectionControl($field, $mode);
        } /* only full HTML */ elseif (isset($field['line'])) {
            $tplfield['type'] = 'custom';
            $tplfield['line'] = $field['line'];
        } /* edit field */ else {
            $tplfield['type'] = 'attribute';

            if ($field['attribute']->m_ownerInstance->getNumbering()) {
                $this->_addNumbering($field, $tplfield, $index);
            }

            /* does the field have a label? */
            if ((isset($field['label']) && $field['label'] !== 'AF_NO_LABEL') || !isset($field['label'])) {
                if (!isset($field['label']) || empty($field['label'])) {
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
                $tplfield['obligatory'] = true;
            }

            // Make the attribute and node names available in the template.
            $tplfield['attribute'] = $field['attribute']->fieldName();
            $tplfield['node'] = $field['attribute']->m_ownerInstance->atkNodeUri();

            /* html source */
            $tplfield['widget'] = $field['html'];
            $editsrc = $field['html'];

            $tplfield['id'] = str_replace('.', '_', $this->m_node->atkNodeUri().'_'.$field['id']);
            $tplfield['htmlid'] = $field['id'];

            $tplfield['full'] = $editsrc;

            $column = $field['attribute']->getColumn();
            $tplfield['column'] = $column;

            $tplfield['readonly'] = $field['attribute']->isReadonlyEdit($mode);

            $tplfield['help'] = $field['attribute']->getHelp();
        }

        // allow passing of extra arbitrary data, for example if a user overloads the editArray method
        // to pass custom extra data per attribute to the template
        if (isset($field['extra'])) {
            $tplfield['extra'] = $field['extra'];
        }

        return $tplfield;
    }

    /**
     * Function returns a generic html form for editing a record.
     *
     * @param string $mode The edit mode ("add" or "edit").
     * @param array $record The record to edit.
     * @param array $forceList A key-value array used to preset certain
     *                             fields to a certain value.
     * @param array $suppressList An array of fields that will be hidden.
     * @param string $fieldprefix If set, each form element is prefixed with
     *                             the specified prefix (used in embedded
     *                             forms)
     * @param string $template The template to use for the edit form
     * @param bool $ignoreTab Ignore the tabs an attribute should be shown on.
     *
     * @return string the edit form as a string
     */
    public function editForm(
        $mode = 'add',
        $record = null,
        $forceList = '',
        $suppressList = '',
        $fieldprefix = '',
        $template = '',
        $ignoreTab = false
    ) {
        $node = $this->m_node;

        /* get data, transform into form, return */
        $data = $node->editArray($mode, $record, $forceList, $suppressList, $fieldprefix, $ignoreTab);
        // Format some things for use in tpl.
        /* check for errors and display them */
        $tab = $node->getActiveTab();
        $error_title = '';
        $pk_err_attrib = [];
        $tabs = $node->getTabs($node->m_action);

        // Handle errors
        $errors = [];
        if (Tools::count($data['error']) > 0) {
            $error_title = '<b>'.Tools::atktext('error_formdataerror').'</b>';

            foreach ($data['error'] as $error) {
                if ($error['err'] == 'error_primarykey_exists') {
                    $pk_err_attrib[] = $error['attrib_name'];
                } else {
                    if (Tools::count($tabs) > 1 && $error['tab']) {
                        $tabLink = $this->getTabLink($node, $error);
                        $error_tab = ' ('.Tools::atktext('error_tab').' '.$tabLink.' )';
                    } else {
                        $tabLink = null;
                        $error_tab = '';
                    }

                    if (is_array($error['label'])) {
                        $label = implode(', ', $error['label']);
                    } else {
                        if (!empty($error['label'])) {
                            $label = $error['label'];
                        } else {
                            if (!is_array($error['attrib_name'])) {
                                $label = $node->text($error['attrib_name']);
                            } else {
                                $label = [];
                                foreach ($error['attrib_name'] as $attrib) {
                                    $label[] = $node->text($attrib);
                                }

                                $label = implode(', ', $label);
                            }
                        }
                    }

                    /* Error messages should be rendered in templates using message, label and the link to the tab. */
                    $err = array('message' => $error['msg'], 'tablink' => $tabLink, 'label' => $label);

                    /*
                     * @deprecated: For backwards compatibility, we still support the msg variable as well.
                     * Although the message, tablink variables should be used instead of msg and tab.
                     */
                    $err = array_merge($err, array('msg' => $error['msg'].$error_tab));

                    $errors[] = $err;
                }
            }

            if (Tools::count($pk_err_attrib) > 0) { // Make primary key error message
                $pk_err_msg = '';
                for ($i = 0; $i < Tools::count($pk_err_attrib); ++$i) {
                    $pk_err_msg .= Tools::atktext($pk_err_attrib[$i], $node->m_module, $node->m_type);
                    if (($i + 1) < Tools::count($pk_err_attrib)) {
                        $pk_err_msg .= ', ';
                    }
                }
                $errors[] = array('label' => Tools::atktext('error_primarykey_exists'), 'message' => $pk_err_msg);
            }
        }

        /* display the edit fields */
        $fields = [];
        $errorFields = [];
        $attributes = [];

        for ($i = 0, $_i = Tools::count($data['fields']); $i < $_i; ++$i) {
            $field = &$data['fields'][$i];
            $tplfield = $this->createTplField($data['fields'], $i, $mode, $tab);
            $fields[] = $tplfield; // make field available in numeric array
            $params[isset($field['name'])?$field['name']:null] = $tplfield; // make field available in associative array
            $attributes[isset($field['name'])?$field['name']:null] = $tplfield; // make field available in associative array

            if (!empty($field['error'])) {
                $errorFields[] = $field['id'];
            }
        }

        $ui = $this->getUi();
        $page = $this->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/formsubmit.js');

        // register fields that contain errornous values
        $page->register_scriptcode('var atkErrorFields = '.Json::encode($errorFields).';');

        $result = '';

        foreach ($data['hide'] as $hidden) {
            $result .= $hidden;
        }

        $params['activeTab'] = $tab;
        $params['fields'] = $fields; // add all fields as a numeric array.
        $params['attributes'] = $attributes; // add all fields as an associative array

        $params['errortitle'] = $error_title;
        $params['errors'] = $errors; // Add the list of errors.
        Tools::atkdebug("Render editform - $template");
        if ($template) {
            $result .= $ui->render($template, $params);
        } else {
            $tabTpl = $this->_getTabTpl($node, $tabs, $mode, $record);
            $params['fieldspart'] = $this->_renderTabs($fields, $tabTpl);
            $result .= $ui->render('editform_common.tpl', $params);
        }

        return $result;
    }

    /**
     * Get the link fo a tab.
     *
     * @param Node $node The node
     * @param array $error
     *
     * @return string HTML code with link
     */
    public function getTabLink($node, $error)
    {
        if (Tools::count($node->getTabs($node->m_action)) < 2) {
            return '';
        }

        return '<a href="javascript:void(0)" onclick="ATK.Tabs.showTab(\''.$error['tab'].'\'); return false;">'.$this->getTabLabel($node, $error['tab']).'</a>';
    }

    /**
     * Overrideable function to create a header for edit mode.
     * Similar to the admin header functionality.
     */
    public function editHeader()
    {
        return '';
    }
}
