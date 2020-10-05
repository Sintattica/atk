<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Session\SessionStore;

/**
 * Handler class for the update action of a node. The action saves an
 * existing record to the database. The data is retrieved from the postvars.
 * This is the action that follows an 'edit' action. The 'edit' action
 * draws the edit form, the 'update' action saves the data to the database.
 * Validation of the record is performed before storage. If validation
 * fails, the edit handler is invoked again.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 *
 * @todo Add locking check for when an application calls an action_update on a locked node
 */
class UpdateHandler extends ActionHandler
{
    /**
     * Edit action.
     *
     * @var string
     */
    private $m_editAction = 'edit';

    /**
     * The action handler method.
     */
    public function action_update()
    {
        // clear old reject info
        $this->setRejectInfo(null);

        if (isset($this->m_partial) && $this->m_partial != '') {
            $this->partial($this->m_partial);

            return;
        } else {
            $this->doUpdate();
        }
    }

    /**
     * Returns the edit action, which is called when we want to return
     * the user to the edit form.
     *
     * Defaults to the 'edit' action.
     *
     * @return string edit action
     */
    public function getEditAction()
    {
        return $this->m_editAction;
    }

    /**
     * Sets the edit action which should be called when we need to return
     * the user to the edit form.
     *
     * @param string $action action name
     */
    public function setEditAction($action)
    {
        $this->m_editAction = $action;
    }

