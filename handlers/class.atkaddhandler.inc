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
    /**
     * The action handler.
     */
    function action_add()
    {
      $page = &$this->getPage();

      if (isset($this->m_partial) && $this->m_partial!="")
      {
        list($type, $attribute, $partial) = explode('.', $this->m_partial);
        if ($type == 'attribute')
        {
          $attr =& $this->m_node->getAttribute($attribute);
          if ($attr == NULL)
          {
            atkerror("Unknown / invalid attribute '$attribute' for node '".$this->m_node->atkNodeType()."'");
            return;
          }

          $result = $attr->partial($partial, 'add');
          $page->addContent($result);
          return;
        }
      }

      $page->addContent($this->m_node->renderActionPage("add", $this->invoke("addPage")));
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
      $node = &$this->m_node;
      $page = &$this->getPage();

      // Utility.
      $edithandler = &$node->getHandler("edit");

      // check if there are postvars set for filling the record, this
      // can happen when a new selection is made using the select handler etc.
      if ($record == NULL)
      {
        $record = $this->m_node->updateRecord('', NULL, NULL, true);
      }

      $page->register_script(atkconfig('atkroot').'atk/javascript/prototype/prototype.js');
      $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");
      $page->register_loadscript("placeFocus();");
      $node->addStyle("style.css");
      $controller = &atkcontroller::getInstance();
      $controller->setNode($this->m_node);

      $params = $node->getDefaultActionParams();

      $formstart ='<form id="entryform" name="entryform" enctype="multipart/form-data" action="'.$controller->getPhpFile().'?'.SID.'"'.
                     ' method="post" onsubmit="return globalSubmit(this)">';

      /* if we are on an admin page we would like to stay on the admin page after adding a new record
         if we are on level 0, we want to present the user with a new (empty!) addition form when (s)he gets back */
      if (atkLevel() == 0) $formstart.=session_form(SESSION_NESTED);
      else $formstart.=session_form();

      $formstart .= $controller->getHiddenVarsString();

      if (isset($node->m_postvars['atkfilter']))
      {
        $formstart .= '<input type="hidden" name="atkfilter" value="'.$node->m_postvars['atkfilter'].'">';
      }

      $params["formstart"] = $formstart;

      $ui = &$node->getUi();
      if (is_object($ui))
      {
        $forceList = array();
        if (isset($node->m_postvars['atkfilter']))
        {
          $forceList = decodeKeyValueSet($node->m_postvars['atkfilter']);
        }
        $form = $edithandler->editForm("add",$record,$forceList);

        $params["content"] = $node->tabulate("add", $form);

				$params["buttons"] = $this->m_node->getFormButtons("add", $record);
        $params["formend"] = '</form>';

        $output = $ui->renderAction("add", $params);

        $this->addRenderBoxVar("title", $node->actionTitle('add'));
        $this->addRenderBoxVar("content", $output);
        $total = $ui->renderBox($this->m_renderBoxVars);

        return $total;
      }
      else
      {
        atkerror("ui object failure");
      }
    }
  }

?>
