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
  define("WW_TABLE"    ,  64); // table editor (TODO: TABLE EDITOR IS CURRENTLY BROKEN!!!!!!!!)
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
  
    var $m_editorname = "";
    var $m_urlpath = "";
    
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
          $this->m_editorname = $fieldprefix.$this->fieldname()."_editor";
          $g_layout->register_submitscript($this->m_editorname."_save();");          
          
          
          $result = '<input type="hidden" name="'.$fieldprefix.$this->fieldname().'" value="">';         
          //include $config_atkroot."atk/attributes/wysiwyg/editor.php";
          $result.= $this->renderEditor($fieldprefix.$this->fieldname(),$record[$this->fieldname()], $viewselectbox);
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
    
    function renderEditor($fieldname, $current_value="", $viewselectbox)
    {	
      global $HTTP_USER_AGENT, $insertat_editor, $navegador;
      
      $this->m_urlpath = atkconfig("atkroot")."atk/attributes/wysiwyg/";
      
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
      
      $editor_height = 350;
      $editor_width= 630;
            
      
      $result.=  "<script language=javascript >";
      // Variable de document_complet:
      $result.= "\n var ".$this->m_editorname."_doc_complet = 0;";
      
      // Save function:
      $result.= "\n function ".$this->m_editorname."_save() { ";
      
      // Contingut: (la funció contingut_html ja fa strip_body si cal)
      $result.= "\n var cont = get_html('".$this->m_editorname."');";
      // Posem el valor al input hidden:
      $result.= "\n document.entryform.".$fieldname.".value = cont; ";
      $result.= "\n return true;";			
      
      $result.= "\n } ";
      $result.= "</script>";
      $result.= "<table cellspacing=2 cellpadding=0 border=0 bgcolor=silver><tr><td>";
      //$result.= "<input type=hidden name=\"".$this->m_editorname."_contingut_html\" value=\"\" >"; 
      $result.= '<table cellspacing=0 cellpadding=0 border=0 hspace=3 >
      <tr>
      <td valign=middle >
      &nbsp;&nbsp;&nbsp;
      </td>';
      
      $buttons = $this->getButtons();      
      
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
          $result.= "<td valign=middle ><img src='".$this->m_urlpath."images/separator.gif' border=0 align=absmiddle ></td>";
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
      $result.= "<embed type=\"application/x-eskerplus\" id=\"".$this->m_editorname."\" classid=\"clsid:2D360201-FFF5-11d1-8D03-00A0C959BC0A\"  height=".$editor_height." width=".$editor_width." VIEWASTEXT >";
      }
      else
      {
      $result.= "<object ID=\"".$this->m_editorname."\" CLASSID=\"clsid:2D360201-FFF5-11D1-8D03-00A0C959BC0A\" height=".$editor_height." width=".$editor_width." VIEWASTEXT >";
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
      $result.= "\n ".$this->m_editorname."_timerID=setInterval(\"".$this->m_editorname."_inicial()\",100);";
      $result.= "\n function ".$this->m_editorname."_inicial(){ ";
      $result.= "\n if( window[\"PropertyAccessor\"] && window[\"".$this->m_editorname."\"]){ ";
        $result.= "\n obj_editor = ".$this->m_editorname.";";
        $result.= "\n ".PropAcce_string("Set",$this->m_editorname,"DocumentHTML",$current_value_aux).";";
        $result.= "\n clearInterval(".$this->m_editorname."_timerID);";
      $result.= "\n } ";
      $result.= "\n return true;";
      $result.= "\n } ";
      }
      else
      {
      $result.= "\n ".$this->m_editorname."_timerID=setInterval(\"".$this->m_editorname."_inicial()\",100);";
      $result.= "\n function ".$this->m_editorname."_inicial(){ ";
      $result.= "\n if( document[\"".$this->m_editorname."\"]){ ";
        $result.= "\n obj_editor = document.".$this->m_editorname.";";
        $result.= "\n document.".$this->m_editorname.".DocumentHTML = ".$current_value_aux;
        $result.= "\n clearInterval(".$this->m_editorname."_timerID);";
      $result.= "\n } ";
      $result.= "\n return true;";
      $result.= "\n } ";
      }
      $result.= "\n </script> ";
      
      $insertat_editor = 1;
      
      
      return $result;
    }
    
    function getButtons()
    {    
      $buttons = array();   
     
      if ($this->editmode(WW_CLEAR))
      {
        $buttons["clear"]["buttons"][] = "<a href=\"\"  onclick=\"obj_editor=".$this->m_editorname."; load_image('".$this->m_editorname."_nou','".$this->m_urlpath."images/newdoc.gif'); clear_doc(); return false;\"  onmouseover=\"load_image('".$this->m_editorname."_nou','".$this->m_urlpath."images/newdoc_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_nou','".$this->m_urlpath."images/newdoc.gif')\" ><img src='".$this->m_urlpath."images/newdoc.gif' alt='' border=0 align=absmiddle name='".$this->m_editorname."_nou' ></a>";  
        $buttons["clear"]["size"] = 1;
      }
      
      if ($this->editmode(WW_COPYPASTE))
      {
        $buttons["copypaste"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$this->m_editorname."; cut(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_cort','".$this->m_urlpath."images/cut_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_cort','".$this->m_urlpath."images/cut.gif')\" ><img name='".$this->m_editorname."_cort' src='".$this->m_urlpath."images/cut.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["copypaste"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$this->m_editorname."; copy(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_cop','".$this->m_urlpath."images/copy_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_cop','".$this->m_urlpath."images/copy.gif')\" ><img name='".$this->m_editorname."_cop' src='".$this->m_urlpath."images/copy.gif' alt='' border=0 align=absmiddle ></a>"; 
        $buttons["copypaste"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$this->m_editorname."; paste(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_paster','".$this->m_urlpath."images/paste_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_paster','".$this->m_urlpath."images/paste.gif')\" ><img name='".$this->m_editorname."_paster' src='".$this->m_urlpath."images/paste.gif' alt='' border=0 align=absmiddle ></a>";   
        $buttons["copypaste"]["size"] = 3;
      }
      if ($this->editmode(WW_UNDO))
      {
        $buttons["undo"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$this->m_editorname."; undo(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_ud','".$this->m_urlpath."images/undo_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_ud','".$this->m_urlpath."images/undo.gif')\" ><img name='".$this->m_editorname."_ud' src='".$this->m_urlpath."images/undo.gif' alt='' border=0 align=absmiddle ></a>"; 
        $buttons["undo"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$this->m_editorname."; redo(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_rd','".$this->m_urlpath."images/redo_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_rd','".$this->m_urlpath."images/redo.gif')\" ><img name='".$this->m_editorname."_rd' src='".$this->m_urlpath."images/redo.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["undo"]["size"] = 2;
      }
      if ($this->editmode(WW_FIND))
      {
        $buttons["find"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$this->m_editorname."; load_image('".$this->m_editorname."_fi','".$this->m_urlpath."images/find.gif'); find_in_text(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_fi','".$this->m_urlpath."images/find_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_fi','".$this->m_urlpath."images/find.gif')\" ><img name='".$this->m_editorname."_fi' src='".$this->m_urlpath."images/find.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["find"]["size"] = 1;
      }
      if ($this->editmode(WW_LINK))
      {
        $buttons["link"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$this->m_editorname."; load_image('".$this->m_editorname."_lnk','".$this->m_urlpath."images/link.gif'); insert_link(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_lnk','".$this->m_urlpath."images/link_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_lnk','".$this->m_urlpath."images/link.gif')\" ><img name='".$this->m_editorname."_lnk' src='".$this->m_urlpath."images/link.gif' alt='' border=0 align=absmiddle ></a>";   
        $buttons["link"]["size"] = 1;
      }
      /*
      if ($this->editmode(WW_IMAGE))
      {
      $result.='<td valign=middle >';
      $result.= "\n	<a href=''  onclick=\"obj_editor=".$this->m_editorname."; load_image('".$this->m_editorname."_image','".$this->m_urlpath."images/image.gif'); insert_img(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_image','".$this->m_urlpath."images/image_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_image','".$this->m_urlpath."images/image.gif')\" ><img name='".$this->m_editorname."_image' src='".$this->m_urlpath."images/image.gif' alt='' border=0 align=absmiddle ></a> "; 
      $result.='</td>';
      $buttoncount++;
      }*/      
      
      if ($this->editmode(WW_FONT_FACE))
      {
        $btn = "<select name=\"".$this->m_editorname."_FontName\" style=\"width:140\" TITLE=\"Font Name\" LANGUAGE=\"javascript\" onchange=\"obj_editor=".$this->m_editorname."; return FontName_onchange(this)\"> "; 
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
        $btn ="<select name=\"".$this->m_editorname."_FontSize\" style=\"width:40\" TITLE=\"Font Size\" LANGUAGE=\"javascript\" onchange=\"obj_editor = ".$this->m_editorname."; return FontSize_onchange(this)\"> "; 
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
        $buttons["style"]["buttons"][] = "<a href=\"#\"  onclick=\"obj_editor=".$this->m_editorname."; bold(); return false;\"  onmouseover=\"load_image('".$this->m_editorname."_negr','".$this->m_urlpath."images/bold_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_negr','".$this->m_urlpath."images/bold.gif')\" ><img src='".$this->m_urlpath."images/bold.gif' alt='' border=0 align=absmiddle name='".$this->m_editorname."_negr' ></a>";
        $buttons["style"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$this->m_editorname."; italic(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_curs','".$this->m_urlpath."images/italic_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_curs','".$this->m_urlpath."images/italic.gif')\" ><img name='".$this->m_editorname."_curs' src='".$this->m_urlpath."images/italic.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["style"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$this->m_editorname."; underline(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_subr','".$this->m_urlpath."images/under_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_subr','".$this->m_urlpath."images/under.gif')\" ><img name='".$this->m_editorname."_subr' src='".$this->m_urlpath."images/under.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["style"]["size"] = 3;
      
      }
      if ($this->editmode(WW_COLOR))
      {
        $buttons["color"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$this->m_editorname."; load_image('".$this->m_editorname."_fg','".$this->m_urlpath."images/fgcolor.gif'); popup_fg_color(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_fg','".$this->m_urlpath."images/fgcolor_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_fg','".$this->m_urlpath."images/fgcolor.gif')\" ><img name='".$this->m_editorname."_fg' src='".$this->m_urlpath."images/fgcolor.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["color"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$this->m_editorname."; load_image('".$this->m_editorname."_bg','".$this->m_urlpath."images/bgcolor.gif'); popup_bg_color(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_bg','".$this->m_urlpath."images/bgcolor_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_bg','".$this->m_urlpath."images/bgcolor.gif')\" ><img name='".$this->m_editorname."_bg' src='".$this->m_urlpath."images/bgcolor.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["color"]["size"] = 2;
      }
      if ($this->editmode(WW_ALIGN))
      {
        $buttons["align"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$this->m_editorname."; align_left(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_ae','".$this->m_urlpath."images/left_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_ae','".$this->m_urlpath."images/left.gif')\" ><img name='".$this->m_editorname."_ae' src='".$this->m_urlpath."images/left.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["align"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$this->m_editorname."; align_center(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_center','".$this->m_urlpath."images/center_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_center','".$this->m_urlpath."images/center.gif')\" ><img name='".$this->m_editorname."_center' src='".$this->m_urlpath."images/center.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["align"]["buttons"][] = "<a href='' onclick=\"obj_editor=".$this->m_editorname."; align_right(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_ad','".$this->m_urlpath."images/right_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_ad','".$this->m_urlpath."images/right.gif')\" ><img name='".$this->m_editorname."_ad' src='".$this->m_urlpath."images/right.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["align"]["size"] = 3;
      }
      if ($this->editmode(WW_TABLE))
      {    
        $buttons["table"]["buttons"][] = $this->button("insert_table", "popup_insert_table");
        $buttons["table"]["buttons"][] = $this->button("insert_row", "table_insertrow");
        $buttons["table"]["buttons"][] = $this->button("delete_row", "table_deleterow");
        $buttons["table"]["buttons"][] = $this->button("insert_col", "table_insertcol");
        $buttons["table"]["buttons"][] = $this->button("delete_col", "table_deletecol");
        $buttons["table"]["buttons"][] = $this->button("insert_cell", "table_insertcell");
        $buttons["table"]["buttons"][] = $this->button("delete_cell", "table_deletecell");
        $buttons["table"]["buttons"][] = $this->button("merge_cell", "table_mergecell");
        $buttons["table"]["buttons"][] = $this->button("split_cell", "table_splitcell");
                
        $buttons["table"]["size"]=9;
      }
      if ($this->editmode(WW_LIST))
      {
        $buttons["list"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$this->m_editorname."; list_numbers(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_nl','".$this->m_urlpath."images/numlist_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_nl','".$this->m_urlpath."images/numlist.gif')\" ><img name='".$this->m_editorname."_nl' src='".$this->m_urlpath."images/numlist.gif' alt='' border=0 align=absmiddle ></a>";
        $buttons["list"]["buttons"][] = "<a href='' onclick= \"obj_editor=".$this->m_editorname."; list_bullets(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_ul','".$this->m_urlpath."images/bullist_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_ul','".$this->m_urlpath."images/bullist.gif')\" ><img name='".$this->m_editorname."_ul' src='".$this->m_urlpath."images/bullist.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["list"]["size"] = 2;
      }
      if ($this->editmode(WW_INDENT))
      {
        $buttons["ident"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$this->m_editorname."; deindent(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_deind','".$this->m_urlpath."images/deindent_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_deind','".$this->m_urlpath."images/deindent.gif')\" ><img name='".$this->m_editorname."_deind' src='".$this->m_urlpath."images/deindent.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["ident"]["buttons"][] = "<a href=''  onclick=\"obj_editor=".$this->m_editorname."; indent(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_ind','".$this->m_urlpath."images/inindent_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_ind','".$this->m_urlpath."images/inindent.gif')\" ><img name='".$this->m_editorname."_ind' src='".$this->m_urlpath."images/inindent.gif' alt='' border=0 align=absmiddle ></a>";  
        $buttons["ident"]["size"] = 2;
      }        
      
      return $buttons;
    }
    
    function getEditFlags()
    {
      return $this->m_editflags;
    }
    
    function button($name, $function)
    {
      return "<a href='' onclick=\"obj_editor=".$this->m_editorname.";  $function(); return false;\" onmouseover=\"load_image('".$this->m_editorname."_$name','".$this->m_urlpath."images/$name"."_focus.gif');\" onmouseout=\"load_image('".$this->m_editorname."_$name','".$this->m_urlpath."images/$name.gif')\" ><img name='".$this->m_editorname."_$name' src='".$this->m_urlpath."images/$name.gif' alt='".text("wysiwyg_$name")."' border=0 align=absmiddle ></a> "; 
    }
 
  }
  
?>