    /**
     * Perform the update action.
     */
    public function doUpdate()
    {
        $record = $this->getRecord();

        // allowed to update record?
        if (!$this->allowed($record)) {
            $this->handleAccessDenied();

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

        if (isset($this->m_postvars['atknoclose']) || isset($this->m_postvars['atksaveandclose'])) {
            $this->handleProcess($record);
        } else {
            if (isset($this->m_postvars['atkcancel'])) {
                $this->invoke('handleCancel', $record);
            } else {
                $sm = SessionManager::getInstance();
                // something other than one of the three buttons was pressed. Let's just refresh.
                $location = $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), $this->getEditAction(), array(
                    'atkselector' => $this->m_node->primaryKeyString($record),
                    'atktab' => $this->m_node->getActiveTab(),
                )), SessionManager::SESSION_REPLACE);
                $this->m_node->redirect($location);
            }
        }
    }

    /**
     * Get the record for updating.
     *
     * @return array The record to update
     */
    public function getRecord()
    {
        return $this->m_node->updateRecord();
    }

    /**
     * Called when the acces to this action was denied
     * for the current user.
     */
    public function handleAccessDenied()
    {
        $this->renderAccessDeniedPage();
    }

    /**
     * Called when the user clicks cancel.
     *
     * @param array $record
     */
    public function handleCancel($record)
    {
        $location = $this->m_node->feedbackUrl('update', self::ACTION_CANCELLED, $record, '', 2);
        $this->m_node->redirect($location);
    }

    /**
     * Process a record (preUpdate/validate/store).
     *
     * @param array $record Record to store
     * @param string $errorHandler Error handler method to call on current handler
     * @param string $successHandler Success handler method to call on current handler
     * @param array $extraParams Extra params to pass along to error/success handler methods
     *
     * @return bool Wether the process succeeded in storing the record
     */
    public function handleProcess(
        $record,
        $errorHandler = 'handleUpdateError',
        $successHandler = 'handleUpdateSuccess',
        $extraParams = []
    ) {
        // empty the postvars because we don't want to use these
        $postvars = $this->getNode()->m_postvars;
        $this->getNode()->m_postvars = [];
        // load original record if needed
        $this->getNode()->trackChangesIfNeeded($record);
        // put the postvars back
        $this->getNode()->m_postvars = $postvars;

        // just before we validate the record we call the preUpdate() to check if the record needs to be modified
        $this->m_node->executeTrigger('preUpdate', $record);

        $this->m_node->validate($record, 'update');

        $error = $this->hasError($record);

        if ($error) {
            $this->invoke($errorHandler, $record, null, $extraParams);

            return false;
        }

        $result = $this->updateRecord($record);
        if ($result) {
            $this->invoke($successHandler, $record, $extraParams);
        } else {
            $error = $result;
            $this->invoke($errorHandler, $record, $error, $extraParams);
        }

        return true;
    }

    /**
     * Check if there is an error (this can be determined by the
     * variable atkerror in the record).
     *
     * @param array $record Record to check for errors
     *
     * @return bool Error detected?
     */
    public function hasError($record)
    {
        $error = false;
        if (isset($record['atkerror'])) {
            $error = Tools::count($record['atkerror']) > 0;
            foreach (array_keys($record) as $key) {
                $error = $error || (is_array($record[$key]) && array_key_exists('atkerror', $record[$key]) && Tools::count($record[$key]['atkerror']) > 0);
            }
        }

        return $error;
    }

    /**
     * Update a record, determines wether to update it to the session or the database.
     *
     * @param array $record Record to update
     *
     * @return mixed Result of the update, true, false or string with error
     */
    private function updateRecord(&$record)
    {
        $atkstoretype = '';
        $sessionmanager = SessionManager::getInstance();
        if ($sessionmanager) {
            $atkstoretype = $sessionmanager->stackVar('atkstore');
        }
        switch ($atkstoretype) {
            case 'session':
                $result = $this->updateRecordInSession($record);
                break;
            default:
                $result = $this->updateRecordInDb($record);
                break;
        }

        return $result;
    }

    /**
     * Update a record in the database.
     *
     * @param array $record Record to update
     *
     * @return mixed Result of the update, true, false or string with error
     */
    private function updateRecordInDb(&$record)
    {
        $db = $this->m_node->getDb();
        if ($this->m_node->updateDb($record)) {
            $db->commit();
            $this->notify('update', $record);

            $this->clearCache();

            return true;
        } else {
            $db->rollback();
            if ($db->getErrorType() == 'user') {
                Tools::triggerError($record, 'Error', $db->getErrorMsg(), '', '');

                return false;
            }

            return $db->getErrorMsg();
        }
    }

    /**
     * Update a record in the session.
     *
     * @param array $record Record to update
     *
     * @return mixed Result of the update, true or false
     */
    private function updateRecordInSession($record)
    {
        $selector = Tools::atkArrayNvl($this->m_postvars, 'atkselector', '');

        return SessionStore::getInstance()->updateDataRowForSelector($selector, $record) !== false;
    }

    /**
     * Handle update error. This can either be an error in the record data the user
     * can correct or a fatal error when saving the record in the database. If the
     * latter is the case the $error parameter is set.
     *
     * This method can be overriden inside your node.
     *
     * @param array $record the record
     * @param string $error error string (only on fatal errors)
     * @param array $record
     */
    public function handleUpdateError($record, $error = null)
    {
        if ($this->hasError($record)) {
            $this->setRejectInfo($record);
            $sm = SessionManager::getInstance();
            $location = $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), $this->getEditAction(),
                array('atkselector' => $this->m_node->primaryKeyString($record))), SessionManager::SESSION_BACK);
            $this->m_node->redirect($location);
        } else {
            $location = $this->m_node->feedbackUrl('update', self::ACTION_FAILED, $record, $error);
            $this->m_node->redirect($location);
        }
    }

    /**
     * Handle update success. Normally redirects the user either back to the edit form
     * (when the user only saved) or back to the previous action if the user choose save
     * and close.
     *
     * This method can be overriden inside your node.
     *
     * @param array $record the record
     */
    public function handleUpdateSuccess($record)
    {
        if (isset($this->m_postvars['atknoclose'])) {
            // 'save' was clicked
            $params = array(
                'atkselector' => $this->m_node->primaryKeyString($record),
                'atktab' => $this->m_node->getActiveTab(),
            );
            $sm = SessionManager::getInstance();
            $location = $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), $this->getEditAction(), $params), SessionManager::SESSION_REPLACE, 1);
        } else {
            // 'save and close' was clicked
            $location = $this->m_node->feedbackUrl('update', self::ACTION_SUCCESS, $record, '', 2);
        }

        $this->m_node->redirect($location, $record);
    }
}
