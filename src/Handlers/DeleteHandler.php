<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Session\SessionStore;

/**
 * Handler for the 'delete' action of a node. It asks the user for
 * confirmation and upon actual confirmation, deletes the record (and for
 * any attribute that has Attribute::AF_CASCADE_DELETE set, deletes any detail
 * information (if any) by calling the attributes' delete() method.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class DeleteHandler extends ActionHandler
{
    /**
     * The action handler.
     */
    public function action_delete()
    {
        if (!$this->_checkAllowed()) {
            $this->renderAccessDeniedPage();

            return;
        }

        if ((!empty($this->m_postvars['confirm']) || !empty($this->m_postvars['cancel'])) && !$this->isValidCSRFToken($this->m_postvars['atkcsrftoken'])) {
            $this->renderAccessDeniedPage();

            return;
        }

        if (!empty($this->m_postvars['confirm'])) {
            $this->_doDelete();
        } elseif (empty($this->m_node->m_postvars['cancel'])) {
            // Confirmation page was not displayed

            if (!$this->checkAttributes()) {
                return;
            }

            // If we got here, then the node is not locked and we haven't displayed the
            // confirmation page yet, so we display it
            $page = $this->getPage();
            $page->addContent($this->m_node->renderActionPage('delete',
                $this->m_node->confirmAction($this->m_postvars['atkselector'], 'delete', true, $this->getCSRFToken())));
        } else {
            $this->_handleCancelAction();
        }
    }

    protected function _handleCancelAction()
    {
        // Confirmation page was displayed and 'no' was clicked
        $location = $this->m_node->feedbackUrl('delete', self::ACTION_CANCELLED);
        $this->m_node->redirect($location);
    }

    /**
     * Check if we are allowed to remove the given records.
     *
     * @return bool is delete action allowed?
     */
    public function _checkAllowed()
    {
        $selector = $this->m_node->primaryKeyFromString($this->m_postvars['atkselector']);
        $recordset = $this->m_node->select($atkselector)->mode('delete')->fetchAll();
        foreach ($recordset as $record) {
            if (!$this->allowed($record)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Given an confirmed delete, determine where the record
     * needs to be deleted (session or dabase), delete it
     * and redirect to the feedback url.
     */
    protected function _doDelete()
    {
        $atkstoretype = '';
        $sessionmanager = SessionManager::getInstance();
        if ($sessionmanager) {
            $atkstoretype = $sessionmanager->stackVar('atkstore');
        }
        switch ($atkstoretype) {
            case 'session':
                $result = $this->_doDeleteSession();
                break;
            default:
                $result = $this->_doDeleteDb();
                break;
        }

        if ($result === true) {
            $location = $this->m_node->feedbackUrl('delete', self::ACTION_SUCCESS);
        } else {
            $location = $this->m_node->feedbackUrl('delete', self::ACTION_FAILED, null, $result);
        }

        $this->m_node->redirect($location);
    }

    /**
     * Delete the record in the database.
     *
     * @return mixed Results, true or string with errormessage
     */
    protected function _doDeleteDb()
    {
        $db = $this->m_node->getDb();
        if ($this->m_node->deleteDb($this->m_node->primaryKeyFromString($this->m_postvars['atkselector']))) {
            $db->commit();
            $this->clearCache();

            return true;
        } else { // Something is wrong here, the deleteDb failed
            $db->rollback();

            return $db->getErrorMsg();
        }
    }

    /**
     * Delete the database in the session.
     *
     * @return bool Results, true or false
     */
    protected function _doDeleteSession()
    {
        $selector = Tools::atkArrayNvl($this->m_postvars, 'atkselector', '');

        return SessionStore::getInstance()->deleteDataRowForSelector($selector);
    }

    /**
     * Checks with each of the attributes of the node whose record is about to be deleted
     * if they allow the deletion.
     *
     * @return bool wether or not the attributes have allowed deletion
     */
    public function checkAttributes()
    {
        foreach ($this->m_node->getAttributes() as $attrib) {
            // If allowed !=== true, then it returned an error message
            if ($attrib->deleteAllowed() !== true) {
                $db = $this->m_node->getDb();
                $db->rollback();
                $location = $this->m_node->feedbackUrl('delete', self::ACTION_FAILED, null,
                    sprintf(Tools::atktext('attrib_delete_not_allowed'), Tools::atktext($attrib->m_name, $this->m_node->m_module, $this->m_node->m_type))
                );
                $this->m_node->redirect($location);

                return false;
            }
        }

        return true;
    }
}
