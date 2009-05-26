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
   * @version $Revision: 6323 $
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
      
      // Intercept partial call
      if (!empty($this->m_partial))
      {       
        $this->partial($this->m_partial);
        return;
      }
       
      // Intercept delete call
      if( array_key_exists( 'dodelete', $this->m_postvars ) && ctype_digit( $this->m_postvars['dodelete'] ) )
      {      
        if( array_key_exists( 'confirmed', $this->m_postvars ) && $this->m_postvars['confirmed'] == 'true' )
        {
          $this->deleteSelection( $this->m_postvars['dodelete'] );
        }      
      }

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
      
      $page->register_scriptcode( "
        function toggleSelectionName( fieldval )
        {
          if( fieldval == undefined )
          {
            fieldval = $( 'export_selection_options' ).value;  
          }         
          new Ajax.Updater('export_attributes', '".partial_url( $this->m_postvars['atknodetype'], 'export', 'export' )."exportvalue='+fieldval+'&' );
          
          if( fieldval != 'none' )
          {
            if( fieldval != 'new' )
            {              
              new Ajax.Updater('selection_interact', '".partial_url( $this->m_postvars['atknodetype'], 'export', 'selection_interact' )."exportvalue='+fieldval+'&' );
              new Ajax.Updater('export_name', '".partial_url( $this->m_postvars['atknodetype'], 'export', 'selection_name' )."exportvalue='+fieldval+'&' );
              $( 'selection_interact' ).style.display='';   
              $( 'export_name' ).style.display='';
              $( 'export_save_button' ).style.display='';
            }
            else
            {
              $( 'selection_interact' ).style.display='none';
              $( 'export_name' ).style.display='';
              $( 'export_selection_name' ).value='';
              $( 'export_selection_options' ).selectedIndex=0;
              $( 'export_save_button' ).style.display='none';
            }
          }
          else
          {
            $( 'export_name' ).style.display='none';
            $( 'selection_interact' ).style.display='none'; 
            $( 'export_save_button' ).style.display='none';   
            $( 'export_selection_name' ).value='';                   
          }
        }" );
      
      $page->register_scriptcode( "
        function confirm_delete()
        {
         var where_to = confirm('".atktext('confirm_delete')."');
         var dodelete = $( 'export_selection_options' ).value;
                  
         if (where_to == true)
         {           
           window.location= \"".dispatch_url( $this->m_postvars['atknodetype'], 'export', array( 'confirmed' => 'true' ) )."&dodelete=\"+dodelete;
         }
        }" );     
      
      $page->addContent($this->m_node->genericPage("export", $content));
      return true;
    }
    
    /**
     * Handle partial request
     *
     * @return string
     */
    public function partial_export()
    {
      $value = array_key_exists( 'exportvalue', $this->m_postvars ) ? $this->m_postvars['exportvalue'] : null;
      return $this->getAttributeSelect( $value );
    }
    
    /**
     * Partial fetches and displays the name of the selected value
     *
     * @return string
     */
    public function partial_selection_name()
    {
      $selected = array_key_exists( 'exportvalue', $this->m_postvars ) ? $this->m_postvars['exportvalue'] : null;
      $value = '';
      
      if( $selected )
      {
        $db = atkGetDb();
        $rows = $db->getRows( 'SELECT name FROM atk_exportcriteria WHERE id = '.(int) $selected );
        if( count( $rows ) == 1 )
        {
          $value = htmlentities( $rows[0]['name'] ); 
        }
      }
      return '<td>'.atktext("export_selections_name", "atk").': </td><td align="left"><input type="text" size="40" name="export_selection_name" id="export_selection_name" value="'.$value.'"></td>
              <input type="hidden" name="exportvalue" value="'.$this->m_postvars['exportvalue'].'" />';
    }
    
    /**
     * Partial displays a interaction possibilities with an export selection
     *
     * @return string
     */
    public function partial_selection_interact( )
    {     
      $selected   = array_key_exists( 'exportvalue', $this->m_postvars ) ? $this->m_postvars['exportvalue'] : null;  
            
      $theme = atkinstance( 'atk.ui.atktheme' );
      
      $img_delete = $theme->iconPath('delete', 'recordlist' );          
      $url_delete = dispatch_url( $this->m_node->m_module.'.'.$this->m_node->m_type, 'export', array('dodelete' => $selected ) );
      
      if( $selected )
      {
        $result  = '<a href="'.$url_delete.'" title="'.atktext( 'delete_selection' ).'" onclick="confirm_delete();"><img src="'.$img_delete.'" alt="'.atktext( 'delete_selection' ).'" border="0" /></a>'; 
        return $result;
      }
    }

    /**
     * Gets the HTML for the initial mode of the exporthandler
     * @return String The HTML for the screen
     */
    function _getInitHtml()
    {
      $action = dispatch_url($this->m_node->m_module.'.'.$this->m_node->m_type, "export");

      $params = array();
      $params["formstart"] = '<form name="entryform" enctype="multipart/form-data" action="'.$action.'" method="post">';
      $params["formstart"] .= session_form();
      $params["formstart"] .= '<input type="hidden" name="phase" value="process"/>';
      $params["buttons"][] = '<input class="btn" type="submit" value="'.atkText("export", "atk").'"/>';
      $params["buttons"][] = '<input id="export_save_button" style="display:none;" value="'.atktext("save_export_selection","atk").'" name="save_export" class="btn" type="submit" /> '; 
      $params["buttons"][] = atkButton(atktext("back", "atk"), "", SESSION_BACK, true);
      $params["content"] = atkText("export_config_explanation", "atk", $this->m_node->m_type).'<br/><br/>';
      $params["content"] .= $this->_getOptions();
      $params["formend"] = '</form>';

      return atkInstance("atk.ui.atkui")->renderAction("export", $params, $this->m_node->m_module);
    }

    /**
     * This function checks if there is enough information to export the date
     * else it wil shows a form to set how the file wil be exported
     */
    function doProcess()
    {
      // Update selection
      if( array_key_exists( 'exportvalue', $this->m_postvars ) 
       && array_key_exists( 'save_export', $this->m_postvars )
       && '' != $this->m_postvars['export_selection_name'] )
      {
        $this->updateSelection();        
        $this->getNode()->redirect(dispatch_url($this->getNode(), 'export'));
      }
            
      // Save selection
      if( array_key_exists( 'export_selection_options', $this->m_postvars ) 
       && array_key_exists( 'export_selection_name', $this->m_postvars  ) 
       && 'none' == $this->m_postvars['export_selection_options']
       && '' != $this->m_postvars['export_selection_name'] )
      {   
        $this->saveSelection();
      }
      
      // Export CVS
      if( !array_key_exists( 'save_export', $this->m_postvars ) )
      {
        return $this->doExport();
      }
    }

    /**
     * Get the options for the export
     *
     * @return unknown
     */
    function _getOptions()
    {
      $content = '<table border="0" width="100%">';
      
      // enable extended export options
      if( true === atkconfig( 'enable_export_save_selection' ) )
      {        
        $content .= '<tr><td>'.atktext("export_selections", "atk").': </td><td align="left">'.$this->getExportSelectionDropdown().'&nbsp;&nbsp;&nbsp;<a href="javascript:void(0);" onclick="toggleSelectionName(\'new\');return false;">'.atktext('new', 'atk').'</a></td></tr>';
        $content .= '<tr><td>&nbsp;</td><td align="left"><div id="selection_interact"></div></td></tr>';
        $content .= '<tr id="export_name" style="display:none;"><td>'.atktext("export_selections_name", "atk").': </td><td align="left"><input type="text" size="40" id="export_selection_name" name="export_selection_name" value=""></td></tr>';
      }
      
      $content.= '<tr><td>'.atktext("delimiter", "atk").': </td><td><input type="text" size="2" name="delimiter" value=";"></td></tr>';
      $content.= '<tr><td>'.atktext("enclosure", "atk").': </td><td><input type="text" size="2" name="enclosure" value="&quot;"></td></tr>';
      $content.= '<tr><td valign="top">'.atktext("export_selectcolumns", "atk").': </td><td><div id="export_attributes">'.$this->getAttributeSelect().'</div></td></tr>';
      $content.= '<tr><td>';
      $content.= atktext("export_generatetitlerow").': </td><td><input type="checkbox" name="generatetitlerow" class="atkcheckbox" value="1" />';
      $content.= '</td></tr>';
      $content.= '</table><br /><br />';
      return $content;
    }
    
    /**
     * Build the dropdown field to add the exportselections 
     *
     * @return string
     */
    private function getExportSelectionDropdown( )
    {       
      $html = '      
        <select name="export_selection_options" id="export_selection_options" onchange="javascript:toggleSelectionName( );return false;" >
          <option value="none">'.atktext('none', 'atk');
      
      $options = $this->getExportSelections();
      if( count ( $options ) )
      {
        foreach( $options AS $option )
        {
          $html .= '
           <option value="'.$option['id'].'">'.htmlentities( $option['name'] ).'</option>';
        }
      }
        
      $html .= '</select>';
      
      return $html;
    }
    
    /**
     * Store selectiondetails in the database
     *
     * @return null
     */
    private function saveSelection( )
    {
      $db = atkGetDb( );
      $id = $db->nextid( 'exportcriteria' );

      $user_id = 0;
      if( 'none' !== strtolower( atkConfig( 'authentication' ) ) ) 
      {
        $user = atkGetUser( );
        $user_id = array_key_exists( atkConfig('auth_userpk'), $user ) ? $user[atkConfig('auth_userpk')] : 0;
      }
      
      // first check if the combination of node, name and user_id doesn't already exist
      $rows = $db->getRows("SELECT id FROM atk_exportcriteria 
                            WHERE nodetype = '".$this->m_postvars['atknodetype']."' 
                            AND name = '".$this->m_postvars['export_selection_name']."' 
                            AND user_id = ".$user_id);
      if (count($rows))
      {
        return;
      }
      
      $query = 'INSERT INTO atk_exportcriteria ( id, nodetype, name, criteria, user_id )
                VALUES ( '.$id.', "'.$this->m_postvars['atknodetype'].'", "'.$db->escapeSQL( $this->m_postvars['export_selection_name'] ) .'",
                         "'. addslashes( serialize( $this->m_postvars ) ).'", '.$user_id.' )';
      
      $db->query( $query );
    }
    
    /**
     * Update selectiondetails in the database
     *
     * @return null
     */
    private function updateSelection( )
    {
      $db = atkGetDb( );
      
      $user_id = 0;
      if( 'none' !== strtolower( atkConfig( 'authentication' ) ) ) 
      {
        $user = atkGetUser( );
        $user_id = array_key_exists( atkConfig('auth_userpk'), $user ) ? $user[atkConfig('auth_userpk')] : 0;
      }
      
      // first check if the combination of node, name and user_id doesn't already exist
      $rows = $db->getRows("SELECT id FROM atk_exportcriteria 
                            WHERE nodetype = '".$this->m_postvars['atknodetype']."' 
                            AND name = '".$this->m_postvars['export_selection_name']."' 
                            AND user_id = ".$user_id."
                            AND id <> ".(int) $this->m_postvars['exportvalue']);
      if (count($rows))
      {
        return;
      }

      $query = 'UPDATE 
                  atk_exportcriteria 
                SET 
                  name = "'.$db->escapeSQL( $this->m_postvars['export_selection_name'] ) .'",
                  criteria = "'.addslashes( serialize( $this->m_postvars) ).'"
                WHERE 
                  id = '. (int) $this->m_postvars['exportvalue'];
      
      $db->query( $query );
    }
        
    /**
     * Delete record
     *
     * @param integer $id
     */
    private function deleteSelection( $id )
    {
      $db = atkGetDb( );
      $db->query( 'DELETE FROM atk_exportcriteria WHERE id = '.(int) $id );
    }

    /**
     * Determine the export selections that should be displayed
     *
     * @return array
     */
    protected function getExportSelections( )
    {           
      $where = ' nodetype = "'. $this->m_postvars['atknodetype'].'"';
      if( 'none' !== strtolower( atkConfig( 'authentication' ) ) ) 
      {
        $user = atkGetUser( );
        if( 'administrator' !== strtolower( $user['name'] ) ) 
        {
          $where .= ' AND user_id IN( 0, '. (int) $user[ atkConfig( 'auth_userpk' ) ].' )';
        }
      }
      
      $db = atkGetDb( );      
      return $db->getRows( $query = 'SELECT id, name FROM atk_exportcriteria WHERE '.$where.' ORDER BY name' );
    }

    /**
     * Get all attributes to select for the export
     *
     * @return String HTML code with checkboxes for each attribute to select
     */
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
      $selected = $value == 'new' ? false : true;

      $criteria = array();
      if( !in_array( $value, array( 'new', 'none', '' ) ) )
      {
        $db   = atkGetDb( );
        $rows = $db->getRows( 'SELECT * FROM atk_exportcriteria WHERE id = '.(int) $value );
        $criteria = unserialize($rows[0]['criteria']);
      }
      
      $atts = array();
      $attriblist = $this->invoke('getExportAttributes');
      foreach($attriblist as $key => $value)
      {
        $flags = $value->m_flags;
        $class = strtolower(get_class($value));
        if($value->hasFlag(AF_AUTOKEY) ||
           $value->hasFlag(AF_HIDE_VIEW) ||
           !(strpos($class, "dummy") === FALSE) ||
           !(strpos($class, "image") === FALSE) ||
           !(strpos($class,'tabbedpane') === FALSE)
           )
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
      // selected options based on a new selection, or no selection
        if( empty( $criteria ) )
        {
          $atts[$group][] = array(  'name' => $key,   
                                    'text' => $value->label(), 
                                    'checked' => $selected == true ? !$value->hasFlag(AF_HIDE_LIST) : false
                                   );
        }
        // selected options based on a selection from DB
        else
        {
          $atts[$group][] = array(  'name' => $key,   
                                    'text' => $value->label(), 
                                    'checked' => in_array( 'export_'.$key, $criteria) ? true : false
                                   );
        }
      }
      return $atts;
    }

    /**
     * Return all attributes that can be exported
     *
     * @return array Array with attribs that needs to be exported
     */
    function getExportAttributes()
    {
      $attribs = $this->m_node->getAttributes();
      if(is_null($attribs)) { return array(); } else { return $attribs; }
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
        if(in_array($name, $list_includes) && $att->hasFlag(AF_HIDE_LIST))
          $att->removeFlag(AF_HIDE_LIST);
        elseif (!in_array($name, $list_includes))
          $att->addFlag(AF_HIDE_LIST);
      }

      if (!is_array($actions))
      {
          $actions = $node->defaultActions("export");
      }
      $rl = &atknew("atk.recordlist.atkcustomrecordlist");
      $flags = ($node_bk->hasFlag(NF_MRA) ? RL_MRA : 0) | ($node_bk->hasFlag(NF_MRPA) ? RL_MRPA : 0) | ($node_bk->hasFlag(NF_LOCK) ? RL_LOCK : 0);
      $node_bk->m_postvars = $session_back;
      $atkfilter = atkArrayNvl($source, 'atkfilter', "");
      $recordset = $node_bk->selectDb($session_back['atkselector'].($session_back['atkselector'] != '' && $atkfilter != '' ? ' AND ' : '').$atkfilter, $atkorderby, "", "", $list_includes, "export");
      if (method_exists($this->m_node, "assignExportData")) $this->m_node->assignExportData($list_includes, $recordset);
      $recordset_new = array();

      foreach($recordset as $row)
      {
        foreach($row as $name => $value)
        {
          if(in_array($name, $list_includes))
          {
            $value = str_replace("\r\n", "\\n", $value);
            $value = str_replace("\n", "\\n", $value);
            $value = str_replace("\t", "\\t", $value);
            $row[$name] = $value;
          }
        }
        $recordset_new[] = $row;
      }

      $rl->render($node_bk, $recordset_new, "", $enclosure, $enclosure.$delimiter, "\r\n", 1, "", "", array('filename' => 'export_'.$node->m_type), "csv", $source['generatetitlerow'], true);

      return true;
    }
  }
?>
