<?php

  class atkUpdatehandler extends atkActionHandler 
  {    
    
    function action_update()
    {
      global $g_db;
      
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
            $g_db->rollback();
            if($g_db->getErrorType()=="user")
            {
              triggerError($record, 'Error', $g_db->getErrorMsg(), '', '');
              $this->m_node->m_action="edit";
              $page = &$this->getPage();
              $edithandler = $this->m_node->getHandler("edit");
              $page->addContent($edithandler->invoke("editPage", $record));
            }
            else
            {
              $location = $this->m_node->feedbackUrl("update",ACTION_FAILED, $record, $g_db->getErrorMsg());
              $this->m_node->redirect($location);
            }
          }
          else
          {
            $g_db->commit();
            
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

              $locked = FALSE;
              if ($this->m_node->hasFlag(NF_LOCK))
              {                              
                $page->addContent($this->lockPage());
                return;
              }

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