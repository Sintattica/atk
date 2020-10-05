<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Session\SessionStore;
use Sintattica\Atk\Core\Config;

/**
 * Handler class for the save action of a node. The action saves a
 * new record to the database. The data is retrieved from the postvars.
 * This is the action that follows an 'add' action. The 'add' action
 * draws the add form, the 'save' action saves the data to the database.
 * Validation of the record is performed before storage. If validation
 * fails, the add handler is invoked again.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class SaveHandler extends ActionHandler
{
    /**
     * Add action.
     *
     * @var string
     */
    private $m_addAction = 'add';

    /**
     * The action handler method.
     */
    public function action_save()
    {
        // clear old reject info
        $this->setRejectInfo(null);

        if (isset($this->m_partial) && !empty($this->m_partial)) {
            $this->partial($this->m_partial);

            return;
        } else {
            $this->doSave();
        }
    }

    /**
     * Returns the add action, which is called when we want to return
     * the user to the add form.
     *
     * Defaults to the 'add' action.
     *
     * @return string add action
     */
    public function getAddAction()
    {
        return $this->m_addAction;
    }

    /**
     * Sets the add action which should be called when we need to return
     * the user to the add form.
     *
     * @param string $action action name
     */
    public function setAddAction($action)
    {
        $this->m_addAction = $action;
    }

    /**
     * Save record.
     */
    public function doSave()
    {
        $record = $this->m_node->updateRecord();

        // allowed to save record?
        if (!$this->allowed($record)) {
            $this->renderAccessDeniedPage();

            return;
        }

        $prefix = '';
        if (isset($this->m_postvars['atkfieldprefix'])) {
            $prefix = $this->m_postvars['atkfieldprefix'];
        }

        $csrfToken = isset($this->m_postvars[$prefix.'atkcsrftoken']) ? $this->m_postvars[$prefix.'atkcsrftoken'] : null;

        // check for CSRF token
        if (!$this->isValidCSRFToken($csrfToken)) {
            $this->renderAccessDeniedPage();

            return;
        }

        if (isset($this->m_postvars['atksaveandclose']) || isset($this->m_postvars['atksaveandnext']) || isset($this->m_postvars['atksaveandcontinue'])) {
            $this->handleProcess($record);
        } else {
            if (isset($this->m_postvars['atkcancel'])) {
                // Cancel was pressed
                $location = $this->m_node->feedbackUrl('save', self::ACTION_CANCELLED, $record, '', $this->_getSkip());
                $this->_handleRedirect($location);
            }
        }
    }

    /**
     * @param array $record Record to store
     */
    public function handleProcess($record)
    {
        // just before we validate the record we call the preAdd() to check if the record needs to be modified
        if (!$this->m_node->executeTrigger('preAdd', $record, 'add')) {
            $this->handleAddError($record);

            return;
        }

        $this->validate($record);

        if (!isset($record['atkerror'])) {
            $record['atkerror'] = [];
        }

        $error = Tools::count($record['atkerror']) > 0;

        $db = $this->m_node->getDb();
        if ($error) {
            // something went wrong, back to where we came from
            $db->rollback();

            return $this->goBack($record);
        } else {
            if (!$this->storeRecord($record)) {
                $this->handleAddError($record);

                return;
            } else {
                $location = $this->invoke('getSuccessReturnURL', $record);
                $this->_handleRedirect($location, $record);
            }
        }
    }

    /**
     * Redirect after save.
     *
     * @param string $location
     * @param array|bool $recordOrExit
     * @param bool $exit
     * @param int $levelskip
     */
    protected function _handleRedirect($location = '', $recordOrExit = [], $exit = false, $levelskip = 1)
    {
        $this->m_node->redirect($location, $recordOrExit, $exit, $levelskip);
    }

    /**
     * Get the URL to redirect to after successfully saving a record.
     *
     * @param array $record Saved record
     *
     * @return string Location to redirect to
     */
    protected function getSuccessReturnURL($record)
    {
        $sm = SessionManager::getInstance();
        if ($this->m_node->hasFlag(Node::NF_EDITAFTERADD) && $this->m_node->allowed('edit')) {
            // forward atkpkret for newly added records
            $extra = '';
            if (isset($this->m_postvars['atkpkret'])) {
                $extra = '&atkpkret='.rawurlencode($this->m_postvars['atkpkret']);
            }

            $url = Config::getGlobal('dispatcher').'?atknodeuri='.$this->m_node->atkNodeUri();
            $url .= '&atkaction=edit';
            $url .= '&atkselector='.rawurlencode($this->m_node->primaryKeyString($record));
            $location = $sm->sessionUrl($url.$extra, SessionManager::SESSION_REPLACE, $this->_getSkip() - 1);
        } else {
            if ($this->m_node->hasFlag(Node::NF_ADDAFTERADD) && isset($this->m_postvars['atksaveandnext'])) {
                $filter = '';
                if (isset($this->m_node->m_postvars['atkforce'])) {
                    $filter = '&atkforce='.rawurlencode($this->m_node->m_postvars['atkforce']);
                }
                $url = Config::getGlobal('dispatcher').'?atknodeuri='.$this->m_node->atkNodeUri().'&atkaction='.$this->getAddAction();
                $location = $sm->sessionUrl($url.$filter, SessionManager::SESSION_REPLACE, $this->_getSkip() - 1);
            } else {
                // normal succesful save
                $location = $this->m_node->feedbackUrl('save', self::ACTION_SUCCESS, $record, '', $this->_getSkip());
            }
        }

        return $location;
    }

    /**
     * Store a record, either in the database or in the session.
     *
     * @param array $record Record to store
     *
     * @return bool Successfull save?
     */
    public function storeRecord(&$record)
    {
        $atkstoretype = '';
        $sessionmanager = SessionManager::getInstance();
        if ($sessionmanager) {
            $atkstoretype = $sessionmanager->stackVar('atkstore');
        }
        switch ($atkstoretype) {
            case 'session':
                return $this->storeRecordInSession($record);
            default:
                return $this->storeRecordInDb($record);
        }
    }

    /**
     * Store a record in the session.
     *
     * @param array $record Record to store in the session
     *
     * @return bool Successfull save?
     */
    protected function storeRecordInSession(&$record)
    {
        Tools::atkdebug('STORING RECORD IN SESSION');
        $result = SessionStore::getInstance()->addDataRow($record, $this->m_node->primaryKeyField());

        return $result !== false;
    }

    /**
     * Store a record in the database.
     *
     * @param array $record Record to store in the database
     *
     * @return bool Successfull save?
     */
    protected function storeRecordInDb(&$record)
    {
        if (!$this->m_node->addDb($record, true, 'add')) {
            return false;
        }

        $this->m_node->getDb()->commit();
        $this->notify('save', $record);
        $this->clearCache();

        return true;
    }

    /**
     * Handle error in preAdd/addDb.
     *
     * @param array $record
     */
    public function handleAddError($record)
    {
        // Do a rollback on an error
        $db = $this->m_node->getDb();
        $db->rollback();

        if ($db->getErrorType() == 'user') {
            Tools::triggerError($record, 'Error', $db->getErrorMsg(), '', '');

            // still an error, back to where we came from
            $this->goBack($record);
        } else {
            $location = $this->m_node->feedbackUrl('save', self::ACTION_FAILED, $record, $db->getErrorMsg());
            $this->_handleRedirect($location);
        }
    }

    /**
     * Get the number of levels to skip.
     *
     * @return int The number of levels to skip
     */
    public function _getSkip()
    {
        if (isset($this->m_postvars['atkreturnbehaviour']) && $this->m_postvars['atkreturnbehaviour'] == self::ATK_ACTION_BACK) {
            return 2;
        }

        return 1;
    }

    /**
     * Go back to the add page.
     *
     * @param array $record The record with reject info
     */
    public function goBack($record)
    {
        $this->setRejectInfo($record);
        $this->_handleRedirect();
    }

    /**
     * Validate record.
     *
     * @param array $record The record to validate
     *
     * @return bool
     */
    public function validate(&$record)
    {
        $error = (!$this->m_node->validate($record, 'add'));

        if (!isset($record['atkerror'])) {
            $record['atkerror'] = [];
        }

        $error = $error || Tools::count($record['atkerror']) > 0;

        foreach (array_keys($record) as $key) {
            $error = $error || (is_array($record[$key]) && array_key_exists('atkerror', $record[$key]) && Tools::count($record[$key]['atkerror']) > 0);
        }

        return !$error;
    }
}
