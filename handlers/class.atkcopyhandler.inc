<?php

  class atkCopyHandler extends atkActionHandler 
  {
    function action_copy()
    {
      atkdebug("atkCopyHandler::action_copy()");
      $recordset = $this->m_node->selectDb($this->m_postvars['atkselector'],"","","","","copy");
      if(count($recordset)>0)
      {
        if(!$this->m_node->copyDb($recordset[0]))
        {
          atkdebug("atknode::action_copy() -> Error");
          $this->m_node->m_db->rollback();
          $location = $this->m_node->feedbackUrl("save", ACTION_FAILED, $recordset[0], $this->m_node->m_db->getErrorMsg());
          atkdebug("atknode::action_copy() -> Redirect");
          $this->m_node->redirect($location);
        }
        else
        {
          $this->node->m_db->commit();
        }
      }
      
      $this->m_node->redirect();
    }
  }

?>