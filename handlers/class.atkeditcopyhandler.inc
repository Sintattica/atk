<?php

  class atkEditCopyHandler extends atkActionHandler
  {
    function action_editcopy()
    {
      atkdebug("atknode::action_editcopy()");
      $recordset = $this->m_node->selectDb($this->m_postvars['atkselector'],"","","","","copy");

      if(count($recordset) > 0)
      {
        if(!$this->m_node->copyDb($recordset[0]))
        {
          $this->m_node->m_db->rollback();
          $location = $this->m_node->feedbackUrl("editcopy", ACTION_FAILED, $recordset[0], $this->m_node->m_db->getErrorMsg());
          $this->m_node->redirect($location);
        }
        else
        {
          $this->m_node->m_db->commit();
          $location = session_url(dispatch_url($this->m_node->atknodetype(), "edit", array("atkselector" => $this->m_node->primaryKey($recordset[0]))), SESSION_REPLACE);
          $this->m_node->redirect($location);
        }
      }

      else
      {
        atkdebug("Geen records gevonden met selector: $selector");
        $this->m_node->redirect();
      }
    }
  }

?>