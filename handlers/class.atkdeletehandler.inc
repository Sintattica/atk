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
   * @copyright (c)2000-2004 Ivo Jansch
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Handler for the 'delete' action of a node. It asks the user for
   * confirmation and upon actual confirmation, deletes the record (and for
   * any attribute that has AF_CASCADE_DELETE set, deletes any detail
   * information (if any) by calling the attributes' delete() method.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkDeleteHandler extends atkActionHandler 
  {
    /** 
     * The action handler.
     */
    function action_delete()
    {    
      if (!empty($this->m_postvars['confirm']))
      {
        // Confirmation page was displayed and 'yes' was clicked
        if($this->m_node->deleteDb($this->m_postvars['atkselector']))
        {
          $this->m_node->m_db->commit();
          $location = $this->m_node->feedbackUrl("delete", ACTION_SUCCESS);
          $this->m_node->redirect($location);
        }
        else
        {
          $this->m_node->m_db->rollback();
          $location = $this->m_node->feedbackUrl("delete",ACTION_FAILED,$this->m_node->m_db->getErrorMsg());
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