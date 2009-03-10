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
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Handler for the 'tcopy' action of a node. It copies the selected
   * record, and then redirects back to the calling page.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkCopyHandler extends atkActionHandler 
  {
    /**
     * The action handler.
     * 
     * @param Bool $redirect
     */
    function action_copy($redirect=true)
    {
      $this->invoke("nodeCopy");
    }
    
    /**
     * Copies a record, based on parameters passed in the url.
     */
    function nodeCopy()
    {
      atkdebug("atkCopyHandler::nodeCopy()");
      $recordset = $this->m_node->selectDb($this->m_postvars['atkselector'],"","","","","copy");
      $db = &$this->m_node->getDb();
      if(count($recordset)>0)
      {
        // allowed to copy record?
        if (!$this->allowed($recordset[0]))
        {
          $this->renderAccessDeniedPage();
          return;
        }
        
        if(!$this->m_node->copyDb($recordset[0]))
        {
          atkdebug("atknode::action_copy() -> Error");          
          $db->rollback();
          $location = $this->m_node->feedbackUrl("save", ACTION_FAILED, $recordset[0], $db->getErrorMsg());
          atkdebug("atknode::action_copy() -> Redirect");
          $this->m_node->redirect($location);
        }
        else
        {
          $db->commit();
          $this->notify("copy", $recordset[0]);
          $this->clearCache();
        }
      }
      $this->m_node->redirect();
    }
  }

?>