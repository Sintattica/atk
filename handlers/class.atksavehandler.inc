<?php

  class atkSaveHandler extends atkActionHandler
  {
    function action_save()
    {      
      global $g_db;
      
      $record = $this->m_node->updateRecord();   
      
      if ($this->m_postvars['atkcancel']=="")
      {      

        // just before we validate the record we call the preAdd() to check if the record needs to be modified
        $this->m_node->preAdd($record);

        $this->m_node->validate($record, "add");


        if(!is_array($record['atkerror']))
          $record['atkerror'] = array();

        $error = count($record['atkerror']) > 0;

        foreach (array_keys($record) as $key)
        {
          $error = $error || (is_array($record[$key]) && array_key_exists('atkerror', $record[$key]) && count($record[$key]['atkerror']) > 0);
        }

        if ($error)
        {
          $this->m_action="add";
          $page = &$this->getPage();
          $addhandler = &$this->m_node->getHandler("add");
          $page->addContent($addhandler->invoke("addPage", $record));
        }
        else
        {
          if(!$this->m_node->addDb($record, true, "add"))
          {
            $g_db->rollback();
            if($g_db->getErrorType()=="user")
            {
              triggerError($record, 'Error', $g_db->getErrorMsg(), '', '');
              $this->m_action="add";
              $this->m_node->addPage($record);
            }
            else
            {
              $location = $this->m_node->feedbackUrl("save", ACTION_FAILED, $record, $g_db->getErrorMsg());
              $this->m_node->redirect($location);
            }
          }
          else
          {
            $g_db->commit();
            
            $location="";
            if ($this->m_node->hasFlag(NF_EDITAFTERADD))
            {
              $location = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->m_node->atknodetype().'&atkaction=edit&atkselector='.rawurlencode($this->m_node->primaryKey($record)), SESSION_REPLACE);
            }
            else
            {
              $location = $this->m_node->feedbackUrl("save", ACTION_SUCCESS, $record);
            }
            $this->m_node->redirect($location, $record);
          }
        }
      }
      else
      {
        // Cancel was pressed
        $location = $this->m_node->feedbackUrl("save", ACTION_CANCELLED, $record);
        $this->m_node->redirect($location);
      }
    }
  }

?>
