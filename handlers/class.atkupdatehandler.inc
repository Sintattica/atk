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
   * @todo Add locking check for when an application calls an action_update on a locked node
   */
  class atkUpdatehandler extends atkActionHandler
  {
    var $m_dialogSaveUrl;

    /**
     * The action handler method.
     */
    function action_update()
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
     * Get the record for updating
     *
     * @return Array The record to update
     */
    function getRecord()
    {
      return $this->m_node->updateRecord();
    }

    /**
     * Called when the acces to this action was denied
     * for the current user.
     */
    function handleAccessDenied()
    {
      $this->renderAccessDeniedPage();
    }

    /**
     * Check if there is an error (this can be determined by the
     * variable atkerror in the record).
     *
     * @param array $record
     * @return Boolean True on error, false otherwise
     */
    function hasError($record)
    {
      $error = false;

      if (isset($record['atkerror']))
      {
        $error = count($record['atkerror']) > 0;
        foreach (array_keys($record) as $key)
          $error = $error || (is_array($record[$key]) && array_key_exists('atkerror', $record[$key]) && count($record[$key]['atkerror']) > 0);
      }

      return $error;
    }

    /**
     * Called when an error has occurred.
     *
     * @param array $record
     */
    function handleError($record)
    {
      $this->setRejectInfo($record);
      $location = session_url(dispatch_url($this->m_node->atknodetype(),"edit",array("atkselector"=>$this->m_node->primaryKey($record))),SESSION_REPLACE);
      $this->m_node->redirect($location);
    }

    /**
     * Called when an update error has occurred.
     *
     * @param atkDb $db
     * @param array $record
     */
    function handleUpdateError(&$db, $record)
    {
      $db->rollback();
      if($db->getErrorType()=="user")
      {
        triggerError($record, 'Error', $db->getErrorMsg(), '', '');
        $this->m_node->m_action="edit";
        global $ATK_VARS; $ATK_VARS["atkaction"]="edit";
        $page = &$this->getPage();
        $edithandler = $this->m_node->getHandler("edit");
        $page->addContent($edithandler->invoke("editPage", $record));
      }
      else
      {
        $location = $this->m_node->feedbackUrl("update",ACTION_FAILED, $record, $db->getErrorMsg());
        $this->m_node->redirect($location, $record);
      }
    }

    /**
     * Perform the update action
     *
     */
    function doUpdate()
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
     * Handle the update of a dialog.
     *
     * @param String $attrRefreshUrl
     */
    function handleUpdate($attrRefreshUrl=null)
    {
      $record = $this->getRecord();

      // allowed to update record?
      if (!$this->allowed($record))
      {
        $content = $this->renderAccessedDeniedDialog();
        $this->updateDialog($content);
        return;
      }

      // In the dialog we only support saving and closing
      $this->handleProcessDialog($record, $attrRefreshUrl);
    }

    /**
     * Handle action when user has clicked atksaveandclose in a dialog.
     *
     * @param array $record
     * @param String $attrRefreshUrl
     */
    function handleProcessDialog($record, $attrRefreshUrl=null)
    {
      $db = &$this->m_node->getDb();
      
      // empty the postvars because we don't want to use these
      $postvars = $this->getNode()->m_postvars;
      $this->getNode()->m_postvars = array();
      // load original record if needed
      $this->getNode()->trackChangesIfNeeded($record);
      // put the postvars back
      $this->getNode()->m_postvars = $postvars;
      
      if (!$this->m_node->executeTrigger("preUpdate", $record) ||
          !$this->m_node->validate($record, "update") ||
          !$this->m_node->updateDb($record))
      {

        if ($db->hasError() && $db->getErrorType() != "user")
        {
          triggerError($record, 'Error', $db->getErrorMsg(), '', '');
        }

        // Re-render the edit dialog.
        $this->m_node->m_action="edit";
        $edithandler = $this->m_node->getHandler("edit");
        global $ATK_VARS; $ATK_VARS["atkaction"]="edit";
        $page = &$this->getPage();
        if ($this->m_dialogSaveUrl != null)
          $edithandler->setDialogSaveUrl($this->m_dialogSaveUrl);
        $content = $edithandler->renderEditDialog($record);
        $this->updateDialog($content);
        return;
      }

      $this->handleUpdateSuccessDialog($db, $record, $attrRefreshUrl);
    }

    /**
     * Called when a record is succesfully updated.
     *
     * @todo refresh only the recordlist not the full page.
     * @todo document.location.href is problematic if you already clicked the save
     * action on a normal edit page. If you use the editdialog after that and you
     * save the dialog, the page will redirect to the index page of the application.
     *
     * @param atkDb $db
     * @param array $record
     * @param String $attrRefreshUrl
     */
    function handleUpdateSuccessDialog(&$db, $record, $attrRefreshUrl=null)
    {
      $db->commit();
      $this->notify("update", $record);
      $this->clearCache();

      atkimport("atk.ui.atkdialog");

      $page = &$this->getPage();

      atkimport("atk.ui.atkdialog");
      $script = atkDialog::getCloseCall();

      if ($attrRefreshUrl == null)
      {
        $script .= "document.location.href = document.location.href;";
      }
      else
      {
        $page->register_script(atkconfig('atkroot').'atk/javascript/class.atkattribute.js');
        $script .= "ATK.Attribute.refresh('".$attrRefreshUrl."');";
      }

      $page = &$this->getPage();
      $page->register_loadscript($script);
    }

    /**
     * Called when the user clicks cancel
     *
     * @param array $record
     */
    function handleCancel($record)
    {
      $location = $this->m_node->feedbackUrl("update", ACTION_CANCELLED, $record);
      $this->m_node->redirect($location);
    }

    /**
     * Called when a record is succesfully updated
     *
     * @param atkDb $db
     * @param array $record
     */
    function handleUpdateSuccess(&$db, $record)
    {
      $db->commit();
      $this->notify("update", $record);

      $this->clearCache();
      if (!isset($this->m_postvars['atknoclose']))
      {
        // 'save and close' was clicked
        $location = $this->m_node->feedbackUrl("update", ACTION_SUCCESS, $record, "");
      }
      else
      {
        // 'save' was clicked
        $location = session_url(dispatch_url($this->m_node->atknodetype(),"edit", array("atkselector"=>$this->m_node->primaryKey($record),"atktab"=>$this->m_node->getActiveTab())), SESSION_REPLACE);
      }
      $this->m_node->redirect($location);
    }


    /**
     * Handle action when user has clicked atknoclose, atksaveandclose or atkwizardaction
     *
     * @param array $record
     */
    function handleProcess($record)
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
        $this->handleError($record);
        return;
      }

      $db = &$this->m_node->getDb();
      if(!$this->m_node->updateDb($record))
      {
        $this->handleUpdateError($db, $record);
      }
      else
      {
        $this->handleUpdateSuccess($db, $record);
      }
    }

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
    * This method is obsolete. Do not use it. It is only here for backward
    * compatibility...
    * Render an editpage
    *
    * We have to make the entire system think we are in edit mode again.
    *
    * @param array $record
    * @deprecated
    */
    function renderEditPage(&$record)
    {
      $editpage = $this->getEditPage($record);
      $content = $this->m_node->renderActionPage("edit", $editpage);
      $page = &$this->getPage();
      $page->addContent($content);
    }
	 
    /**
    * This method is obsolete. Do not use it. It is only here for backwards
    * compatibility...
    *
    * @param Array $record
    * @return String HTML code for the edit page
    * @deprecated
    */
    function getEditPage(&$record)
    {
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

?>
