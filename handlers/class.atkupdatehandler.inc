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
   *
   */
  class atkUpdatehandler extends atkActionHandler 
  {    
    /**
     * The action handler method.
     */
    function action_update()
    {
      $record = $this->m_node->updateRecord();

      if ($this->m_postvars['atkcancel']=="")
      {      

        // just before we validate the record we call the preUpdate() to check if the record needs to be modified
        $this->m_node->preUpdate($record);

        $this->m_node->validate($record, "update");

        $error = count($record['atkerror']) > 0;
        foreach (array_keys($record) as $key)
          $error = $error || (is_array($record[$key]) && count($record[$key]['atkerror']) > 0);

        if ($error)
        {
          $this->m_node->m_action="edit";
          $page = &$this->getPage();
          $edithandler = $this->m_node->getHandler("edit");
          $page->addContent($edithandler->invoke("editPage", $record));
        }
        else
        {
          if(!$this->m_node->updateDb($record))
          {
            $this->m_node->m_db->rollback();
            if($this->m_node->m_db->getErrorType()=="user")
            {
              triggerError($record, 'Error', $this->m_node->m_db->getErrorMsg(), '', '');
              $this->m_node->m_action="edit";
              $page = &$this->getPage();
              $edithandler = $this->m_node->getHandler("edit");
              $page->addContent($edithandler->invoke("editPage", $record));
            }
            else
            {
              $location = $this->m_node->feedbackUrl("update",ACTION_FAILED, $record, $this->m_node->m_db->getErrorMsg());
              $this->m_node->redirect($location);
            }
          }
          else
          {
            $this->m_node->m_db->commit();            
            
            if ($this->m_postvars['atknoclose']=="")
            {
              // 'save and close' was clicked
              $location = $this->m_node->feedbackUrl("update", ACTION_SUCCESS, $record, "");
              $this->m_node->redirect($location);
            }
            else
            {
              // 'save' was clicked
              $page = &$this->getPage();
            //  $this->m_action="edit";
              //update succesful, pk value might be changed so update m_orgkey
              $record["atkprimkey"] = $this->m_node->primaryKey($record);

              //$this->setOrgKeyValue($record);
              $this->m_node->m_action = "edit";              
              $edithandler = $this->m_node->getHandler("edit");
              $editpage = $edithandler->invoke("editPage", $record, $locked);
              $screen = $this->m_node->renderActionPage("edit", $editpage);
              $page->addContent($screen);
            }

          }
        }
      }
      else
      {
        // Cancel was pressed
        $location = $this->m_node->feedbackUrl("update", ACTION_CANCELLED, $record);
        $this->m_node->redirect($location);
      }
    }
  }

?>
