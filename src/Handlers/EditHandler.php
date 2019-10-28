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
        if (isset($node->m_postvars['atkfilter'])) {
            $forceList = Tools::decodeKeyValueSet($node->m_postvars['atkfilter']);
        }

        $suppressList = [];
        if (isset($node->m_postvars['atksuppress'])) {
            $suppressList = $node->m_postvars['atksuppress'];
        }

        return $this->editForm('edit', $record, $forceList, $suppressList, $node->getEditFieldPrefix());
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
     *
     * @return string the edit form as a string
     */
    public function editForm(
        $mode = 'add',
        $record = null,
        $forceList = '',
        $suppressList = '',
        $fieldprefix = '',
        $template = ''
    ) {
        $node = $this->m_node;

        /* get data, transform into form, return */
        $data = $node->editArray($mode, $record, $forceList, $suppressList, $fieldprefix);
        $params = [];
        $params['fields'] = $this->fieldsWithTabsAndSections($data['fields']); // add all fields as a numeric array.
        $params['tabs'] = $this->getTabs($params['fields']);
        $params['activeTab'] = $this->getActiveTab($param['fields'], $mode);

        // Format some things for use in tpl.
        /* check for errors and display them */
        $tab = $this->getActiveTab($param['fields'], $mode);
        $error_title = '';
        $pk_err_attrib = [];
        $tabs = $this->getTabs($params['fields']);
        $errorFields = [];

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

        foreach ($data['fields'] as $field) {
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
        $params['fields'] = $this->fieldsWithTabsAndSections($data['fields']); // add all fields as a numeric array.
        $result .= $this->tabulate('edit', $params['fields']);

        $params['errortitle'] = $error_title;
        $params['errors'] = $errors; // Add the list of errors.
        Tools::atkdebug("Render editform - $template");
        if ($template) {
            $result .= $ui->render($template, $params);
        } else {
            $tabTpl = $this->_getTabTpl($node, $tabs, $mode, $record);
            $params['fieldspart'] = $this->_renderTabs($params['fields'], $tabTpl);
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
