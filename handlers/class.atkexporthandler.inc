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
   * @copyright (c)2004 Ivo Jansch
   * @copyright (c)2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Handler for the 'import' action of a node. The import action is a
   * generic tool for importing CSV files into a table.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkExportHandler extends atkActionHandler
  {
    /**
     * The action handler.
     */
    function action_export()
    {
      global $ATK_VARS;

      //need to keep the postdata after a AF_LARGE selection in the allfield
      if(!isset($this->m_postvars["phase"]) && isset($ATK_VARS['atkformdata']))
        foreach($ATK_VARS['atkformdata'] as $key=>$value)
          $this->m_postvars[$key] = $value;

      //need to keep the selected item after an exporterror
      $phase = atkArrayNvl($this->m_postvars, "phase", "init");
      if (!in_array($phase, array("init", "process")))
        $phase = "init";

      switch ($phase)
      {
        case "init": $this->doInit(); break;
        case "process": $this->doProcess(); break;
      }
      return true;
    }

    /**
     * This function shows a form to configure the .csv
     */
    function doInit()
    {
      $content = $this->_getInitHtml();
      $page = &$this->getPage();
      $page->addContent($this->m_node->genericPage("export", $content));
      return true;
    }

    /**
     * Gets the HTML for the initial mode of the exporthandler
     * @return String The HTML for the screen
     */
    function _getInitHtml()
    {
      $action = dispatch_url($this->m_node->m_module.'.'.$this->m_node->m_type, "export");
      $content = atktext("export_config_explanation", "atk", $this->m_node->m_type);
      $content.= '<br><br><form name="entryform" enctype="multipart/form-data" action="'.$action.'" method="post">';
      $content.= session_form();
      $content.= '<input type="hidden" name="phase" value="process">';
      $content.= $this->_getOptions();

      $content.= '<br><br><input type="submit" value="'.atktext("export","atk").'"> ';
      $content.= atkButton(atktext("back","atk"), "", SESSION_BACK, true);

      $content.= '</form><br><br>';
      return $content;
    }

    /**
     * This function checks if there is enough information to export the date
     * else it wil shows a form to set how the file wil be exported
     */
    function doProcess()
    {
      return $this->doExport();
    }

    function _getOptions()
    {
      $content = '<table border="0" width="100%">';
      $content.= '<tr><td>'.atktext("delimiter", "atk").': </td><td><input type="text" size="2" name="delimiter" value=";"></td></tr>';
      $content.= '<tr><td>'.atktext("enclosure", "atk").': </td><td><input type="text" size="2" name="enclosure" value="&quot;"></td></tr>';
      $content.= '<tr><td valign="top">'.atktext("export_selectcolumns", "atk").': </td><td>'.$this->getAttributeSelect().'</td></tr>';
      $content.= '<tr><td>';
      $content.= atktext("export_generatetitlerow").': </td><td><input type="checkbox" name="generatetitlerow" class="atkcheckbox" value="1" />';
      $content.= '</td></tr>';
      $content.= '</table><br /><br />';
      return $content;
    }

    function getAttributeSelect()
    {
      $cols = 5;

      $atts = $this->getUsableAttributes();
      $content = "";

      foreach($atts as $tab => $group)
      {
        $content .= '<TABLE style="border: 1px solid #d8d8d8; width: 90%">';
        if($tab != 'default')
        {
          $content .= '<TR><TD colspan="'.$cols.'"><div style="background-color: #ccc; color: #00366E; font-weight: bold">'.atktext(array("tab_$tab", $tab),$this->m_node->m_module,$this->m_node->m_type).'</div></TD></TR><TR>';
        }
        $idx = 0;
        foreach($group as $item)
        {
          if($item['checked']) $checked = 'CHECKED';
          else $checked = '';

          $content.= '<TD align="left" width="'.(90/$cols).'%"><LABEL><INPUT type="checkbox" name="export_'.$item['name'].'" class="atkcheckbox" value="export_'.$item['name'].'" '.$checked.'>'.$item['text'].'</LABEL></TD>';

          $idx++;
          if($idx%$cols == 0)
          {
            $content .= '</TR><TR>';
          }
        }
        while($idx%$cols != 0){
          $content .= '<TD width="'.(90/$cols).'%">&nbsp;</TD>';
          $idx++;
        }
        $content .= "</TR></TABLE><BR/>";
      }
      return $content;
    }

    /**
     * Gives all the attributes that can be used for the import
     * @return array              the attributes
     */
    function getUsableAttributes()
    {
      $node = &$this->m_node;
      $atts = array();
      foreach($node->m_attribList as $key => $value){
        $flags = $value->m_flags;
        $class = get_class($value);
        if($value->hasFlag(AF_AUTOKEY) ||
           $value->hasFlag(AF_HIDE_VIEW) ||
           !(strpos($class, "dummy") === FALSE) ||
           !(strpos($class, "image") === FALSE))
          continue;
        if (method_exists($this->m_node, "getExportAttributeGroup"))
        {
          $group = $this->m_node->getExportAttributeGroup($value->m_name);
        }
        else
        {
          $group = $value->m_tabs[0];
        }
        if(in_array($group, $atts))
        {
          $atts[$group] = array();
        }
        $atts[$group][] = array('name' => $key, 'text' => $value->label(), 'checked' => !$value->hasFlag(AF_HIDE_LIST));
      }
      return $atts;
    }

    /**
     * the real import function
     * import the uploaded csv file for real
     */
    function doExport()
    {
      $enclosure = $this->m_postvars["enclosure"];
      $delimiter = $this->m_postvars["delimiter"];
      $source = $this->m_postvars;
      $list_includes = array();
      foreach($source as $name => $value)
      {
        $pos = strpos($name, 'export_');
        if (is_integer($pos) and $pos == 0)
          $list_includes[] = substr($name, strlen('export_'));
      }
      global $g_sessionData;
      $session_back = $g_sessionData["default"]["stack"][atkStackID()][atkLevel()-1];
      $atkorderby = $session_back['atkorderby'];

      $node = &$this->m_node;
      $node_bk = $node;
      $num_atts = count($node_bk->m_attribList);
      $atts = &$node_bk->m_attribList;

      foreach($atts as $name => $object)
      {
        $att = &$node_bk->getAttribute($name);
        if(in_array($name, $list_includes))
          $att->removeFlag(AF_HIDE_LIST);
        else
          $att->addFlag(AF_HIDE_LIST);
      }

      if (!is_array($actions))
      {
          $actions = $node->defaultActions("export");
      }
      $rl = &atknew("atk.recordlist.atkcustomrecordlist");
      $flags = ($node_bk->hasFlag(NF_MRA) ? RL_MRA : 0) | ($node_bk->hasFlag(NF_MRPA) ? RL_MRPA : 0) | ($node_bk->hasFlag(NF_LOCK) ? RL_LOCK : 0);
      $node_bk->m_postvars = $session_back;
      $recordset = $node_bk->selectDb($session_back['atkselector'], $atkorderby, "", "", $list_includes, "export");
      if (method_exists($this->m_node, "assignExportData")) $this->m_node->assignExportData($list_includes, $recordset);
      $recordset_new = array();

      foreach($recordset as $row)
      {
        foreach($row as $name => $value)
        {
          $value = str_replace("\r\n", "\\n", $value);
          $value = str_replace("\n", "\\n", $value);
          $value = str_replace("\t", "\\t", $value);
          $row[$name] = $value;
        }
        $recordset_new[] = $row;
      }

      $rl->render($node_bk, $recordset_new, "", $enclosure, $enclosure.$delimiter, "\r\n", 1, "", "", array('filename' => 'export_'.$node->m_type), "csv", $source['generatetitlerow'], true);

      return true;
    }
  }
?>
