<?php
 /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage handlers
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Handler class for the update action of a node. The action saves an
   * existing record to the database. The data is retrieved from the postvars.
   * This is the action that follows an 'edit' action. The 'edit' action
   * draws the edit form, the 'update' action saves the data to the database.
   * Validation of the record is performed before storage. If validation
   * fails, the edit handler is invoked again.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   * @todo Add locking check for when an application calls an action_update on a locked node
   */
  class atkUpdatehandler extends atkActionHandler
  {
    /**
     * The action handler method.
     */
    function action_update()
    {
      $record = $this->m_node->updateRecord();
      
      // allowed to update record?
      if (!$this->allowed($record))
      {
        $this->renderAccessDeniedPage();
        return;
      }

      if (isset($this->m_postvars['atknoclose'])||
          isset($this->m_postvars['atksaveandclose']) ||
          isset($this->m_postvars['atkwizardaction']))
      {

        // just before we validate the record we call the preUpdate() to check if the record needs to be modified
        $this->m_node->executeTrigger("preUpdate", $record);

        $this->m_node->validate($record, "update");

        $error = false;
        if (isset($record['atkerror']))
        {
          $error = count($record['atkerror']) > 0;
          foreach (array_keys($record) as $key)
            $error = $error || (is_array($record[$key]) && count($record[$key]['atkerror']) > 0);
        }

        if ($error)
        {
          $this->m_node->m_action="edit";
          $page = &$this->getPage();
          $edithandler = $this->m_node->getHandler("edit");
          $page->addContent($this->m_node->renderActionPage("edit", $edithandler->invoke("editPage", $record)));
        }
        else
        {
          $db = &$this->m_node->getDb();
          if(!$this->m_node->updateDb($record))
          {
            $db->rollback();
            if($db->getErrorType()=="user")
            {
              triggerError($record, 'Error', $db->getErrorMsg(), '', '');
              $this->m_node->m_action="edit";
              global $ATK_VARS; $ATK_VARS["atkaction"]="edit";
              $page = &$this->getPage();
              $edithandler = $this->m_node->getHandler("edit");
              $page->addContent($edithandler->invoke("editPage", $record));
            }
            else
            {
              $location = $this->m_node->feedbackUrl("update",ACTION_FAILED, $record, $db->getErrorMsg());
              $this->m_node->redirect($location, $record);
            }
          }
          else
          {
            $db->commit();
            $this->notify("update", $record);

            $this->clearCache();
            if (!isset($this->m_postvars['atknoclose']))
            {
              // 'save and close' was clicked
              $location = $this->m_node->feedbackUrl("update", ACTION_SUCCESS, $record, "");
              $this->m_node->redirect($location);
            }
            else
            {
              // 'save' was clicked
              $this->renderEditPage($record);
            }

          }
        }
      }
      else if (isset($this->m_postvars['atkcancel']))
      {
        // Cancel was pressed
        $location = $this->m_node->feedbackUrl("update", ACTION_CANCELLED, $record);
        $this->m_node->redirect($location);
      }
      else
      {
        // something other than one of the three buttons was pressed. Let's just refresh.
        $this->renderEditPage($record);
      }
    }

    function renderEditPage(&$record)
    {
       $page = &$this->getPage();
    //  $this->m_action="edit";
      //update succesful, pk value might be changed so update m_orgkey
      $record["atkprimkey"] = $this->m_node->primaryKey($record);

      $locked = ($this->m_node->hasFlag(NF_LOCK));

      //$this->setOrgKeyValue($record);
      $this->m_node->m_action = "edit";
      // we have to make the entire system think we are in edit mode again.
      global $ATK_VARS; $ATK_VARS["atkaction"]="edit";
      $edithandler = $this->m_node->getHandler("edit");
      $editpage = $edithandler->invoke("editPage", $record, $locked);
      $screen = $this->m_node->renderActionPage("edit", $editpage);
      $page->addContent($screen);
    }
  }

?>
