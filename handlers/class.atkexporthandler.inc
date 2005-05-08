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
      $phase = ($this->m_postvars["phase"]!=""?$this->m_postvars["phase"]:"init");

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
      $content.=$this->_getInitHtml();
      $page = &$this->getPage();
      $page->addContent($this->m_node->genericPage(atktext("export", "atk"), $content));
      return true;
    }
    
    /**
     * Gets the HTML for the initial mode of the exporthandler
     * @return String The HTML for the screen
     */
    function _getInitHtml()
    {
      global $HTTP_SERVER_VARS;
      $content = atktext("export_config_explanation", "atk", $this->m_node->m_type);
      $content.= '<br><br><form name="entryform" enctype="multipart/form-data" action="'.$HTTP_SERVER_VARS['REQUEST_URI'].'?'.SID.'"'.
                                  ' method="post">'.session_form();
      $content.= '<input type="hidden" name="phase" value="process">';
      $content.= $this->_getOptions();
            
      $content.= '<br><br><input type="submit" value="'.atktext("export_upload","atk").'"> ';
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
      return $this->doExport($filename);
    }

    
    
    /**
     * Returns the HTML header for the 'analyse' mode of the import handler
     * @param String $fileid      The 'id' (name) of the file we are importing
     * @param String $columncount The number of columns we have
     * @param String $delimiter   The delimiter in the file
     * @param String $enclosure   The enclosure in the file
     * @return String The HTML header
     */
    function _getOptions()
    {
      $content.= '<table border="0">';
      $content.= '<tr><td>'.text("delimiter").': </td><td><input type="text" size="2" name="delimiter" value=";"></td></tr>';
      $content.= '<tr><td>'.text("enclosure").': </td><td><input type="text" size="2" name="enclosure" value="&quot;"></td></tr>';
      $content.= '<tr><td>'.atktext("export_selectcolumns").': </td><td>'.$this->getAttributeSelect().'</td></tr>'; 
      $content.= '<tr><td>';
      $content.= atktext("export_generatetitlerow").': </td><td><input type="checkbox" name="generatetitlerow" value="1" />';
      $content.= '</td></tr>';
      $content.= '</table><br /><br />';
      return $content;
    }
    
    function getAttributeSelect()
    {
      return "";
    }
   
    /**
     * Gives all the attributes that can be used for the import
     * @param $obligatoryOnly     if false then give all attributes, if true then give only the obligatory ones
     * @return array              the attributes
     */
    function getUsableAttributes($obligatoryOnly=false)
    {
      $attrs = array();
      foreach (array_keys($this->m_node->m_attribList) as $attribname)
      {
        $attrib = &$this->m_node->getAttribute($attribname);

        if($this->integrateAttribute($attrib))
        {
          $attrib->createDestination();
          foreach(array_keys($attrib->m_destInstance->m_attribList) as $relattribname)
          {
            $relattrib = &$attrib->m_destInstance->getAttribute($relattribname);
            
            if ($this->_usableForImport($obligatoryOnly, $relattrib))
            {
              $attrs[] = $relattribname;
            }
          }
        }
        else
        {        
          if ($this->_usableForImport($obligatoryOnly, $attrib))
          {
            $attrs[] = $attribname;
          }
        }
      }
      return $attrs;
    }
       
    //make a record of translations of the given attributes
    function getAttributesTranslation($attributes){
      $result = array();
      
      foreach($attributes as $attribute)
      {
        $result[] = atktext($attribute, $this->m_node->m_module, $this->m_node->m_type);
      }
      
      return $result;
    }
    
    /**
     * the real import function
     * import the uploaded csv file for real
     */
    function doExport()
    {
      $col_map =      $this->m_postvars["col_map"];
      $enclosure =    $this->m_postvars["enclosure"];
      $delimiter =    $this->m_postvars["delimiter"];
      
      $skipfirstrow = $this->m_postvars['generatetitlerow'];
      $rl = atknew("atk.recordlist.atkcustomrecordlist");
      $records = $this->m_node->selectDb();
      $rl->render($this->m_node, $records, "", $enclosure, $enclosure.$delimiter, "\r\n", 1, "", "", array(), "csv");
      
      return true; 
    }
  }
?>
