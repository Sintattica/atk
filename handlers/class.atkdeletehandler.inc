<?php

  class atkDeleteHandler extends atkActionHandler 
  {
    function action_delete()
    {    
      if (!empty($this->m_postvars['confirm']))
      {
        global $g_db;
        // Confirmation page was displayed and 'yes' was clicked
        if($this->m_node->deleteDb($this->m_postvars['atkselector']))
        {
          $g_db->commit();
          $location = $this->m_node->feedbackUrl("delete", ACTION_SUCCESS);
          $this->m_node->redirect($location);
        }
        else
        {
          $g_db->rollback();
          $location = $this->m_node->feedbackUrl("delete",ACTION_FAILED,$g_db->getErrorMsg());
          $this->m_node->redirect($location);
        }
      }
      elseif (empty($this->m_node->m_postvars['cancel']))
      {
        $locked = FALSE;
        if ($this->m_node->hasFlag(NF_LOCK))
        {
          $locked = TRUE;
          if (is_array($this->m_postvars['atkselector']))
          {
            foreach ($this->m_postvars['atkselector'] as $selector)
             if (!$this->m_node->m_lock->lock($selector, $this->m_node->m_table)) $locked = FALSE;
          }
          elseif (!$this->m_node->m_lock->lock($this->m_postvars['atkselector'], $this->m_node->m_table)) $locked = FALSE;

          if (!$locked)
          {          
            $page = &$this->getPage();
            $page->addContent($this->m_node->lockPage());
            return;
          }
        }

        // Confirmation page was not displayed
        $page = &$this->getPage();        
        $page->addContent($this->m_node->confirmAction($this->m_postvars['atkselector'], "delete", $locked, TRUE));        
      }
      else
      {
        // Confirmation page was displayed and 'no' was clicked
        $location = $this->m_node->feedbackUrl("delete", ACTION_CANCELLED);
        $this->m_node->redirect($location);
      }      
    }
  }

?>