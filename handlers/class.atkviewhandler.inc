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
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  atkimport("atk.handlers.atkvieweditbase");

  /**
   * Handler class for a readonly view action. Similar to the edit handler,
   * but all fields are displayed readonly.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkViewHandler extends atkViewEditBase
  {
    /**
     * The action handler method.
     */
    function action_view($renderbox=true)
    {
      if (isset($this->m_partial) && $this->m_partial!="")
      {
        $this->partial($this->m_partial);
        return;
      }      

      $record = $this->getRecord();
      
      // allowed to view record?
      if (!$this->allowed($record))
      {
        $this->renderAccessDeniedPage();
        return;
      }

      $page = &$this->getPage();
      $this->notify("view", $record);
      $page->addContent($this->m_node->renderActionPage("admin", $this->invoke("viewPage", $record, $this->m_node, $renderbox)));
    }
    
    /**
     * Returns the view record.
     */
    function getRecord()
    {
      list($record) = $this->m_node->selectDb($this->m_postvars['atkselector'],"","",$this->m_node->m_viewExcludes,"","view");
      return $record;      
    }

    /**
     * Returns an htmlpage displaying all displayable attributes.
     * @param array $record The record to display.
     * @param atkNode $node The node for which a viewPage is displayed.
     * @return String The html page with a reaonly view of relevant fields.
     */
    function viewPage($record, $node, $renderbox=true)
    {
      $ui = &$this->getUi();
      $node->addStyle("style.css");

      if (is_object($ui))
      {
        $params = $node->getDefaultActionParams();
        $tab = $node->getActiveTab();
        $innerform = $this->viewForm($record, "view");

        $params["activeTab"] = $tab;

        $params["content"] = $node->tabulate("view", $innerform);

        $params["formstart"] = '<form name="entryform" action="'.getDispatchFile().'" method="get">'.session_form(SESSION_NESTED).
                               '<input type="hidden" name="atkselector" value="'.$node->primaryKey($record).'">';
        $params["buttons"]   = $node->getFormButtons("view", $record);
        $params["formend"] = '</form>';

        $output = $ui->renderAction("view", $params);

        if (!$renderbox)
        {
          return $output;
        }

        $vars = array("title"=>$node->actionTitle('view', $record), "content"=>$output);        
        
        if ($this->getRenderMode() == "dialog")
        {
          $total = $ui->renderDialog($vars);
        }
        else
        {
          $total = $ui->renderBox($vars);
        }

        return $total;
      }
      else
      {
        atkerror("ui object error");
      }
    }

    function viewForm($record, $mode="view")
    {
      $node = &$this->m_node;

      // get data, transform into form, return
      $data = $node->viewArray($mode, $record);

      // get active tab
      $tab = $node->getActiveTab();

      $fields = array();

      // For all attributes we use the display() function to display the
      // attributes current value. This may be overridden by supplying
      // an <attributename>_display function in the derived classes.
      for ($i = 0, $_i= count($data["fields"]); $i<$_i; $i++)
      {
        $field = &$data["fields"][$i];
        $tplfield = array();

        $classes = array();
        if ($field["sections"] == "*")
        {
          $classes[] = "alltabs";
        }
        else if ($field["html"] == "section")
        {
          // section should only have the tab section classes
          foreach ($field["tabs"] as $section)
            $classes[] = "section_".str_replace('.', '_', $section);
        }
        else if (is_array($field["sections"]))
        {
          foreach ($field["sections"] as $section)
            $classes[] = "section_".str_replace('.', '_', $section);
        }

        $tplfield["class"] = implode(" ", $classes);
        $tplfield["tab"] = $tplfield["class"]; // for backwards compatibility

        // visible sections, both the active sections and the tab names (attribute that are
        // part of the anonymous section of the tab)
        $visibleSections = array_merge($this->m_node->getActiveSections($tab, $mode), $this->m_node->getTabs($mode));

        // Todo fixme: initial_on_tab kan er uit, als er gewoon bij het opstarten al 1 keer showTab aangeroepen wordt (is netter dan aparte initial_on_tab check)
        // maar, let op, die showTab kan pas worden aangeroepen aan het begin.
        $tplfield["initial_on_tab"] =
          ($field["tabs"] == "*" || in_array($tab, $field["tabs"])) &&
          (!is_array($field["sections"]) || count(array_intersect($field['sections'], $visibleSections)) > 0);

        // Give the row an id if it doesn't have one yet
        if (!isset($field["id"]) || empty($field["id"]))
          $field['id'] = getUniqueID("anonymousattribrows");

        // ar_ stands voor 'attribrow'.
        $tplfield["rowid"] = "ar_" . $field['id']; // The id of the containing row

        /* check for separator */
        if ($field["html"] == "-" && $i > 0 && $data["fields"][$i-1]["html"] != "-")
        {
          $tplfield["line"] = "<hr>";
        }
        /* double separator, ignore */
        elseif ($field["html"] == "-")
        {
        }
        /* sections */
        elseif($field["html"] == "section")
        {
          $tplfield["line"] = $this->getSectionControl($field, $mode);
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
            }
          }
          else
          {
              $tplfield["label"]="AF_NO_LABEL";
          }

          // Make the attribute and node names available in the template.
          $tplfield['attribute'] = $field["attribute"]->fieldName();
          $tplfield['node'] = $field["attribute"]->m_ownerInstance->atkNodeType();

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

          $tplfield['id']=str_replace('.','_',$node->atknodetype().'_'.$field["id"]);

          $tplfield["full"] = $editsrc;
        }
        $fields[] = $tplfield; // make field available in numeric array
        $params[$field["name"]] = $tplfield; // make field available in associative array
      }
      $ui = &$this->getUi();

      $innerform = $ui->render($node->getTemplate("view", $record, $tab), array("fields"=>$fields));

      return $innerform;
    }
    

    function partial_dialog()
    {
      return $this->renderViewDialog();
    }

    /**
     * Render add dialog.
     *
     * @param array $record
     * @return string html
     */
    function renderViewDialog($record=null)
    {
      if ($record == null)
      {
        $record = $this->getRecord();
      }

      $this->setRenderMode('dialog');
      $result = $this->m_node->renderActionPage("view", $this->invoke("viewPage", $record, $this->m_node));
      return $result;
    }    
  }
?>