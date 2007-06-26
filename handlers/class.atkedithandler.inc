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

  atkimport("atk.handlers.atkvieweditbase");

  /**
   * Handler class for the edit action of a node. The handler draws a
   * generic edit form for the given node.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkEditHandler extends atkViewEditBase
  {
    /**
     * The action handler method.
     */
    function action_edit()
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

          $result = $attr->partial($partial, 'edit');
          $page->addContent($result);
          return;
        }
        elseif($type == 'sectionstate')
        {
        	atkimport("atk.session.atkstate");
        	atkState::set(array("nodetype"=>$this->m_node->atknodetype(),"section"=>$this->m_postvars['atksectionname']),$this->m_postvars['atksectionstate']);
        	die();
        }

      }

      list($record) = $this->m_node->selectDb(atkArrayNvl($this->m_postvars, 'atkselector', ""),"","","","","edit");

      // allowed to edit record?
      if (!$this->allowed($record))
      {
        $this->renderAccessDeniedPage();
        return;
      }

      // check if there are postvars set that overwrite the record contents, this can happen when a
      // new selection is made using the select handler etc.
      $fetchedRecord = $this->m_node->updateRecord('', NULL, NULL, true);
      if (is_array($record))
        $record = array_merge($record, $fetchedRecord);

      $this->notify("edit", $record);
      if ($this->m_node->hasFlag(NF_LOCK))
      {
        if ($this->m_node->m_lock->lock($this->m_node->primaryKey($record), $this->m_node->m_table))
        {
          $res = $this->invoke("editPage", $record, true);
        }
        else
        {
          $res = $this->m_node->lockPage();
        }
      }
      else
      {
        $res = $this->invoke("editPage", $record, false);
      }

      $page->addContent($this->m_node->renderActionPage("edit", $res));
    }

    /**
     * This method draws a generic edit-page for a given record.
     *
     * @param array $record The record to edit.
     * @param boolean $locked Indicates whether the record is locked by the
     *                        current user.
     * @return String The rendered page as a string.
     */
    function editPage($record, $locked=FALSE)
    {
      $node = &$this->m_node;
      $page = &$this->getPage();
      $page->register_script(atkconfig('atkroot').'atk/javascript/prototype/prototype.js');
      $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");
      $page->register_loadscript("placeFocus();");
      $page->register_script(atkconfig("atkroot")."atk/javascript/dhtml_formtools.js");
      $node->addStyle("style.css");

      $params = $node->getDefaultActionParams($locked);
      $controller = &atkcontroller::getInstance();
      $controller->setNode($this->m_node);

      $formstart = '<form id="entryform" name="entryform" enctype="multipart/form-data" action="'.$controller->getPhpFile().'?'.SID.'"'.
                                   ' method="post" onsubmit="return globalSubmit(this)">'.
                                   session_form();

      $formstart .= $controller->getHiddenVarsString();

      $params["formstart"] = $formstart;

      $ui = &$node->getUi();

      if (is_object($ui))
      {
        $forceList = array();
        if (isset($node->m_postvars['atkfilter']))
        {
          $forceList = decodeKeyValueSet($node->m_postvars['atkfilter']);
        }
        $suppressList = array();
        if (isset($node->m_postvars['atksuppress']))
        {
          $suppressList = $node->m_postvars['atksuppress'];
        }
        $form =$this->editForm("edit",$record,$forceList,$suppressList);

        $params["header"] = $this->invoke("editHeader");

        $params["content"] = $node->tabulate("edit", $form);

        $params["buttons"] = $node->getFormButtons("edit", $record);

        $output = $ui->renderAction("edit", $params);

        $this->addRenderBoxVar("title", $node->actionTitle('edit', $record));
        $this->addRenderBoxVar("content", $output);

        return $ui->renderBox($this->m_renderBoxVars);
      }
      else
      {
        atkerror("Failed to create ui object");
      }
    }
    
    /**
     * Create template field array for the given edit field.
     *
     * @param array  $fields all fields
     * @param int    $index  field index
     * @param string $mode   mode (add/edit)
     * @param string $tab    active tab
     * 
     * @return array template field
     */
    function createTplField(&$fields, $index, $mode, $tab)
    {
      $field = &$fields[$index];

	    // visible sections, both the active sections and the tab names (attribute that are
	    // part of the anonymous section of the tab)
	    $visibleSections = array_merge($this->m_node->getActiveSections($tab, $mode), $this->m_node->getTabs($mode));      
      
      $tplfield = array();

      $classes = isset($field['class']) ? explode(" ", $field['class']) : array();
      if ($field["sections"] == "*")
      {
        $classes[] = "alltabs";
      }
      else if ($field["html"] == "section")
      {
        // section should only have the tab section classes
        foreach ($field["tabs"] as $section)
          $classes[] = "section_".str_replace('.', '_', $section);
        if ($this->isSectionInitialHidden($field['name'], $fields))
          $classes[] = "atkAttrRowHidden";
      }
      else if (is_array($field["sections"]))
      {
        foreach ($field["sections"] as $section)
          $classes[] = "section_".str_replace('.', '_', $section);
      }

      if (isset($field["initial_hidden"]) && $field["initial_hidden"])
      {
        $classes[] = "atkAttrRowHidden";
      }

      $tplfield["class"] = implode(" ", $classes);
	    $tplfield["tab"] = $tplfield["class"]; // for backwards compatibility

	    // Todo fixme: initial_on_tab kan er uit, als er gewoon bij het opstarten al 1 keer showTab aangeroepen wordt (is netter dan aparte initial_on_tab check)
	    // maar, let op, die showTab kan pas worden aangeroepen aan het begin.
  		$tplfield["initial_on_tab"] =
  		   ($field["tabs"] == "*" || in_array($tab, $field["tabs"])) &&
         (!is_array($field["sections"]) || count(array_intersect($field['sections'], $visibleSections)) > 0);

      // ar_ stands voor 'attribrow'.
	    $tplfield["rowid"] = "ar_".($field['id']!=''?$field['id']:getUniqueID("anonymousattribrows")); // The id of the containing row

	    /* check for separator */
      if ($field["html"] == "-" && $index > 0 && $data["fields"][$index-1]["html"] != "-")
      {
        $tplfield["type"] = "line";
        $tplfield["line"] = "<hr>";
      }
      /* double separator, ignore */
      elseif ($field["html"] == "-")
      {
      }
      /* sections */
      elseif($field["html"] == "section")
      {
        $tplfield["type"] = "section";    
        list($tab, $section) = explode('.', $field["name"]);        
        $tplfield["section_name"] = "section_{$tab}_{$section}";
        $tplfield["line"] = $this->getSectionControl($field, $mode);
      }
      /* only full HTML */
      elseif (isset($field["line"]))
      {
        $tplfield["type"] = "custom";
        $tplfield["line"] = $field["line"];
      }
      /* edit field */
      else
      {
        $tplfield["type"] = "attribute";        
                
        if ($field["attribute"]->m_ownerInstance->getNumbering())
        {
          $this->_addNumbering($field, $tplfield, $index);
        }

        /* does the field have a label? */
        if ((isset($field["label"]) && $field["label"]!=="AF_NO_LABEL") || !isset($field["label"]))
        {
          if (!isset($field["label"]) || empty($field["label"]))
          {
            $tplfield["label"] = "";
          }
          else
          {
            $tplfield["label"] = $field["label"];
            if ($field["error"]) // TODO KEES
            {
              $tplfield["error"] = $field["error"];
            }
          }
        }
        else
        {
        	$tplfield["label"]="AF_NO_LABEL";
        }

        /* obligatory indicator */
        if ($field["obligatory"])
        {
          // load images
          $theme = &atkinstance("atk.ui.atktheme");
          $reqimg = '<img align="top" src="'.$theme->imgPath("required_field.gif").'" border="0"
                     alt="'.atktext("field_obligatory").'" title="'.atktext("field_obligatory").'">';
          
          $tplfield["label"];
          $tplfield["obligatory"] = $reqimg;
        }

        /* html source */
        $tplfield["widget"] = $field["html"];
        $editsrc = $field["html"];

        /* tooltip */
        $tooltip = $field["attribute"]->getToolTip();
        if($tooltip)
        {
          $tplfield["tooltip"] = $tooltip;
          $editsrc.=$tooltip."&nbsp;";
        }

        $tplfield['id']=str_replace('.','_',$this->m_node->atknodetype().'_'.$field["id"]);

        $tplfield["full"] = $editsrc;
      }      
      
      return $tplfield;
    }

    /**
     * Function returns a generic html form for editing a record.
     *
     * @param String $mode         The edit mode ("add" or "edit").
     * @param array $record        The record to edit.
     * @param array $forcelist     A key-value array used to preset certain
     *                             fields to a certain value.
     * @param array $suppresslist  An array of fields that will be hidden.
     * @param String $fieldprefix  If set, each form element is prefixed with
     *                             the specified prefix (used in embedded
     *                             forms)
     * @param boolean $ignoreTab   Ignore the tabs an attribute should be shown on.
     *
     * @return String the edit form as a string
     */
    function editForm($mode="add", $record = NULL, $forceList="", $suppressList="", $fieldprefix="",$template="",$ignoreTab=false)
    {
      $node = &$this->m_node;

      /* get data, transform into form, return */
      $data = $node->editArray($mode, $record, $forceList, $suppressList, $fieldprefix, $ignoreTab);
      // Format some things for use in tpl.
      /* check for errors and display them */
      $tab = $node->getActiveTab();
      $error_title = "";
      $pk_err_attrib = array();

      // Handle errors
      $errors = array();
      if (count($data['error']) > 0)
      {
        $error_title = '<b>'.atktext('error_formdataerror').'</b>';

        foreach ($data["error"] as $error)
        {
          if ($error['err'] == "error_primarykey_exists")
          {
            $pk_err_attrib[] = $error['attrib_name'];
          }
          else
          {
            $type = (empty($error["node"]) ? $node->m_type : $error["node"]);

            if (count($node->getTabs($node->m_action)) > 1 && $error["tab"])
              $error_tab = ' ('.atktext("error_tab").' '.'<a href="javascript:void(0)" onclick="showTab(\''.$error["tab"].'\'); return false;">'.$this->getTabLabel($node, $error["tab"]).'</a> )';
            else $error_tab = "";

            if (is_array($error['label']))
            {
              $label = implode(', ', $error['label']);
            }
            else if (!empty($error['label']))
            {
              $label = $error['label'];
            }
            else if (!is_array($error['attrib_name']))
            {
              $label = $error['label'];
            }
            else if (!is_array($error['attrib_name']))
            {
              $label = $node->text($error['attrib_name']);
            }
            else 
            {
              $label = array();
              foreach($error['attrib_name'] as $attrib)
              {
                $label[] = $node->text($attrib);
              }

              $label = implode(", ", $label);
            }

            $errors[] = array("msg"=>$error['msg'].$error_tab, "label"=>$label);
          }
        }
        if (count($pk_err_attrib)>0) // Make primary key error message
        {
          for($i=0;$i<count($pk_err_attrib); $i++)
          {
            $pk_err_msg .= atktext($pk_err_attrib[$i], $node->m_module);
            if (($i+1) < count($pk_err_attrib)) $pk_err_msg .= ", ";
          }
          $errors[] = array("label"=>atktext("error_primarykey_exists"),
                            "msg"=>$pk_err_msg);
        }

      }

      /* display the edit fields */
      $fields = array();
      $errorFields = array();
      for ($i = 0, $_i= count($data["fields"]); $i<$_i; $i++)
      {
        $field = &$data["fields"][$i];
        $tplfield = $this->createTplField($data["fields"], $i, $mode, $tab);
        $fields[] = $tplfield; // make field available in numeric array
        $params[$field["name"]] = $tplfield; // make field available in associative array
        
        if ($field['error'])
          $errorFields[] = $field['id'];
      }

      $ui = &$this->getUi();
      $page = &$this->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/formsubmit.js");

      // register fields that contain errornous values
      atkimport('atk.utils.atkjson');
      $page->register_scriptcode("var atkErrorFields = ".atkJSON::encode($errorFields).";");
      
      if (atkconfig('lose_changes_warning', true))
      {
        // If we are in the save or update action the user has added a nested record, has done
        // a selection using the select handler or generated an error, in either way we assume
        // the form has been changed, so we always warn the user when leaving the page.
        $isChanged = 'false';
        if ((isset($record['atkerror']) && count($record['atkerror']) > 0) || 
            (isset($this->m_node->m_postvars['__atkunloadhelper']) && $this->m_node->m_postvars['__atkunloadhelper']))
        {
          $isChanged = 'true';
        }
        
        $unloadText = addslashes($this->m_node->text('lose_changes_warning'));
        $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkunloadhelper.js");      
        $page->register_loadscript("new ATK.UnloadHelper('entryform', '{$unloadText}', {$isChanged});");
      }

      $result = "";

      foreach ($data["hide"] as $hidden)
      {
        $result.= $hidden;
      }

      $params["activeTab"] = $tab;
      $params["fields"] = $fields; // add all fields as an numeric array.
      $params["errortitle"] = $error_title;
      $params["errors"] = $errors; // Add the list of errors.
      if (!$template) $template = $node->getTemplate($mode, $record, $tab);
      $result.= $ui->render($template, $params);

	    return $result;
    }

    /**
    * Overrideable function to create a header for edit mode.
    * Similar to the admin header functionality.
    */
    function editHeader()
    {
      return "";
    }
  }
?>
