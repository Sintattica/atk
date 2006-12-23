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

  /**
   * Handler class for a readonly view action. Similar to the edit handler,
   * but all fields are displayed readonly.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkViewHandler extends atkActionHandler
  {
    /**
     * The action handler method.
     */
    function action_view($renderbox=true)
    {
      atkdebug("Action view");
      $recordset = $this->m_node->selectDb($this->m_postvars['atkselector'],"","",$this->m_node->m_viewExcludes,"","view");

      $page = &$this->getPage();

      $page->addContent($this->m_node->renderActionPage("admin", $this->invoke("viewPage", $recordset[0], $this->m_node, $renderbox)));
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

        $fields = array();

        // get active tab
        $tab = $node->getActiveTab();

        // For all attributes we use the display() function to display the
        // attributes current value. This may be overridden by supplying
        // an <attributename>_display function in the derived classes.
        foreach (array_keys($node->m_attribIndexList) as $i)
        {
          $tplfield = array();
          $p_attrib = &$node->m_attribList[$node->m_attribIndexList[$i]["name"]];
          if (!$p_attrib->hasFlag(AF_HIDE_VIEW))
          {

            $field_name = $node->m_attribIndexList[$i]["name"];

            $tplfield["tab"] = $p_attrib->m_tabs[0];
            $tplfield["initial_on_tab"] = ($tplfield["tab"]==$tab);

            // An <attributename>_display function may be provided in a derived
            // class to display an attribute. If it exists we will use that method
            // else we will just use the attribute's display method.
            $funcname = $p_attrib->m_name."_display";
            if (method_exists($node, $funcname)) $editsrc = $node->$funcname($record, "view");
            else $editsrc=$p_attrib->display($record, "view");

            /* tooltip */
            $module = $node->m_attribList[$field_name]->getModule();
            if (!$module) $module = "atk";
            $ttip = atktext($node->m_type."_".$field_name."_tooltip", $module, "", "", "", true);

            if ($ttip)
            {
              $theme = &atkinstance("atk.ui.atktheme");
              $tipimg = $theme->imgPath("help.gif");

              $onelinetip = preg_replace('/([\r\n])/e',"",$ttip);
              $tooltip = '<img align="top" src="'.$tipimg.'" border="0" alt="'.$onelinetip.'" onClick="javascript:alert(\''.str_replace("\n", '\n', addslashes($ttip)).
                                      '\')" onMouseOver="javascript:window.status=\''. addslashes($onelinetip).'\';">';
              $tplfield["tooltip"] = $tooltip;
              $editsrc.=$tooltip."&nbsp;";
            }

            $tplfield["full"] = $editsrc;
            $tplfield["widget"] = $editsrc; // in view mode, widget and full are equal
            $tplfield["rowid"] = "ar_".$field_name; // The id of the containing row

            // The Label of the attribute (can be suppressed with AF_NOLABEL or AF_BLANKLABEL)
            // For each attribute, a txt_<attributename> must be provided in the language files.
            if ($p_attrib->hasFlag(AF_NOLABEL)==false)
            {
              if ($p_attrib->hasFlag(AF_BLANKLABEL))
              {
                $tplfield["label"] = "";
              }
              else
              {
                $tplfield["label"] = $p_attrib->label($record);
              }
            }
            else
            {
              // Make the rest fill up the entire line
              $tplfield["label"] = "";
              $tplfield["line"] = $tplfield["full"];
            }

            $fields[] = $tplfield;

          }
        }
        $innerform = $ui->render($node->getTemplate("view", $record, $tab), array("fields"=>$fields));

        $params["activeTab"] = $tab;

        $params["content"] = $node->tabulate("view", $innerform);

        $params["formstart"] = '<form action="dispatch.php" method="get">'.session_form(SESSION_NESTED).
                               '<input type="hidden" name="atkselector" value="'.$node->primaryKey($record).'">';
        $params["buttons"]   = $node->getFormButtons("view", $record);
        $params["formend"] = '</form>';

        $output = $ui->renderAction("view", $params);

        if (!$renderbox)
        {
          return $output;
        }

        $total = $ui->renderBox(array("title"=>$node->actionTitle('view', $record),
                                      "content"=>$output));
        return $total;
      }
      else
      {
        atkerror("ui object error");
      }
    }
  }

?>