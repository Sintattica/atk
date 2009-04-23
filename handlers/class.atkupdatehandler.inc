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
   * @version $Revision: 6310 $
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
    public $m_dialogSaveUrl;

    /**
     * The action handler method.
     */
    public function action_update()
    {
      if (isset($this->m_partial) && $this->m_partial!="")
      {
        $this->partial($this->m_partial);
        return;
      }
      else
      {
        $this->doUpdate();
      }
    }

    /**
     * Perform the update action
     */
    public function doUpdate()
    {
      $record = $this->getRecord();

      // allowed to update record?
      if (!$this->allowed($record))
      {
        $this->handleAccessDenied();
        return;
      }

      if (isset($this->m_postvars['atknoclose'])||
          isset($this->m_postvars['atksaveandclose']) ||
          isset($this->m_postvars['atkwizardaction']))
      {
        $this->handleProcess($record);
      }
      else if (isset($this->m_postvars['atkcancel']))
      {
        $this->handleCancel($record);
      }
      else
      {
        // something other than one of the three buttons was pressed. Let's just refresh.
        $location = session_url(dispatch_url($this->m_node->atknodetype(),"edit", array("atkselector"=>$this->m_node->primaryKey($record),"atktab"=>$this->m_node->getActiveTab())), SESSION_REPLACE);
        $this->m_node->redirect($location);
      }
    }

    /**
     * Get the record for updating
     *
     * @return Array The record to update
     */
    public function getRecord()
    {
      return $this->m_node->updateRecord();
    }

    /**
     * Called when the acces to this action was denied
     * for the current user.
     */
    public function handleAccessDenied()
    {
      $this->renderAccessDeniedPage();
    }

    /**
     * Called when the user clicks cancel
     *
     * @param array $record
     */
    public function handleCancel($record)
    {
      $location = $this->m_node->feedbackUrl("update", ACTION_CANCELLED, $record);
      $this->m_node->redirect($location);
    }

    /**
     * Process a record (preUpdate/validate/store)
     *
     * @param array  $record                 Record to store
     * @param string $error_handler_method   Error handler method to call on current handler
     * @param string $success_handler_method Success handler method to call on current handler
     * @param array  $extra_params           Extra params to pass along to error/success handler methods
     * @return bool Wether the process succeeded in storing the record
     */
    public function handleProcess($record, $error_handler_method='handleError', $success_handler_method="redirectForUpdateResult", $extra_params=array())
    {
      // empty the postvars because we don't want to use these
      $postvars = $this->getNode()->m_postvars;
      $this->getNode()->m_postvars = array();
      // load original record if needed
      $this->getNode()->trackChangesIfNeeded($record);
      // put the postvars back
      $this->getNode()->m_postvars = $postvars;

      // just before we validate the record we call the preUpdate() to check if the record needs to be modified
      $this->m_node->executeTrigger("preUpdate", $record);

      $this->m_node->validate($record, "update");

      $error = $this->hasError($record);

      if ($error)
      {
        $this->$error_handler_method($record, $extra_params);
        return false;
      }

      $result = $this->updateRecord($record);
      $this->$success_handler_method($record, $result, $extra_params);
      return true;
    }

    /**
     * Check if there is an error (this can be determined by the
     * variable atkerror in the record).
     *
     * @param array $record Record to check for errors
     * @return bool Error detected?
     */
    public function hasError($record)
    {
      $error = false;
      if (isset($record['atkerror']))
      {
        $error = count($record['atkerror']) > 0;
        foreach (array_keys($record) as $key)
        {
          $error = $error || (is_array($record[$key]) && array_key_exists('atkerror', $record[$key]) && count($record[$key]['atkerror']) > 0);
        }
      }
      return $error;
    }

    /**
     * Update a record, determines wether to update it to the session or the database
     *
     * @param array $record Record to update
     * @return mixed Result of the update, true, false or string with error
     */
    private function updateRecord(&$record)
    {
      $atkstoretype = "";
      $sessionmanager = atkGetSessionManager();
      if ($sessionmanager) $atkstoretype = $sessionmanager->stackVar('atkstore');
      switch ($atkstoretype)
      {
        case 'session': $result = $this->updateRecordInSession($record); break;
        default:        $result = $this->updateRecordInDb($record); break;
      }
      return $result;
    }

    /**
     * Update a record in the database
     *
     * @param array $record Record to update
     * @return mixed Result of the update, true, false or string with error
     */
    private function updateRecordInDb(&$record)
    {
      $db = &$this->m_node->getDb();
      if ($this->m_node->updateDb($record))
      {
        $db->commit();
        $this->notify("update", $record);

        $this->clearCache();
        return true;
      }
      else
      {
        $db->rollback();
        if ($db->getErrorType()=="user")
        {
          triggerError($record, 'Error', $db->getErrorMsg(), '', '');
          return false;
        }
        return $db->getErrorMsg();
      }
    }

    /**
     * Update a record in the session
     *
     * @param array $record Record to update
     * @return mixed Result of the update, true or false
     */
    private function updateRecordInSession($record)
    {
      $selector = atkArrayNvl($this->m_postvars, 'atkselector','');
      return (atkinstance('atk.session.atksessionstore')->updateDataRowForSelector($selector, $record)!==false);
    }

    /**
     * Called when an error has occurred.
     *
     * @param array $record
     */
    public function handleError($record)
    {
      $this->setRejectInfo($record);
      $location = session_url(dispatch_url($this->m_node->atknodetype(),"edit",array("atkselector"=>$this->m_node->primaryKey($record))),SESSION_REPLACE);
      $this->m_node->redirect($location);
    }

    /**
     * Given a record and a result from the update redirects to a new location, if necessary
     *
     * @param array $record Record that was possibly saved
     * @param mixed $result Result of the update, true, false or string with error
     */
    private function redirectForUpdateResult($record, $result)
    {
      $location = false;
      if ($result===true)
      {
        if (!isset($this->m_postvars['atknoclose']))
        {
          // 'save and close' was clicked
          $location = $this->m_node->feedbackUrl("update", ACTION_SUCCESS, $record, "");
        }
        else
        {
          // 'save' was clicked
          $params = array("atkselector"=>$this->m_node->primaryKey($record),"atktab"=>$this->m_node->getActiveTab());
          $location = session_url(dispatch_url($this->m_node->atknodetype(), "edit", $params), SESSION_REPLACE);
        }
      }
      // We got an error back
      else if ($result!==false)
      {
        $location = $this->m_node->feedbackUrl("update", ACTION_FAILED, $record, $result);
      }
      else
      {
        $this->loadEditPageWithError($record);
      }
      if ($location) $this->m_node->redirect($location);
    }

    /**
     * Load up a page with the error for a given record that failed to update
     *
     * @param array $record Record to use with the error page
     */
    private function loadEditPageWithError($record)
    {
      global $ATK_VARS;
      $ATK_VARS["atkaction"] = "edit";
      $this->m_node->m_action = "edit";

      $edithandler = $this->m_node->getHandler("edit");
      $this->getPage()->addContent($edithandler->invoke("editPage", $record));
    }

    //=================== PARTIAL / DIALOG METHODS ===================\\

    /**
     * Handle the dialog partial
     *
     * @param String $mode The current mode
     */
    function partial_dialog($mode)
    {
      $this->handleUpdate();
    }

    /**
     * Override the dialog save url
     *
     * @param string $url dialog save URL
     */
    function setDialogSaveUrl($url)
    {
      $this->m_dialogSaveUrl = $url;
    }

    /**
     * Handle the update of a dialog.
     *
     * @param String $attrRefreshUrl
     */
    public function handleUpdate($attrRefreshUrl=null)
    {
      $record = $this->getRecord();

      // allowed to update record?
      if (!$this->allowed($record))
      {
        $content = $this->renderAccessedDeniedDialog();
        $this->updateDialog($content);
      }
      else
      {
        $this->handleProcess($record, 'loadSuccessDialog', 'loadEditDialogWithErrors', array('attribute_refresh_url'=>$attrRefreshUrl));
      }
    }

    /**
     * @todo refresh only the recordlist not the full page.
     * @todo document.location.href is problematic if you already clicked the save
     * action on a normal edit page. If you use the editdialog after that and you
     * save the dialog, the page will redirect to the index page of the application.
     *
     * @param unknown_type $attrRefreshUrl
     */
    private function loadSuccessDialog($record, $result, $extra_params)
    {
      atkimport("atk.ui.atkdialog");
      $script = atkDialog::getCloseCall();

      $page = $this->getPage();
      if ($extra_params['attribute_refresh_url'] == null)
      {
        $script .= "document.location.href = document.location.href;";
      }
      else
      {
        $page->register_script(atkconfig('atkroot').'atk/javascript/class.atkattribute.js');
        $script .= "ATK.Attribute.refresh('{$extra_params['attribute_refresh_url']}');";
      }

      $page->register_loadscript($script);
    }

    /**
     * Update the edit dialog for a failed update
     *
     * @param array $record Record that failed update
     */
    private function loadEditDialogWithErrors($record)
    {
      // Re-render the edit dialog.
      global $ATK_VARS;
      $ATK_VARS["atkaction"]="edit";
      $this->m_node->m_action="edit";

      $edithandler = $this->m_node->getHandler("edit");
      if ($this->m_dialogSaveUrl != null)
      {
        $edithandler->setDialogSaveUrl($this->m_dialogSaveUrl);
      }
      $this->updateDialog($edithandler->renderEditDialog($record));
    }

    //=================== DEPRECATED METHODS ===================\\

   /**
    * This method is obsolete. Do not use it. It is only here for backward
    * compatibility...
    * Render an editpage
    *
    * We have to make the entire system think we are in edit mode again.
    *
    * @deprecated
    *
    * @param array $record
    */
    function renderEditPage(&$record)
    {
      atkwarning("Use of deprecated method: ".__CLASS__.'->'.__METHOD__);

      $editpage = $this->getEditPage($record);
      $content = $this->m_node->renderActionPage("edit", $editpage);
      $page = &$this->getPage();
      $page->addContent($content);
    }

   /**
    * This method is obsolete. Do not use it. It is only here for backwards
    * compatibility...
    *
    * @deprecated
    *
    * @param Array $record
    * @return String HTML code for the edit page
    */
    public function getEditPage(&$record)
    {
      atkwarning("Use of deprecated method: ".__CLASS__.'->'.__METHOD__);
      //  $this->m_action="edit";
      //update succesful, pk value might be changed so update m_orgkey
      $record["atkprimkey"] = $this->m_node->primaryKey($record);

      $locked = ($this->m_node->hasFlag(NF_LOCK));

      //$this->setOrgKeyValue($record);
      $this->m_node->m_action = "edit";
      // we have to make the entire system think we are in edit mode again.
      global $ATK_VARS; $ATK_VARS["atkaction"]="edit";
      $edithandler = $this->m_node->getHandler("edit");
      return $edithandler->invoke("editPage", $record, $locked);
    }
  }