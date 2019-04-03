<?php

namespace Sintattica\Atk\Handlers;

use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Tools;

/**
 * Handler for the 'editcopy' action of a node. It copies the selected
 * record, and then redirects to the edit action for the copied record.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
class EditcopyHandler extends ActionHandler
{
    /**
     * The action method.
     */
    public function action_editcopy()
    {
        Tools::atkdebug('node::action_editcopy()');

        $record = $this->getCopyRecord();
        // allowed to editcopy record?
        if (!$this->allowed($record)) {
            $this->renderAccessDeniedPage();

            return;
        }

        $db = $this->m_node->getDb();
        if (!$this->m_node->copyDb($record)) {
            $db->rollback();
            $location = $this->m_node->feedbackUrl('editcopy', self::ACTION_FAILED, $record, $db->getErrorMsg());
            $this->m_node->redirect($location);
        } else {
            $db->commit();
            $this->clearCache();
            $sm = SessionManager::getInstance();
            $location = $sm->sessionUrl(Tools::dispatch_url($this->m_node->atkNodeUri(), 'edit', array('atkselector' => $this->m_node->primaryKeyString($record))),
                SessionManager::SESSION_REPLACE);
            $this->m_node->redirect($location);
        }
    }

    /**
     * Get the selected record from.
     *
     * @return array the record to be copied
     */
    protected function getCopyRecord()
    {
        $selector = $this->m_postvars['atkselector'];
        $recordset = $this->m_node->select($selector)->mode('copy')->fetchAll();
        if (Tools::count($recordset) > 0) {
            return $recordset[0];
        } else {
            Tools::atkdebug("Geen records gevonden met selector: $selector");
            $this->m_node->redirect();
        }

        return;
    }
}
