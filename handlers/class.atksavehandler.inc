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
    var $m_dialogSaveUrl;

    /**
     * The action handler method
     */
    function action_save()
    {
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

        if ($error)
        {
          // something went wrong, back to where we came from
          $db = &$this->m_node->getDb();
          $db->rollback();
          return $this->goBack($record);
        }
        else
        {
          $db = &$this->m_node->getDb();

          if(!$this->m_node->addDb($record, true, "add"))
          {
            $this->handleAddError($record);
            return;
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

              $back = SESSION_BACK;
              if ($this->_getSkip()==1) $back = SESSION_REPLACE;

              $location = session_url(atkSelf().'?atknodetype='.$this->m_node->atknodetype().'&atkaction=edit&atkselector='.rawurlencode($this->m_node->primaryKey($record)).$extra, $back);
            }
            else if ($this->m_node->hasFlag(NF_ADDAFTERADD) && isset($this->m_postvars['atksaveandnext']))
            {
              $back = SESSION_BACK;
              if ($this->_getSkip()==1) $back = SESSION_DEFAULT;

              $filter = isset($this->m_node->m_postvars['atkfilter']) ? "&atkfilter=".rawurlencode($this->m_node->m_postvars['atkfilter']) : "";
              $location = session_url(atkSelf().'?atknodetype='.$this->m_node->atknodetype().'&atkaction=add'.$filter, $back);
            }
            else
            {
              // normal succesful save
              $location = $this->m_node->feedbackUrl("save", ACTION_SUCCESS, $record, "", $this->_getSkip());
            }
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

    function _getSkip()
    {
      if (isset($this->m_postvars["atkreturnbehaviour"]) &&
          $this->m_postvars["atkreturnbehaviour"]==ATK_ACTION_BACK)
      {
        return 2;
      }
      return 1;
    }

    function goBack($record)
    {
      $this->setRejectInfo($record);
      $this->m_node->redirect();
    }

    /**
     * Validate record.
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
  }
?>
