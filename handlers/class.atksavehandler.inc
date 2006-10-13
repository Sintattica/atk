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
   * Handler class for the save action of a node. The action saves a
   * new record to the database. The data is retrieved from the postvars.
   * This is the action that follows an 'add' action. The 'add' action
   * draws the add form, the 'save' action saves the data to the database.
   * Validation of the record is performed before storage. If validation
   * fails, the add handler is invoked again.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkSaveHandler extends atkActionHandler
  {
    /**
     * The action handler method
     */
    function action_save()
    {
      $record = $this->m_node->updateRecord();

      if (!isset($this->m_postvars['atkcancel']))
      {

        // just before we validate the record we call the preAdd() to check if the record needs to be modified
        $this->m_node->executeTrigger("preAdd", $record, "add");

        $this->m_node->validate($record, "add");


        if(!isset($record['atkerror']))
          $record['atkerror'] = array();

        $error = count($record['atkerror']) > 0;

        foreach (array_keys($record) as $key)
        {
          $error = $error || (is_array($record[$key]) && array_key_exists('atkerror', $record[$key]) && count($record[$key]['atkerror']) > 0);
        }

        // if an error occured, or we got back from a higher level we present the user
        // with the add form again (instead of saving the data to the database)
        if ($error || atkPrevLevel() > atkLevel())
        {
          $this->m_action="add";
          $this->m_node->m_action = "add";
          $page = &$this->getPage();
          $addhandler = &$this->m_node->getHandler("add");          
          $page->addContent($this->m_node->renderActionPage("add", $addhandler->invoke("addPage", $record)));
        }
        else
        {
          $db = &$this->m_node->getDb();

          if(!$this->m_node->addDb($record, true, "add"))
          {
            $db->rollback();
            if($db->getErrorType()=="user")
            {
              triggerError($record, 'Error', $db->getErrorMsg(), '', '');
              $this->m_action="add";
              $this->m_node->m_action = "add";
              $page = &$this->getPage();
              $addhandler = &$this->m_node->getHandler("add");
              $page->addContent($addhandler->invoke("addPage", $record));
            }
            else
            {
              $location = $this->m_node->feedbackUrl("save", ACTION_FAILED, $record, $db->getErrorMsg());
              $this->m_node->redirect($location);
            }
          }
          else
          {
            $db->commit();
            $this->notify("save", $record);
            $this->clearCache();

            $location="";
            if ($this->m_node->hasFlag(NF_EDITAFTERADD))
            {
              // forward atkpkret for newly added records
              $extra = "";
              if (isset($this->m_postvars["atkpkret"]))
                $extra = "&atkpkret=".rawurlencode($this->m_postvars["atkpkret"]);              
                
              $location = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->m_node->atknodetype().'&atkaction=edit&atkselector='.rawurlencode($this->m_node->primaryKey($record)).$extra, SESSION_REPLACE);
            }
            else if ($this->m_node->hasFlag(NF_ADDAFTERADD) && isset($this->m_postvars['atksaveandnext']))
            {
              $filter = isset($this->m_node->m_postvars['atkfilter']) ? "&atkfilter=".rawurlencode($this->m_node->m_postvars['atkfilter']) : "";
              $location = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->m_node->atknodetype().'&atkaction=add'.$filter, SESSION_DEFAULT);
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