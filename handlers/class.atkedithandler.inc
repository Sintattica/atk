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
   * Handler class for the edit action of a node. The handler draws a
   * generic edit form for the given node.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkEditHandler extends atkActionHandler
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
      }

      list($record) = $this->m_node->selectDb($this->m_postvars['atkselector'],"","","","","edit");
      
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
              $error_tab = ' ('.atktext("error_tab").' '.'<a href="javascript:showTab(\''.$error["tab"].'\');">'.atktext(array("tab_".$error["tab"], $error["tab"]),$node->m_module, $node->m_type).'</a> )';
            else $error_tab = "";

            if(!is_array($error['attrib_name']))
            {
              $label = atktext($error['attrib_name'], $node->m_module, $type);
            }
            else
            {
              $label = array();
              foreach($error['attrib_name'] as $attrib)
                $label[] = atktext($attrib, $node->m_module, $type);

              $label= implode(", ", $label);
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

      // Handle fields
      // load images
      $theme = &atkinstance("atk.ui.atktheme");
      $tipimg = $theme->imgPath("help.gif");
      $reqimg = '<img align="top" src="'.$theme->imgPath("required_field.gif").'" border="0"
                  alt="'.atktext("field_obligatory").'" title="'.atktext("field_obligatory").'">';

      /* display the edit fields */
      $fields = array();
      for ($i = 0, $_i= count($data["fields"]); $i<$_i; $i++)
      {
        $field = &$data["fields"][$i];
        $tplfield = array();

		    $tplfield["tab"] = ($field["tabs"][0]=="*"?"alltabs":implode("|", $field["tabs"]));

		    // Todo fixme: initial_on_tab kan er uit, als er gewoon bij het opstarten al 1 keer showTab aangeroepen wordt (is netter dan aparte initial_on_tab check)
		    // maar, let op, die showTab kan pas worden aangeroepen aan het begin.
		    $tplfield["initial_on_tab"] = ($tplfield["tab"]==$tab || $tplfield["tab"]=="alltabs");

		    // ar_ stands voor 'attribrow'.
		    $tplfield["rowid"] = "ar_".($field['id']!=''?$field['id']:getUniqueID("anonymousattribrows")); // The id of the containing row

		    /* check for separator */
        if ($field["html"] == "-" && $i > 0 && $data["fields"][$i-1]["html"] != "-")
        {
          $tplfield["line"] = "<hr>";
        }
        /* double separator, ignore */
        elseif ($field["html"] == "-")
        {
        }
        /* only full HTML */
        elseif (isset($field["line"]))
        {
          $tplfield["line"] = $field["line"];
        }
        /* edit field */
        else
        {
          if ($field["attribute"]->m_ownerInstance->getNumbering())
          {
            $this->_addNumbering($field, $tplfield, $i);
          }

          /* does the field have a label? */
          if ((isset($field["label"]) && $field["label"]!=="AF_NO_LABEL") || !isset($field["label"]))
          {
            if ($field["label"] == "")
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
            $tplfield["label"];
            $tplfield["obligatory"] = $reqimg;
          }

          /* html source */
          $tplfield["widget"] = $field["html"];
          $editsrc = $field["html"];

          /* tooltip */
          if (is_object($node->m_attribList[$field['name']]))
          {
            $module = $node->m_attribList[$field['name']]->getModule();
          }
          if (!$module) $module = "atk";
          $ttip = atktext($node->m_type."_".$field["name"]."_tooltip", $module, "", "", "", true);

          if ($ttip)
          {
            $onelinetip = preg_replace('/([\r\n])/e',"",$ttip);
            $tooltip = '<img align="top" src="'.$tipimg.'" border="0" alt="'.$onelinetip.'" onClick="javascript:alert(\''.str_replace("\n", '\n', addslashes($ttip)).
                                    '\')" onMouseOver="javascript:window.status=\''. addslashes($onelinetip).'\';">';
            $tplfield["tooltip"] = $tooltip;
            $editsrc.=$tooltip."&nbsp;";
          }

          $tplfield['id']=str_replace('.','_',$node->atknodetype().'_'.$field["id"]);

          $tplfield["full"] = $editsrc;
        }
        $fields[] = $tplfield; // make field available in numeric array
        $params[$field["name"]] = $tplfield; // make field available in associative array
      }

      $ui = &$this->getUi();
      $page = &$this->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/formsubmit.js");

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
     * Adds numbering to the label of a field
     * @access private
     * @param array $field    the currently handled attribute
     * @param array $tplfield the template data for the current attribute
     * @param int $i          the counter being used to loop the node for each attribute
     */
    function _addNumbering(&$field, &$tplfield, &$i)
    {
      static $number, $subnumber;

      if (!$number && !$subnumber) $number = $field["attribute"]->m_ownerInstance->getNumbering();
      if (!$subnumber)
      {
        if (strlen($number) == 1 || (floor($number) <= 9 && floor($number) >= -9 && floor($number) == $number))
        {
          $subnumber = $number;
          $number = null;
        }
        else
        {
          $subnumber = substr($number, strrpos($number, ".")+1);
          $number = substr($number, 0, strrpos($number, "."));
        }
      }

      if ($field["label"])
      {
        if ($number) $tplfield["label"]="$number.$subnumber. ";
        else $tplfield["label"]="$subnumber. ";
        $subnumber++;
      }
    }
  }
?>
