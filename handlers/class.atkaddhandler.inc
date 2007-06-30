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
   * Handler for the 'add' action of a node. It draws a page where the user
   * can enter a new record.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkAddHandler extends atkActionHandler
  {
    var $m_buttonsource = null;
    var $m_dialogSaveUrl = null;

    function atkAddHandler()
    {
      $this->atkActionHandler();
      $this->setReturnBehaviour(ATK_ACTION_BACK);
    }

    /**
     * The action handler.
     */
    function action_add()
    {
      $page = &$this->getPage();

      if (isset($this->m_partial) && $this->m_partial!="")
      {
        $result = $this->partial($this->m_partial);
        $page->addContent($result);
        return;
      }

      // we could get here because of a reject.
      $record = $this->getRejectInfo();

      $page->addContent($this->m_node->renderActionPage("add", $this->invoke("addPage", $record)));
    }

    /**
     * Creates an add page.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return String A String containing a box with an add form.
     */
    function addPage($record=NULL)
    {
      $result = $this->getAddPage($record);
      if ($result !== false)
        return $result;
    }

    function getAddPage($record=NULL)
    {
      // check if there are postvars set for filling the record, this
      // can happen when a new selection is made using the select handler etc.
      if ($record == NULL)
      {
        $record = $this->m_node->updateRecord('', NULL, NULL, true);
      }

      $this->registerExternalFiles();

      $params = $this->getAddParams($record);

      if ($params===false)
        return false;

      return $this->renderAddPage($params);
    }

    function setButtonSource(&$object)
    {
      $this->m_buttonsource = &$object;
    }

    function registerExternalFiles()
    {
      $page = &$this->getPage();
      $ui = &$this->getUi();
      $page->register_script(atkconfig('atkroot').'atk/javascript/prototype/prototype.js');
      $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");
      $page->register_loadscript("placeFocus();");
      $page->register_style($ui->stylePath("style.css"));
    }

    function getAddParams($record=NULL)
    {
      $node = &$this->m_node;
      $params = $node->getDefaultActionParams();
      $params["formstart"] = $this->getFormStart();

      $ui = &$node->getUi();
      if (is_object($ui))
      {
        $params["content"] = $this->getContent($record);
        $params["buttons"] = $this->getFormButtons($record);
        $params["formend"] = '</form>';

        return $params;
      }

      atkerror("ui object failure");
      return false;
    }

    function getFormStart()
    {
      $controller = &atkcontroller::getInstance();
      $controller->setNode($this->m_node);

      $node = &$this->m_node;

      $formIdentifier = ((isset($this->m_partial) && $this->m_partial!="")) ? "dialogform" : "entryform";
      $formstart ='<form id="'.$formIdentifier.'" name="'.$formIdentifier.'" enctype="multipart/form-data" action="'.$controller->getPhpFile().'?'.SID.'"'.
                     ' method="post" onsubmit="return globalSubmit(this)">';


      $formstart .= session_form(SESSION_NESTED, $this->getReturnBehaviour());
      $formstart .= $controller->getHiddenVarsString();

      if (isset($node->m_postvars['atkfilter']))
      {
        $formstart .= '<input type="hidden" name="atkfilter" value="'.$node->m_postvars['atkfilter'].'">';
      }

      return $formstart;
    }

    function getContent($record)
    {
      $node = &$this->m_node;

      // Utility.
      $edithandler = &$node->getHandler("edit");

      $forceList = (isset($node->m_postvars['atkfilter'])) ? decodeKeyValueSet($node->m_postvars['atkfilter']) : array();
      $form = $edithandler->editForm("add",$record,$forceList);

      return $node->tabulate("add", $form);
    }

    function getFormButtons($record=null)
    {
      if ($this->m_partial == 'dialog')
      {
        $controller = &atkController::getInstance();
        $result = array();
        $result[] = $controller->getDialogButton('save', null, $this->getDialogSaveUrl(), $this->getDialogSaveParams());
        $result[] = $controller->getDialogButton('cancel');
        return $result;
      }

      // If no custom button source is given, get the default atkController.
      if ($this->m_buttonsource===null)
      {
        $this->m_buttonsource = &$this->m_node;
      }

      return $this->m_buttonsource->getFormButtons("add", $record);
    }

    function renderAddPage($params)
    {
      $node = &$this->m_node;
      $ui = &$node->getUi();

      if (is_object($ui))
      {
        $output = $ui->renderAction("add", $params);
        $this->addRenderBoxVar("title", $node->actionTitle('add'));
        $this->addRenderBoxVar("content", $output);
        
        if ($this->getRenderMode() == "dialog")
        {
          $total = $ui->renderDialog($this->m_renderBoxVars);
        }
        else 
        {
          $total = $ui->renderBox($this->m_renderBoxVars);
        }

        return $total;
      }
    }

    /**
     * Attribute handler.
     *
     * @param string $partial full partial
     */
    function partial_attribute($partial)
    {
      list($type, $attribute, $partial) = explode('.', $partial);        

      $attr = &$this->m_node->getAttribute($attribute);
      if ($attr == NULL)
      {
        atkerror("Unknown / invalid attribute '$attribute' for node '".$this->m_node->atkNodeType()."'");
        return '';
      }

      $result = $attr->partial($partial, 'add');
      return $result;
    }

    /**
     * Section state handler.
     */
    function partial_sectionstate()
    {
      atkimport("atk.session.atkstate");
      atkState::set(array("nodetype"=>$this->m_node->atknodetype(),"section"=>$this->m_postvars['atksectionname']),$this->m_postvars['atksectionstate']);
    }
    
    /**
     * Render add dialog.
     *
     * @param array $record
     * @return string html
     */
    function renderAddDialog($record=null)
    {
      $this->setRenderMode('dialog');
      $result = $this->m_node->renderActionPage("add", $this->invoke("addPage", $record));
      return $result;    
    }
    
    /**
     * Handle the dialog partial.
     *
     * @param String $mode
     * @return unknown
     */
    function partial_dialog()
    {
      return $this->renderAddDialog();
    }
    
    
    /**
     * Returns the dialog save URL.
     * 
     * @return string dialog save URL
     */
    function getDialogSaveUrl()
    {
      if ($this->m_dialogSaveUrl != null)
      {
        return $this->m_dialogSaveUrl;
      }
      else 
      {
        return partial_url($this->m_node->atkNodeType(), 'save', 'dialog');            
      }
    }
    
    /**
     * Returns the dialog save params. These are the same params that are part of the
     * dialog save url, but they will be appended at the end of the query string to
     * override any form variables with the same name!
     */
    function getDialogSaveParams()
    {
      $parts = parse_url($this->getDialogSaveUrl());
      $query = $parts['query'];
      $params = array();
      parse_str($query, $params);
      return $params;
    }
    
    /**
     * Override the default dialog save URL.
     *
     * @param string $url dialog save URL
     */
    function setDialogSaveUrl($url)
    {
      $this->m_dialogSaveUrl = $url;
    }     
  }
?>