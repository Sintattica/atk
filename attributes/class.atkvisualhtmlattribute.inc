<?php
  /**
   * The atkVisualHtmlAttribute class is an MS IE html edit control thingee
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @version $Revision$
   *
   * $Id$   
   *
   */
   
  include_once($config_atkroot."atk/attributes/wysiwyg/property_accessor.php");
  include_once($config_atkroot."atk/atkbrowsertools.inc");
  include_once($config_atkroot."atk/attributes/class.atktextattribute.inc");
  
  /**
   * These defines control the behaviour of the visual html control. You can 
   * pass these defines as second parameter to the atkVisualHtmlAttribute.
   * Use a | to specify multiple values (like flags).
   */
  define("WW_ALL"      ,   0); 
  define("WW_STYLE"    ,   1); // style options (bold, italic, underline)
  define("WW_FONT_FACE",   2); // font face (helvetica, arial, etc)
  define("WW_FONT_SIZE",   4); // font size
  define("WW_FONT"     ,WW_FONT_SIZE|WW_FONT_FACE); // all font options (font face, font size)
  define("WW_COPYPASTE",   8); // cut, copy, paste, undo, redo and find options
  define("WW_LINK"     ,  16); // link button
  //define("WW_IMAGE"    ,  32); // image button (TODO: IMAGE EDITOR IS CURRENTLY BROKEN!!!!!)
  //define("WW_TABLE"    ,  64); // table editor (TODO: TABLE EDITOR IS CURRENTLY BROKEN!!!!!!!!)
  define("WW_COLOR"    , 128); // text background and foreground color
  define("WW_ALIGN"    , 256); // text alignment (left, right, center)
  define("WW_LIST"     , 512); // numbered and bulleted lists
  define("WW_INDENT"   ,1024); // Indentation buttons
  define("WW_CLEAR"    ,2048); // clear document button
  define("WW_UNDO"     ,4096);
  define("WW_FIND"     ,8192);
  define("WW_TOOLS", WW_COPYPASTE|WW_UNDO|WW_FIND);

  
  // predefine for a 'simple' visual editor that only supports basic options
  // that don't allow a user to crap a sites layout.
  define("WW_LIGHT", WW_STYLE|WW_TOOLS|WW_LIST|WW_LINK|WW_ALIGN);
   
  class atkVisualHtmlAttribute extends atkTextAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $editflags These define what buttons the toolbar contains.
     * @param $flags Flags for this attribute
     */
    function atkVisualHtmlAttribute($name, $editflags=WW_ALL, $flags=0)
    {
      $this->m_editflags = $editflags;
      $this->atkTextAttribute($name,12,$flags); // base class constructor
    }
    
    function editmode($flag)
    {
      // first check special WW_ALL flag, which is zero, but results
      // in true anyway.
      $flags = $this->getEditFlags();
      if ($flags==WW_ALL) return true;
      return (($flags & $flag) == $flag);
    }


    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a textarea
     */
    function edit($record="", $fieldprefix="")
    {      
      global $config_atkroot, $g_layout, $config_enable_ie_extensions;
      global $atkwwview, $PHP_SELF;
      
      //var_dump($record);
                                                            
      if (!ieExtensions())
      {
        return atkTextAttribute::edit($record, $fieldprefix);
      }
      else
      {    
        $wwview = $atkwwview[$fieldprefix.$this->fieldName()];
        if ($wwview=="")
        {
          $wwview = atkconfig("ww_defaultview");
        }

        $viewselectbox = '<select name="atkwwview['.$fieldprefix.$this->fieldName().']">
                            <option value="visual">'.text("ww_visualmode").
                            '<option value="plain" '.($wwview=="plain"?"selected":"").'>'.text("ww_sourcemode").
                         '</select>';
      
        if ($wwview!="plain")
        {
          $g_layout->register_script($config_atkroot."atk/javascript/class.atkvisualhtmlattribute.js");      
          $editor_name = $fieldprefix.$this->fieldname()."_editor";
          $g_layout->register_submitscript($editor_name."_save();");          
          
          
          $result = '<input type="hidden" name="'.$fieldprefix.$this->fieldname().'" value="">';         
          //include $config_atkroot."atk/attributes/wysiwyg/editor.php";
          $result.= $this->renderEditor($editor_name, $record[$this->fieldname()], $viewselectbox);
        }
        else
        {          
          $result.=$viewselectbox.'<br>';
          $result.=atkTextAttribute::edit($record, $fieldprefix);          
        }
        return $result;
      }
    }
    
    function display($record)
    {
      return $record[$this->fieldName()];
    }

    // overide the fetchValue function to perform some content cleaning
    function fetchValue($rec)
    {
     // TODO: value is replaced in the DB
     //       but the screen still shows the unreplaced value

     $content = $rec[$this->fieldName()];
     // remove <p>&nbsp;</p>
      if (strtolower(trim($content)) == "<p>&nbsp;</p>")
      {
       $content = "";
      }

     // no fonts allowed!
     if (!hasFlag($this->getEditFlags(), WW_FONT)) 
      {
        atkdebug("NO FONT's ALLOWED");
        $syntax = array("<font[^>.]*>"=>" ",  
                      "</font[^>.]*>"=>" ");
       $content = $this->stripTags($content,$syntax);
      }

    return $content;
    }

    function stripTags($ostring, $syntax)
    {
      foreach ($syntax as $toreplace => $replaceby)
      {
        $htmlk = "(" . $toreplace . ")";   //construct the html key
        $ostring = eregi_replace($toreplace,$replaceby,$ostring);
      }
     return $ostring;
    }
    
    function renderEditor($editor_name, $current_value="", $viewselectbox)
    {	
      global $HTTP_USER_AGENT, $insertat_editor, $navegador;
      
      $urlpath = atkconfig("atkroot")."atk/attributes/wysiwyg/";
      
      if( strstr($HTTP_USER_AGENT,'IE')!= false || strstr($HTTP_USER_AGENT,'MS')!= false || strstr($HTTP_USER_AGENT,'EXPLORER')!= false )
      {
        $navegador = "IE";
      }
      else
      {
        $navegador = "NS";
      }
          
      if( !isset($current_value) ){
      $current_value = '';
      }
      
      if( !isset($funcio_save) ){
      $funcio_save = '';
      if( $noset == 1 ){
      $funcio_save = 'salvar_html(codi);';
      }
      
      }
      
      if( !isset($action_submit) ){
      $action_submit = '';
      }
      
      $editor_height = 200;
      $editor_width= 550;
            
      
      $result.=  "<script language=javascript >";
      // Variable de document_complet:
      $result.= "\n var ".$editor_name."_doc_complet = 0;";
      
      // Save function:
      $result.= "\n function ".$editor_name."_save() { ";
      
      // Contingut: (la funció contingut_html ja fa strip_body si cal)
      $result.= "\n var cont = get_html('".$editor_name."');";
      // Posem el valor al input hidden:
      $result.= "\n document.entryform.".$fieldprefix.$this->fieldname().".value = cont; ";
      $result.= "\n return true;";			
      
      $result.= "\n } ";
      $result.= "</script>";
      $result.= "<table cellspacing=2 cellpadding=0 border=0 bgcolor=silver><tr><td>";
      //$result.= "<input type=hidden name=\"".$editor_name."_contingut_html\" value=\"\" >"; 
      $result.= '<table cellspacing=0 cellpadding=0 border=0 hspace=3 >
      <tr>
      <td valign=middle >
      &nbsp;&nbsp;&nbsp;
      </td>';
      
      $buttons = $this->getButtons($editor_name, $urlpath);      
      
      // Special button for switching views
      $buttons["viewselect"]["buttons"][] = "&nbsp;".$viewselectbox;
      $buttons["viewselect"]["size"] = 5; // about as big as 5 buttons.            
            
      $btncnt = 0;
      $first = true; // keep track, because first group doesn't start with a separator
      foreach ($buttons as $group=>$btns)
      {
        // Does this group of buttons fit on the current row?
        if ($btncnt+$btns["size"]>=17)
        {
          // Doesn't fit. 
          // split the row:
          $btncnt=0;
          $result.='</tr>
            </table>
            <table cellspacing=0 cellpadding=0 border=0 >
              <tr>
          <td valign=middle >
                &nbsp;&nbsp;&nbsp;
          </td>';
          $first = true;
        }
      
        if (!$first)
        {
          $result.= "<td valign=middle ><img src='".$urlpath."images/separator.gif' border=0 align=absmiddle ></td>";
        }
      
        // echo the buttons:
        for($i=0, $_i=count($btns["buttons"]);$i<$_i;$i++)
        {
          $result.='<td valign="middle">'.$btns["buttons"][$i];    
        }  
        $btncnt+=$btns["size"];
        $first = false;
      }
      
      $result.=  "<td>";
      if( !isset($insertat_editor) ){
      if( !strcmp($navegador,"NS") ){
      $result.= "<!-- Accessor de propietats a través del plug-in de Esker -->";
      $result.= "<EMBED type=\"application/x-eskerplus\" id=PropertyAccessor  classid=\"clsid:BB356E70-A100-11D4-8AF1-00104B4228F5\" codebase=\"accessor.ocx#Version=1,0,0,1\" width=2 height=2 >";
      
      $result.= "<!-- DEInsertTableParam Object -->";
      $result.= "<EMBED type=\"application/x-eskerplus\" id=\"ObjTableInfo\" CLASSID=\"clsid:47B0DFC7-B7A3-11D1-ADC5-006008A5848C\" width=2 height=2 VIEWASTEXT >";
      }
      else
      {
      $result.= "<!-- DEInsertTableParam Object -->";
      $result.= "<object ID=\"ObjTableInfo\" CLASSID=\"clsid:47B0DFC7-B7A3-11D1-ADC5-006008A5848C\" width=2 height=2 VIEWASTEXT>";
      $result.= "</object>";
      }
      }
      $result.= "</td>";
      $result.= "</tr>";
      $result.= "</table>";
      
      //Objeto de edición:
      if( !strcmp($navegador,"NS") ){
      $result.= "<embed type=\"application/x-eskerplus\" id=\"".$editor_name."\" classid=\"clsid:2D360201-FFF5-11d1-8D03-00A0C959BC0A\"  height=".$editor_height." width=".$editor_width." VIEWASTEXT >";
      }
      else
      {
      $result.= "<object ID=\"".$editor_name."\" CLASSID=\"clsid:2D360201-FFF5-11D1-8D03-00A0C959BC0A\" height=".$editor_height." width=".$editor_width." VIEWASTEXT >";
      $result.= "<param name=Scrollbars value=true>";
      $result.= "</object>";
      }
      // NOTA: fem servir el DHTMLSafe: clsid:2D360201-FFF5-11d1-8D03-00A0C959BC0A
      // Es podria fer servir el normal: clsid:2D360200-FFF5-11d1-8D03-00A0C959BC0A 
      // però dona més problemes d'autoritzacions i merdes a Explorer.
      
      $result.= "</td></tr></table>";
      
      $result.= "\n <script language =javascript >";
      // Pel contingut i els settings inicials: cal intents asíncrons i recurrents ...
      // Farem:
      // 		Lletra: Arial
      // 		Referenciat de la variable JS (que senyala a l'objecte Java del <embed del editor )
      //		Posem contingut inicial
      
      // Preparació del $content_inicial:
      $current_value = str_replace('"',"'",$current_value);
      $car_aux = "\r\n";
      $current_value = ereg_replace($car_aux,'',$current_value);
      $current_value_aux = '"'.$current_value.'"';
      
      //  Es farà d'una forma o d'altre en funció del navegador:
      if( !strcmp($navegador,"NS") ){
      $result.= "\n ".$editor_name."_timerID=setInterval(\"".$editor_name."_inicial()\",100);";
      $result.= "\n function ".$editor_name."_inicial(){ ";
      $result.= "\n if( window[\"PropertyAccessor\"] && window[\"".$editor_name."\"]){ ";
        $result.= "\n obj_editor = ".$editor_name.";";
        $result.= "\n ".PropAcce_string("Set",$editor_name,"DocumentHTML",$current_value_aux).";";
        $result.= "\n clearInterval(".$editor_name."_timerID);";
      $result.= "\n } ";
      $result.= "\n return true;";
      $result.= "\n } ";
      }
      else
      {
      $result.= "\n ".$editor_name."_timerID=setInterval(\"".$editor_name."_inicial()\",100);";
      $result.= "\n function ".$editor_name."_inicial(){ ";
      $result.= "\n if( document[\"".$editor_name."\"]){ ";
        $result.= "\n obj_editor = document.".$editor_name.";";
        $result.= "\n document.".$editor_name.".DocumentHTML = ".$current_value_aux;
        $result.= "\n clearInterval(".$editor_name."_timerID);";
      $result.= "\n } ";
      $result.= "\n return true;";
      $result.= "\n } ";
      }
      $result.= "\n </script> ";
      
      $insertat_editor = 1;
      
      
      return $result;
    }
    
    function getButtons($editor_name, $urlpath)
    {    
      $buttons = array();   
     
      if ($this->editmode(WW_CLEAR))
      {
        $buttons["clear"]["buttons"][] = "<a href=\"\"  onclick=\"obj_editor=".$editor_name."; load_image('".$editor_name."_nou','".$urlpath."images/newdoc.gif'); clear_doc(); return false;\"  onmouseover=\"load_image('".$editor_name."_nou','".$urlpath."images/newdoc_focus.gif');\" onmouseout=\"load_image('".$editor_name."_nou','".$urlpath."images/newdoc.gif')\" ><img src='".$urlpath."images/newdoc.gif' alt='' border=0 align=absmiddle name='".$editor_name."_nou' ></a>";  
        $buttons["clear"]["size"] = 1;
      }
      
      if ($this->editmode(WW_COPYPASTE))
      {
        $buttons["copypaste"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$editor_name."; cut(); return false;\" onmouseover=\"load_image('".$editor_name."_cort','".$urlpath."images/cut_focus.gif');\" onmouseout=\"load_image('".$editor_name."_cort','".$urlpath."images/cut.gif')\" ><img name='".$editor_name."_cort' src='".$urlpath."images/cut.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["copypaste"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$editor_name."; copy(); return false;\" onmouseover=\"load_image('".$editor_name."_cop','".$urlpath."images/copy_focus.gif');\" onmouseout=\"load_image('".$editor_name."_cop','".$urlpath."images/copy.gif')\" ><img name='".$editor_name."_cop' src='".$urlpath."images/copy.gif' alt='' border=0 align=absmiddle ></a>"; 
        $buttons["copypaste"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$editor_name."; paste(); return false;\" onmouseover=\"load_image('".$editor_name."_paster','".$urlpath."images/paste_focus.gif');\" onmouseout=\"load_image('".$editor_name."_paster','".$urlpath."images/paste.gif')\" ><img name='".$editor_name."_paster' src='".$urlpath."images/paste.gif' alt='' border=0 align=absmiddle ></a>";   
        $buttons["copypaste"]["size"] = 3;
      }
      if ($this->editmode(WW_UNDO))
      {
        $buttons["undo"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$editor_name."; undo(); return false;\" onmouseover=\"load_image('".$editor_name."_ud','".$urlpath."images/undo_focus.gif');\" onmouseout=\"load_image('".$editor_name."_ud','".$urlpath."images/undo.gif')\" ><img name='".$editor_name."_ud' src='".$urlpath."images/undo.gif' alt='' border=0 align=absmiddle ></a>"; 
        $buttons["undo"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$editor_name."; redo(); return false;\" onmouseover=\"load_image('".$editor_name."_rd','".$urlpath."images/redo_focus.gif');\" onmouseout=\"load_image('".$editor_name."_rd','".$urlpath."images/redo.gif')\" ><img name='".$editor_name."_rd' src='".$urlpath."images/redo.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["undo"]["size"] = 2;
      }
      if ($this->editmode(WW_FIND))
      {
        $buttons["find"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$editor_name."; load_image('".$editor_name."_fi','".$urlpath."images/find.gif'); find_in_text(); return false;\" onmouseover=\"load_image('".$editor_name."_fi','".$urlpath."images/find_focus.gif');\" onmouseout=\"load_image('".$editor_name."_fi','".$urlpath."images/find.gif')\" ><img name='".$editor_name."_fi' src='".$urlpath."images/find.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["find"]["size"] = 1;
      }
      if ($this->editmode(WW_LINK))
      {
        $buttons["link"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$editor_name."; load_image('".$editor_name."_lnk','".$urlpath."images/link.gif'); insert_link(); return false;\" onmouseover=\"load_image('".$editor_name."_lnk','".$urlpath."images/link_focus.gif');\" onmouseout=\"load_image('".$editor_name."_lnk','".$urlpath."images/link.gif')\" ><img name='".$editor_name."_lnk' src='".$urlpath."images/link.gif' alt='' border=0 align=absmiddle ></a>";   
        $buttons["link"]["size"] = 1;
      }
      /*
      if ($this->editmode(WW_IMAGE))
      {
      $result.='<td valign=middle >';
      $result.= "\n	<a href=''  onclick=\"obj_editor=".$editor_name."; load_image('".$editor_name."_image','".$urlpath."images/image.gif'); insert_img(); return false;\" onmouseover=\"load_image('".$editor_name."_image','".$urlpath."images/image_focus.gif');\" onmouseout=\"load_image('".$editor_name."_image','".$urlpath."images/image.gif')\" ><img name='".$editor_name."_image' src='".$urlpath."images/image.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td>';
      $buttoncount++;
      }*/
      /*if ($this->editmode(WW_TABLE))
      {
      $result.='<td valign=middle >';
      $result.= "\n	<img src='".$urlpath."images/separator.gif' border=0 align=absmiddle > "; 
      $result.='</td><td valign=middle >';
      $result.= "\n	<a href=''  onclick=\"obj_editor=".$editor_name."; load_image('".$editor_name."_table','".$urlpath."images/instable.gif');  popup_insert_table(); return false;\" onmouseover=\"load_image('".$editor_name."_table','".$urlpath."images/instable_focus.gif');\" onmouseout=\"load_image('".$editor_name."_table','".$urlpath."images/instable.gif')\" ><img name='".$editor_name."_table' src='".$urlpath."images/instable.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td><td valign=middle >';
      $result.= "\n	<a href=''  onclick=\"obj_editor=".$editor_name."; table_insertrow(); return false;\" onmouseover=\"load_image('".$editor_name."_ir','".$urlpath."images/insrow_focus.gif');\" onmouseout=\"load_image('".$editor_name."_ir','".$urlpath."images/insrow.gif')\" ><img name='".$editor_name."_ir' src='".$urlpath."images/insrow.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td><td valign=middle >';
      $result.= "\n	<a href=''  onclick=\"obj_editor=".$editor_name."; table_deleterow(); return false;\" onmouseover=\"load_image('".$editor_name."_dr','".$urlpath."images/delrow_focus.gif');\" onmouseout=\"load_image('".$editor_name."_dr','".$urlpath."images/delrow.gif')\" ><img name='".$editor_name."_dr' src='".$urlpath."images/delrow.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td><td valign=middle >';
      $result.="\n	<a href=''  onclick=\"obj_editor=".$editor_name."; table_insertcol(); return false;\" onmouseover=\"load_image('".$editor_name."_ic','".$urlpath."images/inscol_focus.gif');\" onmouseout=\"load_image('".$editor_name."_ic','".$urlpath."images/inscol.gif')\" ><img name='".$editor_name."_ic' src='".$urlpath."images/inscol.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td><td valign=middle >';
      $result.="\n	<a href=''  onclick=\"obj_editor=".$editor_name."; table_deletecol(); return false;\" onmouseover=\"load_image('".$editor_name."_dc','".$urlpath."images/delcol_focus.gif');\" onmouseout=\"load_image('".$editor_name."_dc','".$urlpath."images/delcol.gif')\" ><img name='".$editor_name."_dc' src='".$urlpath."images/delcol.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td><td valign=middle >';
      $result.="\n	<img src='".$urlpath."images/separator.gif' border=0 align=absmiddle > "; 
      $result.='</td><td valign=middle >';
      $result.="\n	<a href=''  onclick=\"obj_editor=".$editor_name."; table_insertcell(); return false;\" onmouseover=\"load_image('".$editor_name."_ice','".$urlpath."images/inscell_focus.gif');\" onmouseout=\"load_image('".$editor_name."_ice','".$urlpath."images/inscell.gif')\" ><img name='".$editor_name."_ice' src='".$urlpath."images/inscell.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td><td valign=middle >';
      $result.="\n	<a href=''  onclick=\"obj_editor=".$editor_name."; table_deletecell(); return false;\" onmouseover=\"load_image('".$editor_name."_dce','".$urlpath."images/delcell_focus.gif');\" onmouseout=\"load_image('".$editor_name."_dce','".$urlpath."images/delcell.gif')\" ><img name='".$editor_name."_dce' src='".$urlpath."images/delcell.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td><td valign=middle >';
      $result.="\n	<a href=''  onclick=\"obj_editor=".$editor_name."; table_mergecell(); return false;\" onmouseover=\"load_image('".$editor_name."_cc','".$urlpath."images/mrgcell_focus.gif');\" onmouseout=\"load_image('".$editor_name."_cc','".$urlpath."images/mrgcell.gif')\" ><img name='".$editor_name."_cc' src='".$urlpath."images/mrgcell.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td><td valign=middle >';
      $result.="\n	<a href=''  onclick=\"obj_editor=".$editor_name."; table_splitcell(); return false;\" onmouseover=\"load_image('".$editor_name."_sc','".$urlpath."images/spltcell_focus.gif');\" onmouseout=\"load_image('".$editor_name."_sc','".$urlpath."images/spltcell.gif')\" ><img name='".$editor_name."_sc' src='".$urlpath."images/spltcell.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td><td valign=middle >';
      $result.='</td>';
      $buttoncount+=9;
      }*/
      
      /*if ($buttoncount>15)
      {
      $result.='</tr>
        </table>
        <table cellspacing=0 cellpadding=0 border=0 >
          <tr>
      <td valign=middle >
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      </td>';
      }*/
      
      if ($this->editmode(WW_FONT_FACE))
      {
        $btn = "<select name=\"".$editor_name."_FontName\" style=\"width:140\" TITLE=\"Font Name\" LANGUAGE=\"javascript\" onchange=\"obj_editor=".$editor_name."; return FontName_onchange(this)\"> "; 
        $btn.= "<option value=\"0\" selected >".text('wysiwyg_fontface');
        
        $btn.='	<option value="Verdana" >Verdana
          <option value="Arial" >Arial
          <option value="Tahoma">Tahoma
          <option value="Courier New">Courier New
          <option value="Times New Roman" >Times New Roman
          <option value="Wingdings">Wingdings
        </select>
        </td>';
        $buttons["fontface"]["buttons"][] = $btn;
        $buttons["fontface"]["size"] = 5; // dropdown is about as big as 5 regular buttons.
      }
      if ($this->editmode(WW_FONT_SIZE))
      {  
        $btn ="<select name=\"".$editor_name."_FontSize\" style=\"width:40\" TITLE=\"Font Size\" LANGUAGE=\"javascript\" onchange=\"obj_editor = ".$editor_name."; return FontSize_onchange(this)\"> "; 
        $btn.= "<option value=\"0\" selected >".text('wysiwyg_fontsize');
      
        $btn.='    <option value="1">1
          <option value="2">2
          <option value="3">3
          <option value="4">4
          <option value="5">5
          <option value="6">6
          <option value="7">7
        </select>
        </td>';
        $buttons["fontsize"]["buttons"][] = $btn;
        $buttons["fontsize"]["size"] = 1;
      }
      if ($this->editmode(WW_STYLE))
      {
        $buttons["style"]["buttons"][] = "<a href=\"#\"  onclick=\"obj_editor=".$editor_name."; bold(); return false;\"  onmouseover=\"load_image('".$editor_name."_negr','".$urlpath."images/bold_focus.gif');\" onmouseout=\"load_image('".$editor_name."_negr','".$urlpath."images/bold.gif')\" ><img src='".$urlpath."images/bold.gif' alt='' border=0 align=absmiddle name='".$editor_name."_negr' ></a>";
        $buttons["style"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$editor_name."; italic(); return false;\" onmouseover=\"load_image('".$editor_name."_curs','".$urlpath."images/italic_focus.gif');\" onmouseout=\"load_image('".$editor_name."_curs','".$urlpath."images/italic.gif')\" ><img name='".$editor_name."_curs' src='".$urlpath."images/italic.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["style"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$editor_name."; underline(); return false;\" onmouseover=\"load_image('".$editor_name."_subr','".$urlpath."images/under_focus.gif');\" onmouseout=\"load_image('".$editor_name."_subr','".$urlpath."images/under.gif')\" ><img name='".$editor_name."_subr' src='".$urlpath."images/under.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["style"]["size"] = 3;
      
      }
      if ($this->editmode(WW_COLOR))
      {
        $buttons["color"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$editor_name."; load_image('".$editor_name."_fg','".$urlpath."images/fgcolor.gif'); popup_fg_color(); return false;\" onmouseover=\"load_image('".$editor_name."_fg','".$urlpath."images/fgcolor_focus.gif');\" onmouseout=\"load_image('".$editor_name."_fg','".$urlpath."images/fgcolor.gif')\" ><img name='".$editor_name."_fg' src='".$urlpath."images/fgcolor.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["color"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$editor_name."; load_image('".$editor_name."_bg','".$urlpath."images/bgcolor.gif'); popup_bg_color(); return false;\" onmouseover=\"load_image('".$editor_name."_bg','".$urlpath."images/bgcolor_focus.gif');\" onmouseout=\"load_image('".$editor_name."_bg','".$urlpath."images/bgcolor.gif')\" ><img name='".$editor_name."_bg' src='".$urlpath."images/bgcolor.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["color"]["size"] = 2;
      }
      if ($this->editmode(WW_ALIGN))
      {
        $buttons["align"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$editor_name."; align_left(); return false;\" onmouseover=\"load_image('".$editor_name."_ae','".$urlpath."images/left_focus.gif');\" onmouseout=\"load_image('".$editor_name."_ae','".$urlpath."images/left.gif')\" ><img name='".$editor_name."_ae' src='".$urlpath."images/left.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["align"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$editor_name."; align_center(); return false;\" onmouseover=\"load_image('".$editor_name."_center','".$urlpath."images/center_focus.gif');\" onmouseout=\"load_image('".$editor_name."_center','".$urlpath."images/center.gif')\" ><img name='".$editor_name."_center' src='".$urlpath."images/center.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["align"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$editor_name."; align_right(); return false;\" onmouseover=\"load_image('".$editor_name."_ad','".$urlpath."images/right_focus.gif');\" onmouseout=\"load_image('".$editor_name."_ad','".$urlpath."images/right.gif')\" ><img name='".$editor_name."_ad' src='".$urlpath."images/right.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["align"]["size"] = 3;
      }
      if ($this->editmode(WW_LIST))
      {
        $buttons["list"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$editor_name."; list_numbers(); return false;\" onmouseover=\"load_image('".$editor_name."_nl','".$urlpath."images/numlist_focus.gif');\" onmouseout=\"load_image('".$editor_name."_nl','".$urlpath."images/numlist.gif')\" ><img name='".$editor_name."_nl' src='".$urlpath."images/numlist.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["list"]["buttons"][] = "<a href='' onclick= \"obj_editor=".$editor_name."; list_bullets(); return false;\" onmouseover=\"load_image('".$editor_name."_ul','".$urlpath."images/bullist_focus.gif');\" onmouseout=\"load_image('".$editor_name."_ul','".$urlpath."images/bullist.gif')\" ><img name='".$editor_name."_ul' src='".$urlpath."images/bullist.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["list"]["size"] = 2;
      }
      if ($this->editmode(WW_INDENT))
      {
        $buttons["ident"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$editor_name."; deindent(); return false;\" onmouseover=\"load_image('".$editor_name."_deind','".$urlpath."images/deindent_focus.gif');\" onmouseout=\"load_image('".$editor_name."_deind','".$urlpath."images/deindent.gif')\" ><img name='".$editor_name."_deind' src='".$urlpath."images/deindent.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["ident"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$editor_name."; indent(); return false;\" onmouseover=\"load_image('".$editor_name."_ind','".$urlpath."images/inindent_focus.gif');\" onmouseout=\"load_image('".$editor_name."_ind','".$urlpath."images/inindent.gif')\" ><img name='".$editor_name."_ind' src='".$urlpath."images/inindent.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["ident"]["size"] = 2;
      }        
      
      return $buttons;
    }
    
    function getEditFlags()
    {
      return $this->m_editflags;
    }
 
  }
  
?>