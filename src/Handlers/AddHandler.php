<?php namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Session\State;
use Sintattica\Atk\Session\SessionManager;

/**
 * Handler for the 'add' action of a node. It draws a page where the user
 * can enter a new record.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage handlers
 *
 */
class AddHandler extends ActionHandler
{
    var $m_buttonsource = null;

    /**
     * Save action.
     *
     * @var string
     */
    private $m_saveAction = 'save';

    /**
     * Constructor
     *
     * @return AddHandler
     */
    function __construct()
    {
        parent::__construct();
        $this->setReturnBehaviour(self::ATK_ACTION_BACK);
    }

    /**
     * The action handler.
     */
    function action_add()
    {
        if (!empty($this->m_partial)) {
            $this->partial($this->m_partial);
            return;
        }

        // we could get here because of a reject.
        $record = $this->getRejectInfo();

        $page = $this->getPage();
        $page->addContent($this->m_node->renderActionPage("add", $this->invoke("addPage", $record)));
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
     * @return String HTML A String containing a box with an add form.
     */
    function addPage($record = null)
    {
        $result = $this->getAddPage($record);
        if ($result !== false) {
            return $result;
        }
        return null;
    }

    /**
     * Create an add page.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return String HTML A String containing a box with an add form.
     */
    function getAddPage($record = null)
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
     * @param Object $object An object that implements the getFormButtons() method
     */
    function setButtonSource(&$object)
    {
        $this->m_buttonsource = &$object;
    }

    /**
     * Register external javascript and css files for the handler
     */
    function registerExternalFiles()
    {
        $page = $this->getPage();
        $page->register_script(Config::getGlobal("assets_url") . "javascript/tools.js");
    }

    /**
     * Retrieve the parameters needed to render the Add form elements
     * in a smarty template.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return array An array containing the elements used in a template for
     *               add pages.
     */
    function getAddParams($record = null)
    {
        $node = $this->m_node;
        $ui = &$node->getUi();

        if (!is_object($ui)) {
            Tools::atkerror("ui object failure");
            return false;
        }

        $params = $node->getDefaultActionParams();
        $params["title"] = $node->actionTitle('add');
        $params["header"] = $this->invoke("addHeader", $record);
        $params["formstart"] = $this->getFormStart();
        $params["content"] = $this->getContent($record);
        $params["buttons"] = $this->getFormButtons($record);
        $params["formend"] = $this->getFormEnd();
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
     * @return String HTML Form open tag and hidden variables.
     */
    function getFormStart()
    {
        $sm = SessionManager::getInstance();
        $node = $this->m_node;

        $formstart = '<form id="' . "entryform" . '" name="' . "entryform" . '" enctype="multipart/form-data" action="' . Config::getGlobal('dispatcher') . '?' . SID . '"' .
            ' method="post" onsubmit="return globalSubmit(this,false)" autocomplete="off">';


        $formstart .= $sm->formState(SessionManager::SESSION_NESTED, $this->getReturnBehaviour(), $node->getEditFieldPrefix());
        $formstart .= '<input type="hidden" name="' . $this->getNode()->getEditFieldPrefix() . 'atkaction" value="' . $this->getSaveAction() . '" />';
        $formstart .= '<input type="hidden" name="' . $this->getNode()->getEditFieldPrefix() . 'atkprevaction" value="' . $this->getNode()->m_action . '" />';
        $formstart .= '<input type="hidden" name="' . $this->getNode()->getEditFieldPrefix() . 'atkcsrftoken" value="' . $this->getCSRFToken() . '" />';
        $formstart .= '<input type="hidden" class="atksubmitaction" />';

        if (isset($node->m_postvars['atkfilter'])) {
            $formstart .= '<input type="hidden" name="atkfilter" value="' . $node->m_postvars['atkfilter'] . '">';
        }

        return $formstart;
    }

    /**
     * Retrieve the content of an add page.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return String HTML Content of the addpage.
     */
    function getContent($record)
    {
        $node = $this->m_node;

        // Utility.
        $edithandler = &$node->getHandler("edit");

        $forceList = $this->invoke("createForceList");
        $form = $edithandler->editForm("add", $record, $forceList, '', $node->getEditFieldPrefix());

        return $node->tabulate("add", $form);
    }

    /**
     * Based on information provided in the url (atkfilter), this function creates an array with
     * field values that are used as the initial values of a record in an add page.
     *
     * @return array Values of the newly created record.
     */
    function createForceList()
    {
        $node = $this->m_node;
        $forceList = array();
        $filterList = (isset($node->m_postvars['atkfilter'])) ? Tools::decodeKeyValueSet($node->m_postvars['atkfilter'])
            : array();
        foreach ($filterList as $field => $value) {
            list($table, $column) = explode('.', $field);
            if ($column == null) {
                $forceList[$table] = $value;
            } else {
                if ($table == $this->getNode()->getTable()) {
                    $forceList[$column] = $value;
                } else {
                    $forceList[$table][$column] = $value;
                }
            }
        }
        return $forceList;
    }

    /**
     * Get the end of the form.
     *
     * @return String HTML The forms' end
     */
    function getFormEnd()
    {
        return '</form>';
    }

    /**
     * Retrieve an array of form buttons that are rendered in the add page.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return array a list of HTML buttons.
     */
    function getFormButtons($record = null)
    {
        // If no custom button source is given, get the default
        if ($this->m_buttonsource === null) {
            $this->m_buttonsource = $this->m_node;
        }

        return $this->m_buttonsource->getFormButtons("add", $record);
    }

    /**
     * Renders a complete add page including title and content
     *
     * @param array $params Parameters needed in templates for the add page
     * @return String HTML the add page.
     */
    function renderAddPage($params)
    {
        $node = $this->m_node;
        $ui = &$node->getUi();

        if (is_object($ui)) {
            $output = $ui->renderAction("add", $params);
            $this->addRenderBoxVar("title", $node->actionTitle('add'));
            $this->addRenderBoxVar("content", $output);
            $total = $ui->renderBox($this->m_renderBoxVars, $this->m_boxTemplate);
            return $total;
        }
        return null;
    }

    /**
     * Handler for partial actions on an add page
     *
     * @param string $partial full partial name
     * @return string
     */
    function partial_attribute($partial)
    {
        list($type, $attribute, $partial) = explode('.', $partial);

        $attr = $this->m_node->getAttribute($attribute);
        if ($attr == null) {
            Tools::atkerror("Unknown / invalid attribute '$attribute' for node '" . $this->m_node->atkNodeUri() . "'");
            return '';
        }

        return $attr->partial($partial, 'add');
    }

    /**
     * Partial handler for section state changes.
     */
    function partial_sectionstate()
    {
        State::set(array(
            "nodetype" => $this->m_node->atkNodeUri(),
            "section" => $this->m_postvars['atksectionname']
        ), $this->m_postvars['atksectionstate']);
    }
}

