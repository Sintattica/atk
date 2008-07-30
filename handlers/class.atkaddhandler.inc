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
      if (!empty($this->m_partial))
      {
        $this->partial($this->m_partial);
        return;
      }

      // we could get here because of a reject.
      $record = $this->getRejectInfo();

      $page = &$this->getPage();
      $page->addContent($this->m_node->renderActionPage("add", $this->invoke("addPage", $record)));
    }

    /**
     * Creates an add page or null, of it cannot be created.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return String HTML A String containing a box with an add form.
     */
    function addPage($record=NULL)
    {
      $result = $this->getAddPage($record);
      if ($result !== false)
        return $result;
    }

    /**
     * Create an add page.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return String HTML A String containing a box with an add form.
     */
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

    /**
     * Set the source object where the add handler should
     * retrieve the formbuttons from. Default this is the owner
     * node.
     *
     * @param Object $object An object that implements the getFormButtons() method
     */
    function setButtonSource(&$object)
    {
      $this->m_buttonsource = &$object;
    }

    /**
     * Register external javascript and css files for the handler
     */
    function registerExternalFiles()
    {
      $page = &$this->getPage();
      $ui = &$this->getUi();
      $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");
      $page->register_loadscript("placeFocus();");
      $page->register_style($ui->stylePath("style.css"));
    }

    /**
     * Retrieve the parameters needed to render the Add form elements
     * in a smarty template.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return Array An array containing the elements used in a template for
     *               add pages.
     */
    function getAddParams($record=NULL)
    {
      $node = &$this->m_node;
      $ui = &$node->getUi();

      if (!is_object($ui))
      {
        atkerror("ui object failure");
        return false;
      }

      $params               = $node->getDefaultActionParams();
      $params["header"]     = $this->invoke("addHeader", $record);
      $params["formstart"]  = $this->getFormStart();
      $params["content"]    = $this->getContent($record);
      $params["buttons"]    = $this->getFormButtons($record);
      $params["formend"]    = $this->getFormEnd();
      return $params;
    }

    /**
     * Allows you to add an header above the addition form.
     *
     * @param array $record initial values
     *
     * @return string HTML or plain text that will be added above the add form.
     */
    public function addHeader($record=null)
    {
      return '';
    }

    /**
     * Retrieve the HTML code for the start of an HTML form and some
     * hidden variables needed by an add page.
     *
     * @return String HTML Form open tag and hidden variables.
     */
    function getFormStart()
    {
      $controller = &atkcontroller::getInstance();
      $controller->setNode($this->m_node);

      $node = &$this->m_node;

      $formIdentifier = ((isset($this->m_partial) && $this->m_partial!="")) ? "dialogform" : "entryform";
      $formstart ='<form id="'.$formIdentifier.'" name="'.$formIdentifier.'" enctype="multipart/form-data" action="'.$controller->getPhpFile().'?'.SID.'"'.
                     ' method="post" onsubmit="return globalSubmit(this)">';


      $formstart .= session_form(SESSION_NESTED, $this->getReturnBehaviour(), $node->getEditFieldPrefix());
      $formstart .= $controller->getHiddenVarsString();

      if (isset($node->m_postvars['atkfilter']))
      {
        $formstart .= '<input type="hidden" name="atkfilter" value="'.$node->m_postvars['atkfilter'].'">';
      }

      return $formstart;
    }

    /**
     * Retrieve the content of an add page.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return String HTML Content of the addpage.
     */
    function getContent($record)
    {
      $node = &$this->m_node;

      // Utility.
      $edithandler = &$node->getHandler("edit");

      $forceList = $this->invoke("createForceList");
      $form = $edithandler->editForm("add",$record,$forceList,'',$node->getEditFieldPrefix());

      return $node->tabulate("add", $form);
    }

    /**
     * Based on information provided in the url (atkfilter), this function creates an array with
     * field values that are used as the initial values of a record in an add page.
     *
     * @return Array Values of the newly created record.
     */
    function createForceList()
    {
      $node = &$this->m_node;
      $forceList = array();
      $filterList = (isset($node->m_postvars['atkfilter'])) ? decodeKeyValueSet($node->m_postvars['atkfilter']) : array();
      foreach ($filterList as $field => $value)
      {
         list($table, $column) = explode('.', $field);
         if ($column == null)
         {
           $forceList[$table] = $value;
         }
         else if ($table == $this->getNode()->getTable())
         {
           $forceList[$column] = $value;
         }
         else
         {
           $forceList[$table][$column] = $value;
         }
      }
      return $forceList;
    }

    /**
     * Get the end of the form.
     *
     * @return String HTML The forms' end
     */
    function getFormEnd()
    {
      return '</form>';
    }

    /**
     * Retrieve an array of form buttons that are rendered in the add page.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return Array a list of HTML buttons.
     */
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

    /**
     * Renders a complete add page including title and content
     *
     * @param Array $params Parameters needed in templates for the add page
     * @return String HTML the add page.
     */
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
     * Handler for partial actions on an add page
     *
     * @param string $partial full partial name
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

      return $attr->partial($partial, 'add');
    }

    /**
     * Partial handler for section state changes.
     */
    function partial_sectionstate()
    {
      atkimport("atk.session.atkstate");
      atkState::set(array("nodetype"=>$this->m_node->atknodetype(),"section"=>$this->m_postvars['atksectionname']),$this->m_postvars['atksectionstate']);
    }

    /**
     * Render add dialog.
     *
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return string html Add dialog
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
     * @return String HTML add dialog
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
     * @return Array paramaters
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
     * At default the save action of the current node ($this->m_node) is called
     * as a partial. Here you can set it to a different url.
     *
     * @param string $url dialog save URL
     */
    function setDialogSaveUrl($url)
    {
      $this->m_dialogSaveUrl = $url;
    }
  }
?>