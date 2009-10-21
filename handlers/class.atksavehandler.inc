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
    var $m_dialogSaveUrl;

    /**
     * URL that should be used to redirect to after a successful saved record.
     *
     * @var string
     */
    private $m_successURL;

    /**
     * The action handler method
     */
    function action_save()
    {
      // clear old reject info
      $this->setRejectInfo(null);

      $page = &$this->getPage();

      if (isset($this->m_partial) && !empty($this->m_partial))
      {
        $this->partial($this->m_partial);
        return;
      }
      else
      {
        $this->doSave();
      }
    }

    /**
     * Save record.
     */
    function doSave()
    {
      $record = $this->m_node->updateRecord();

      // allowed to save record?
      if (!$this->allowed($record))
      {
        $this->renderAccessDeniedPage();
        return;
      }

      if (!isset($this->m_postvars['atkcancel']))
      {
        // just before we validate the record we call the preAdd() to check if the record needs to be modified
        if (!$this->m_node->executeTrigger("preAdd", $record, "add"))
        {
          $this->handleAddError($record);
          return;
        }

        $this->validate($record);

        if(!isset($record['atkerror']))
          $record['atkerror'] = array();

        $error = count($record['atkerror']) > 0;

        $db = $this->m_node->getDb();
        if ($error)
        {
          // something went wrong, back to where we came from
          $db->rollback();
          return $this->goBack($record);
        }
        else
        {
          if (!$this->storeRecord($record))
          {
            $this->handleAddError($record);
            return;
          }
          else
          {
            $location = $this->getSuccessReturnURL($record);
            $this->m_node->redirect($location, $record);
          }
        }
      }
      else
      {
        // Cancel was pressed
        $location = $this->m_node->feedbackUrl("save", ACTION_CANCELLED, $record, "", $this->_getSkip());
        $this->m_node->redirect($location);
      }
    }

    /**
     * Get the URL to redirect to after successfully saving a record.
     *
     * @param array $record Saved record
     * @return string Location to redirect to
     */
    private function getSuccessReturnURL($record)
    {
      if ($this->m_successURL)
      {
        return $this->m_successURL;
      }
      if ($this->m_node->hasFlag(NF_EDITAFTERADD) && $this->m_node->allowed('edit'))
      {
        return $this->getEditAfterAddURL($record);
      }
      else if ($this->m_node->hasFlag(NF_ADDAFTERADD) && isset($this->m_postvars['atksaveandnext']))
      {
        return $this->getAddAfterAddURL();
      }
      else
      {
        // normal succesful save
        return $this->m_node->feedbackUrl("save", ACTION_SUCCESS, $record, "", $this->_getSkip());
      }
    }

    /**
     * Given a saved record, return the URL that should be redirected to
     * for 'edit after add'.
     *
     * @param array $record
     * @return string
     */
    private function getEditAfterAddURL(array $record)
    {
      // forward atkpkret for newly added records
        $url_params = array();
        if (isset($this->m_postvars["atkpkret"]))
        {
          $url_params['atkpkret'] = $this->m_postvars["atkpkret"];
        }
        $url_params['atkselector'] = $this->m_node->primaryKey($record);

        if ($this->_getSkip()==1) $sessionstatus = SESSION_REPLACE;
        else                      $sessionstatus = SESSION_BACK;

        return session_url(dispatch_url($this->m_node->atkNodeType(), 'edit'), $sessionstatus);
    }

    /**
     * Return the URL that should be redirected to for 'add after add'.
     *
     * @return string
     */
    private function getAddAfterAddURL()
    {
      $url_params = array();
      if (isset($this->m_node->m_postvars['atkfilter']))
      {
        $url_params['atkfilter'] = $this->m_node->m_postvars['atkfilter'];
      }

      if ($this->_getSkip()==1) $sessionstatus = SESSION_DEFAULT;
      else                      $sessionstatus = SESSION_BACK;

      return session_url(dispatch_url($this->m_node->atkNodeType(), 'add', $url_params), $sessionstatus);
    }

    /**
     * Store a record, either in the database or in the session
     *
     * @param array $record Record to store
     * @return bool Successfull save?
     */
    private function storeRecord(&$record)
    {
      $atkstoretype = "";
      $sessionmanager = atkGetSessionManager();
      if ($sessionmanager) $atkstoretype = $sessionmanager->stackVar('atkstore');
      switch ($atkstoretype)
      {
        case 'session': return $this->storeRecordInSession($record);
        default:        return $this->storeRecordInDb($record);
      }
    }

    /**
     * Store a record in the session
     *
     * @param array $record Record to store in the session
     * @return bool Successfull save?
     */
    private function storeRecordInSession(&$record)
    {
      atkdebug("STORING RECORD IN SESSION");
      $result = atkinstance('atk.session.atksessionstore')->addDataRow($record, $this->m_node->primaryKeyField());
      return ($result!==false);
    }

    /**
     * Store a record in the database
     *
     * @param array $record Record to store in the database
     * @return bool Successfull save?
     */
    private function storeRecordInDb(&$record)
    {
      if (!$this->m_node->addDb($record, true, "add")) return false;

      $this->m_node->getDb()->commit();
      $this->notify("save", $record);
      $this->clearCache();

      return true;
    }

    /**
     * Handle error in preAdd/addDb.
     *
     * @param array $record
     */
    function handleAddError($record)
    {
      // Do a rollback on an error
      $db = &$this->m_node->getDb();
      $db->rollback();

      if($db->getErrorType()=="user")
      {
        triggerError($record, 'Error', $db->getErrorMsg(), '', '');

        // still an error, back to where we came from
        $this->goBack($record);
      }
      else
      {
        $location = $this->m_node->feedbackUrl("save", ACTION_FAILED, $record, $db->getErrorMsg());
        $this->m_node->redirect($location);
      }
    }

    /**
     * Get the number of levels to skip
     *
     * @return Integer The number of levels to skip
     */
    function _getSkip()
    {
      if (isset($this->m_postvars["atkreturnbehaviour"]) &&
          $this->m_postvars["atkreturnbehaviour"]==ATK_ACTION_BACK)
      {
        return 2;
      }
      return 1;
    }

    /**
     * Go back to the add page
     *
     * @param Array $record The record with reject info
     */
    function goBack($record)
    {
      $this->setRejectInfo($record);
      $this->m_node->redirect();
    }

    /**
     * Validate record.
     *
     * @param Array &$record The record to validate
     */
    function validate(&$record)
    {
      $error = (!$this->m_node->validate($record, "add"));

      if(!isset($record['atkerror']))
        $record['atkerror'] = array();

      $error = $error || count($record['atkerror']) > 0;

      foreach (array_keys($record) as $key)
      {
        $error = $error || (is_array($record[$key]) && array_key_exists('atkerror', $record[$key]) && count($record[$key]['atkerror']) > 0);
      }

      return !$error;
    }

    /**
     * Handle save of dialog.
     *
     * @param string $attrRefreshUrl  the attribute refresh url if not specified
     *                                the entire page is refreshed
     */
    function handleSave($attrRefreshUrl='')
    {
      $db = &$this->m_node->getDb();
      $record = $this->m_node->updateRecord();

      // allowed to save record?
      if (!$this->allowed($record))
      {
        $content = $this->renderAccessDeniedDialog();
        $this->updateDialog($content);
        return;
      }

      // just before we validate the record we call the preAdd() to check if the record needs to be modified
      // if an error occurs in the preAdd or the validate we have to handle it properly
      if (!$this->m_node->executeTrigger("preAdd", $record, "add") ||
          !$this->validate($record) ||
          !$this->m_node->addDb($record, true, "add"))
      {
        // an error occured, rollback database
        $db->rollback();

        if ($db->hasError() && $db->getErrorType() != "user")
        {
          triggerError($record, null, '', $db->getErrorMsg());
        }

        // re-render add dialog
        $handler = &$this->m_node->getHandler('add');
        $handler->m_partial = 'dialog';
        $handler->m_postvars = $this->m_postvars;
        if ($this->m_dialogSaveUrl != null)
          $handler->setDialogSaveUrl($this->m_dialogSaveUrl);
        $content = $handler->renderAddDialog($record);
        $this->updateDialog($content);
        return;
      }

      // addition succesfull, commit changes and close the dialog
      $db->commit();
      $this->notify("save", $record);
      $this->clearCache();

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

      $page->register_loadscript($script);
    }


    /**
     * The handler for the dialog partial call.
     *
     * @return html
     */
    function partial_dialog()
    {
      $this->handleSave();
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
     * Set the URL to go to after a succesful save.
     *
     * If you want to get the ID of the newly saved record,
     * then be sure 'atkpkret' is set in the postvars
     * to the variable name you want to get a selector in.
     *
     * @param string $url
     * @return atkSaveHandler
     */
    public function setSuccessURL($url)
    {
      $this->m_successURL = $url;
      return $this;
    }
  }
?>
