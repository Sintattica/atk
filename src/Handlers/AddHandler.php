<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Session\State;
use Sintattica\Atk\Session\SessionManager;

/**
 * Handler for the 'add' action of a node. It draws a page where the user
 * can enter a new record.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class AddHandler extends ActionHandler
{
    public $m_buttonsource = null;

    /**
     * Save action.
     *
     * @var string
     */
    private $m_saveAction = 'save';

    /**
     * Constructor.
     *
     * @return AddHandler
     */
    public function __construct()
    {
        parent::__construct();
        $this->setReturnBehaviour(self::ATK_ACTION_BACK);
    }

    /**
     * The action handler.
     */
    public function action_add()
    {
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        }

        // we could get here because of a reject.
        $record = $this->getRejectInfo();

        $page = $this->getPage();
        $page->addContent($this->m_node->renderActionPage('add', $this->invoke('addPage', $record)));
    }

    /**
     * Returns the save action, which is called when posting the edit form.
     *
     * Defaults to the 'save' action.
     *
     * @return string save action
     */
    public function getSaveAction()
    {
        return $this->m_saveAction;
    }

    /**
     * Sets the save action which should be called when posting the edit form.
     *
     * @param string $action action name
     */
    public function setSaveAction($action)
    {
        $this->m_saveAction = $action;
    }

    /**
     * Creates an add page or null, of it cannot be created.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     *
     * @return string HTML A String containing a box with an add form.
     */
    public function addPage($record = null)
    {
        $result = $this->getAddPage($record);
        if ($result !== false) {
            return $result;
        }

        return;
    }

    /**
     * Create an add page.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     *
     * @return string HTML A String containing a box with an add form.
     */
    public function getAddPage($record = null)
    {
        // check if there are postvars set for filling the record, this
        // can happen when a new selection is made using the select handler etc.
        if ($record == null) {
            $record = $this->m_node->updateRecord('', null, null, true);
        }

        $this->registerExternalFiles();

        $params = $this->getAddParams($record);

        if ($params === false) {
            return false;
        }

        return $this->renderAddPage($params);
    }

    /**
     * Set the source object where the add handler should
     * retrieve the formbuttons from. Default this is the owner
     * node.
     *
     * @param object $object An object that implements the getFormButtons() method
     */
    public function setButtonSource($object)
    {
        $this->m_buttonsource = $object;
    }

    /**
     * Register external javascript and css files for the handler.
     */
    public function registerExternalFiles()
    {

    }

    /**
     * Retrieve the parameters needed to render the Add form elements
     * in a smarty template.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     *
     * @return array An array containing the elements used in a template for
     *               add pages.
     */
    public function getAddParams($record = null)
    {
        $node = $this->m_node;
        $ui = $node->getUi();

        if (!is_object($ui)) {
            Tools::atkerror('ui object failure');

            return false;
        }

        $params = $node->getDefaultActionParams();
        $params['title'] = $node->actionTitle('add');
        $params['header'] = $this->invoke('addHeader', $record);
        $params['formstart'] = $this->getFormStart();
        $params['content'] = $this->getContent($record);
        $params['buttons'] = $this->getFormButtons($record);
        $params['formend'] = $this->getFormEnd();

        return $params;
    }

    /**
     * Allows you to add an header above the addition form.
     *
     * @param array $record initial values
     *
     * @return string HTML or plain text that will be added above the add form.
     */
    public function addHeader($record = null)
    {
        return '';
    }

    /**
     * Retrieve the HTML code for the start of an HTML form and some
     * hidden variables needed by an add page.
     *
     * @return string HTML Form open tag and hidden variables.
     */
    public function getFormStart()
    {
        $sm = SessionManager::getInstance();
        $node = $this->m_node;

        $formstart = '<form id="entryform" name="entryform" enctype="multipart/form-data" action="'.Config::getGlobal('dispatcher').'"'.' method="post" onsubmit="return ATK.globalSubmit(this,false)" autocomplete="off" class="form-horizontal">';

        $formstart .= $sm->formState(SessionManager::SESSION_NESTED, $this->getReturnBehaviour(), $node->getEditFieldPrefix());
        $formstart .= '<input type="hidden" name="'.$this->getNode()->getEditFieldPrefix().'atkaction" value="'.$this->getSaveAction().'" />';
        $formstart .= '<input type="hidden" name="'.$this->getNode()->getEditFieldPrefix().'atkprevaction" value="'.$this->getNode()->m_action.'" />';
        $formstart .= '<input type="hidden" name="'.$this->getNode()->getEditFieldPrefix().'atkcsrftoken" value="'.$this->getCSRFToken().'" />';
        $formstart .= '<input type="hidden" class="atksubmitaction" />';

        return $formstart;
    }

    /**
     * Retrieve the content of an add page.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     *
     * @return string HTML Content of the addpage.
     */
    public function getContent($record)
    {
        $node = $this->m_node;

        /** @var EditHandler $edithandler */
        $edithandler = $node->getHandler('edit');

        $forceList = [];
        if (isset($node->m_postvars['atkforce'])) {
            $forceList = json_decode($node->m_postvars['atkforce'], true);
        }
        $form = $edithandler->editForm('add', $record, $forceList, '', $node->getEditFieldPrefix());

        return $node->tabulate('add', $form);
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
     * Retrieve an array of form buttons that are rendered in the add page.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     *
     * @return array a list of HTML buttons.
     */
    public function getFormButtons($record = null)
    {
        // If no custom button source is given, get the default
        if ($this->m_buttonsource === null) {
            $this->m_buttonsource = $this->m_node;
        }

        return $this->m_buttonsource->getFormButtons('add', $record);
    }

    /**
     * Renders a complete add page including title and content.
     *
     * @param array $params Parameters needed in templates for the add page
     *
     * @return string HTML the add page.
     */
    public function renderAddPage($params)
    {
        $node = $this->m_node;
        $ui = $node->getUi();

        if (is_object($ui)) {
            $output = $ui->renderAction('add', $params);
            $this->addRenderBoxVar('title', $node->actionTitle('add'));
            $this->addRenderBoxVar('content', $output);
            $total = $ui->renderBox($this->m_renderBoxVars, $this->m_boxTemplate);

            return $total;
        }

        return;
    }

    /**
     * Handler for partial actions on an add page.
     *
     * @param string $partial full partial name
     *
     * @return string
     */
    public function partial_attribute($partial)
    {
        list($type, $attribute, $partial) = explode('.', $partial);

        $attr = $this->m_node->getAttribute($attribute);
        if ($attr == null) {
            Tools::atkerror("Unknown / invalid attribute '$attribute' for node '".$this->m_node->atkNodeUri()."'");

            return '';
        }

        return $attr->partial($partial, 'add');
    }

    /**
     * Partial handler for section state changes.
     */
    public function partial_sectionstate()
    {
        State::set(array(
            'nodetype' => $this->m_node->atkNodeUri(),
            'section' => $this->m_postvars['atksectionname'],
        ), $this->m_postvars['atksectionstate']);
    }
}
