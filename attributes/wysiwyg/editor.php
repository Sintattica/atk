<?

global $HTTP_USER_AGENT;


$url_path_editor = $config_atkroot."atk/attributes/wysiwyg/";

// Mirem quin navegador es fa servir:
if( strstr($HTTP_USER_AGENT,'IE')!= false || strstr($HTTP_USER_AGENT,'MS')!= false || strstr($HTTP_USER_AGENT,'EXPLORER')!= false ){
	$navegador = "IE";
}
else
{
	$navegador = "NS";
}

if( !isset($idi_edit) ){
	$idi_edit = 'eng';
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

if( !isset($editor_height) ){
	$editor_height = 400;
}

if( !isset($editor_width) ){
	$editor_width = 700;
}

$result.=  "<script language=javascript >";
// Variable de document_complet:
$result.= "\n var ".$nom_editor."_doc_complet = 0;";

// Save function:
$result.= "\n function ".$nom_editor."_save() { ";
	  
// Contingut: (la funció contingut_html ja fa strip_body si cal)
$result.= "\n var cont = get_html('".$nom_editor."');";
// Posem el valor al input hidden:
$result.= "\n document.entryform.".$fieldprefix.$this->fieldname().".value = cont; ";
$result.= "\n return true;";			
	  
	$result.= "\n } ";
$result.= "</script>";
$result.= "<table cellspacing=2 cellpadding=0 border=0 bgcolor=silver><tr><td>";
 //$result.= "<input type=hidden name=\"".$nom_editor."_contingut_html\" value=\"\" >"; 
 $result.= '<table cellspacing=0 cellpadding=0 border=0 hspace=3 >
<tr>
<td valign=middle >
	&nbsp;&nbsp;&nbsp;
</td>';
$buttoncount = 0;

if ($this->editmode(WW_CLEAR))
{
  $result.='<td valign=middle >';
  $result.= "\n	<a href=\"\"  onclick=\"obj_editor=".$nom_editor."; load_image('".$nom_editor."_nou','".$url_path_editor."images/newdoc.gif'); clear_doc(); return false;\"  onmouseover=\"load_image('".$nom_editor."_nou','".$url_path_editor."images/newdoc_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_nou','".$url_path_editor."images/newdoc.gif')\" ><img src='".$url_path_editor."images/newdoc.gif' alt='".$mesg9[$idi_edit]."' border=0 align=absmiddle name='".$nom_editor."_nou' ></a> "; 
  $result.='</td><td valign=middle >';
  $result.= "\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td>';
  $buttoncount++;
}

if ($this->editmode(WW_TOOLS))
{
  $result.='<td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; cut(); return false;\" onmouseover=\"load_image('".$nom_editor."_cort','".$url_path_editor."images/cut_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_cort','".$url_path_editor."images/cut.gif')\" ><img name='".$nom_editor."_cort' src='".$url_path_editor."images/cut.gif' alt='".$mesg11[$idi_edit]."' border=0 align=absmiddle ></a> ";
  $result.='</td><td valign=middle >';
  $result.= "\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; copy(); return false;\" onmouseover=\"load_image('".$nom_editor."_cop','".$url_path_editor."images/copy_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_cop','".$url_path_editor."images/copy.gif')\" ><img name='".$nom_editor."_cop' src='".$url_path_editor."images/copy.gif' alt='".$mesg12[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.= "\n	<a href='' onclick=\"obj_editor=".$nom_editor."; paste(); return false;\" onmouseover=\"load_image('".$nom_editor."_paster','".$url_path_editor."images/paste_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_paster','".$url_path_editor."images/paste.gif')\" ><img name='".$nom_editor."_paster' src='".$url_path_editor."images/paste.gif' alt='".$mesg13[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.= "\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td><td valign=middle >';
  $result.= "\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; undo(); return false;\" onmouseover=\"load_image('".$nom_editor."_ud','".$url_path_editor."images/undo_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_ud','".$url_path_editor."images/undo.gif')\" ><img name='".$nom_editor."_ud' src='".$url_path_editor."images/undo.gif' alt='".$mesg14[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.= "\n	<a href='' onclick=\"obj_editor=".$nom_editor."; redo(); return false;\" onmouseover=\"load_image('".$nom_editor."_rd','".$url_path_editor."images/redo_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_rd','".$url_path_editor."images/redo.gif')\" ><img name='".$nom_editor."_rd' src='".$url_path_editor."images/redo.gif' alt='".$mesg15[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.= "\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; load_image('".$nom_editor."_fi','".$url_path_editor."images/find.gif'); find_in_text(); return false;\" onmouseover=\"load_image('".$nom_editor."_fi','".$url_path_editor."images/find_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_fi','".$url_path_editor."images/find.gif')\" ><img name='".$nom_editor."_fi' src='".$url_path_editor."images/find.gif' alt='".$mesg16[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td>';
  $buttoncount+=6;
}
if ($this->editmode(WW_LINK))
{
  $result.='<td valign=middle >';
  $result.="\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td><td valign=middle >';
  $result.= "\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; load_image('".$nom_editor."_lnk','".$url_path_editor."images/link.gif'); insert_link(); return false;\" onmouseover=\"load_image('".$nom_editor."_lnk','".$url_path_editor."images/link_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_lnk','".$url_path_editor."images/link.gif')\" ><img name='".$nom_editor."_lnk' src='".$url_path_editor."images/link.gif' alt='".$mesg17[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td>';
  $buttoncount++;
}
/*
if ($this->editmode(WW_IMAGE))
{
  $result.='<td valign=middle >';
  $result.= "\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; load_image('".$nom_editor."_image','".$url_path_editor."images/image.gif'); insert_img(); return false;\" onmouseover=\"load_image('".$nom_editor."_image','".$url_path_editor."images/image_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_image','".$url_path_editor."images/image.gif')\" ><img name='".$nom_editor."_image' src='".$url_path_editor."images/image.gif' alt='".$mesg18[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td>';
  $buttoncount++;
}*/
/*if ($this->editmode(WW_TABLE))
{
  $result.='<td valign=middle >';
  $result.= "\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td><td valign=middle >';
  $result.= "\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; load_image('".$nom_editor."_table','".$url_path_editor."images/instable.gif');  popup_insert_table(); return false;\" onmouseover=\"load_image('".$nom_editor."_table','".$url_path_editor."images/instable_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_table','".$url_path_editor."images/instable.gif')\" ><img name='".$nom_editor."_table' src='".$url_path_editor."images/instable.gif' alt='".$mesg20[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.= "\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; table_insertrow(); return false;\" onmouseover=\"load_image('".$nom_editor."_ir','".$url_path_editor."images/insrow_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_ir','".$url_path_editor."images/insrow.gif')\" ><img name='".$nom_editor."_ir' src='".$url_path_editor."images/insrow.gif' alt='".$mesg21[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.= "\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; table_deleterow(); return false;\" onmouseover=\"load_image('".$nom_editor."_dr','".$url_path_editor."images/delrow_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_dr','".$url_path_editor."images/delrow.gif')\" ><img name='".$nom_editor."_dr' src='".$url_path_editor."images/delrow.gif' alt='".$mesg22[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; table_insertcol(); return false;\" onmouseover=\"load_image('".$nom_editor."_ic','".$url_path_editor."images/inscol_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_ic','".$url_path_editor."images/inscol.gif')\" ><img name='".$nom_editor."_ic' src='".$url_path_editor."images/inscol.gif' alt='".$mesg23[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; table_deletecol(); return false;\" onmouseover=\"load_image('".$nom_editor."_dc','".$url_path_editor."images/delcol_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_dc','".$url_path_editor."images/delcol.gif')\" ><img name='".$nom_editor."_dc' src='".$url_path_editor."images/delcol.gif' alt='".$mesg24[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; table_insertcell(); return false;\" onmouseover=\"load_image('".$nom_editor."_ice','".$url_path_editor."images/inscell_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_ice','".$url_path_editor."images/inscell.gif')\" ><img name='".$nom_editor."_ice' src='".$url_path_editor."images/inscell.gif' alt='".$mesg25[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; table_deletecell(); return false;\" onmouseover=\"load_image('".$nom_editor."_dce','".$url_path_editor."images/delcell_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_dce','".$url_path_editor."images/delcell.gif')\" ><img name='".$nom_editor."_dce' src='".$url_path_editor."images/delcell.gif' alt='".$mesg26[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; table_mergecell(); return false;\" onmouseover=\"load_image('".$nom_editor."_cc','".$url_path_editor."images/mrgcell_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_cc','".$url_path_editor."images/mrgcell.gif')\" ><img name='".$nom_editor."_cc' src='".$url_path_editor."images/mrgcell.gif' alt='".$mesg27[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; table_splitcell(); return false;\" onmouseover=\"load_image('".$nom_editor."_sc','".$url_path_editor."images/spltcell_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_sc','".$url_path_editor."images/spltcell.gif')\" ><img name='".$nom_editor."_sc' src='".$url_path_editor."images/spltcell.gif' alt='".$mesg28[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.='</td>';
  $buttoncount+=9;
}*/

if ($buttoncount>15)
{
  $result.='</tr>
            </table>
            <table cellspacing=0 cellpadding=0 border=0 >
              <tr>
                <td valign=middle >
  	              &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                </td>';
}

if ($this->editmode(WW_FONT_FACE))
{
  $result.='<td valign=middle >';
  $result.="<select name=\"".$nom_editor."_FontName\" style=\"width:140\" TITLE=\"Font Name\" LANGUAGE=\"javascript\" onchange=\"obj_editor=".$nom_editor."; return FontName_onchange(this)\"> "; 
	$result.= "<option value=\"0\" selected >".$mesg41[$idi_edit];

  $result.='	<option value="Verdana" >Verdana
      <option value="Arial" >Arial
      <option value="Tahoma">Tahoma
      <option value="Courier New">Courier New
      <option value="Times New Roman" >Times New Roman
      <option value="Wingdings">Wingdings
    </select>
  </td>';
}
if ($this->editmode(WW_FONT_SIZE))
{
  $result.='<td valign=middle >';
  $result.="<select name=\"".$nom_editor."_FontSize\" style=\"width:40\" TITLE=\"Font Size\" LANGUAGE=\"javascript\" onchange=\"obj_editor = ".$nom_editor."; return FontSize_onchange(this)\"> "; 
	$result.= "<option value=\"0\" selected >".$mesg42[$idi_edit];

  $result.='    <option value="1">1
      <option value="2">2
      <option value="3">3
      <option value="4">4
      <option value="5">5
      <option value="6">6
      <option value="7">7
    </select>
  </td>';
}
if ($this->editmode(WW_STYLE))
{
  $result.='<td valign=middle >';
  $result.="\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=\"#\"  onclick=\"obj_editor=".$nom_editor."; bold(); return false;\"  onmouseover=\"load_image('".$nom_editor."_negr','".$url_path_editor."images/bold_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_negr','".$url_path_editor."images/bold.gif')\" ><img src='".$url_path_editor."images/bold.gif' alt='".$mesg29[$idi_edit]."' border=0 align=absmiddle name='".$nom_editor."_negr' ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; italic(); return false;\" onmouseover=\"load_image('".$nom_editor."_curs','".$url_path_editor."images/italic_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_curs','".$url_path_editor."images/italic.gif')\" ><img name='".$nom_editor."_curs' src='".$url_path_editor."images/italic.gif' alt='".$mesg30[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; underline(); return false;\" onmouseover=\"load_image('".$nom_editor."_subr','".$url_path_editor."images/under_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_subr','".$url_path_editor."images/under.gif')\" ><img name='".$nom_editor."_subr' src='".$url_path_editor."images/under.gif' alt='".$mesg31[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td>';
}
if ($this->editmode(WW_COLOR))
{
  $result.='<td valign=middle >';
  $result.="\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; load_image('".$nom_editor."_fg','".$url_path_editor."images/fgcolor.gif'); popup_fg_color(); return false;\" onmouseover=\"load_image('".$nom_editor."_fg','".$url_path_editor."images/fgcolor_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_fg','".$url_path_editor."images/fgcolor.gif')\" ><img name='".$nom_editor."_fg' src='".$url_path_editor."images/fgcolor.gif' alt='".$mesg32[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; load_image('".$nom_editor."_bg','".$url_path_editor."images/bgcolor.gif'); popup_bg_color(); return false;\" onmouseover=\"load_image('".$nom_editor."_bg','".$url_path_editor."images/bgcolor_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_bg','".$url_path_editor."images/bgcolor.gif')\" ><img name='".$nom_editor."_bg' src='".$url_path_editor."images/bgcolor.gif' alt='".$mesg33[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td>';
}
if ($this->editmode(WW_ALIGN))
{
  $result.='<td valign=middle >';
  $result.="\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; align_left(); return false;\" onmouseover=\"load_image('".$nom_editor."_ae','".$url_path_editor."images/left_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_ae','".$url_path_editor."images/left.gif')\" ><img name='".$nom_editor."_ae' src='".$url_path_editor."images/left.gif' alt='".$mesg34[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; align_center(); return false;\" onmouseover=\"load_image('".$nom_editor."_center','".$url_path_editor."images/center_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_center','".$url_path_editor."images/center.gif')\" ><img name='".$nom_editor."_center' src='".$url_path_editor."images/center.gif' alt='".$mesg35[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href='' onclick=\"obj_editor=".$nom_editor."; align_right(); return false;\" onmouseover=\"load_image('".$nom_editor."_ad','".$url_path_editor."images/right_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_ad','".$url_path_editor."images/right.gif')\" ><img name='".$nom_editor."_ad' src='".$url_path_editor."images/right.gif' alt='".$mesg36[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td>';
}
if ($this->editmode(WW_LIST))
{
  $result.='<td valign=middle >';
  $result.="\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; list_numbers(); return false;\" onmouseover=\"load_image('".$nom_editor."_nl','".$url_path_editor."images/numlist_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_nl','".$url_path_editor."images/numlist.gif')\" ><img name='".$nom_editor."_nl' src='".$url_path_editor."images/numlist.gif' alt='".$mesg37[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href='' onclick= \"obj_editor=".$nom_editor."; list_bullets(); return false;\" onmouseover=\"load_image('".$nom_editor."_ul','".$url_path_editor."images/bullist_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_ul','".$url_path_editor."images/bullist.gif')\" ><img name='".$nom_editor."_ul' src='".$url_path_editor."images/bullist.gif' alt='".$mesg38[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td>';
}
if ($this->editmode(WW_INDENT))
{
  $result.='<td valign=middle >';
  $result.="\n	<img src='".$url_path_editor."images/separator.gif' border=0 align=absmiddle > "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; deindent(); return false;\" onmouseover=\"load_image('".$nom_editor."_deind','".$url_path_editor."images/deindent_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_deind','".$url_path_editor."images/deindent.gif')\" ><img name='".$nom_editor."_deind' src='".$url_path_editor."images/deindent.gif' alt='".$mesg39[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td><td valign=middle >';
  $result.="\n	<a href=''  onclick=\"obj_editor=".$nom_editor."; indent(); return false;\" onmouseover=\"load_image('".$nom_editor."_ind','".$url_path_editor."images/inindent_focus.gif');\" onmouseout=\"load_image('".$nom_editor."_ind','".$url_path_editor."images/inindent.gif')\" ><img name='".$nom_editor."_ind' src='".$url_path_editor."images/inindent.gif' alt='".$mesg40[$idi_edit]."' border=0 align=absmiddle ></a> "; 
  $result.='</td>';
}
$result.='<td align="right">'.$viewselectbox.'</td>';
// Objetos para tablas y para acceder propiedades (si se necesita):
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
	$result.= "<embed type=\"application/x-eskerplus\" id=\"".$nom_editor."\" classid=\"clsid:2D360201-FFF5-11d1-8D03-00A0C959BC0A\"  height=".$editor_height." width=".$editor_width." VIEWASTEXT >";
}
else
{
	$result.= "<object ID=\"".$nom_editor."\" CLASSID=\"clsid:2D360201-FFF5-11D1-8D03-00A0C959BC0A\" height=".$editor_height." width=".$editor_width." VIEWASTEXT >";
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
	$result.= "\n ".$nom_editor."_timerID=setInterval(\"".$nom_editor."_inicial()\",100);";
	$result.= "\n function ".$nom_editor."_inicial(){ ";
		$result.= "\n if( window[\"PropertyAccessor\"] && window[\"".$nom_editor."\"]){ ";
			$result.= "\n obj_editor = ".$nom_editor.";";
			$result.= "\n ".PropAcce_string("Set",$nom_editor,"DocumentHTML",$current_value_aux).";";
			$result.= "\n clearInterval(".$nom_editor."_timerID);";
		$result.= "\n } ";
		$result.= "\n return true;";
	$result.= "\n } ";
}
else
{
	$result.= "\n ".$nom_editor."_timerID=setInterval(\"".$nom_editor."_inicial()\",100);";
	$result.= "\n function ".$nom_editor."_inicial(){ ";
		$result.= "\n if( document[\"".$nom_editor."\"]){ ";
			$result.= "\n obj_editor = document.".$nom_editor.";";
			$result.= "\n document.".$nom_editor.".DocumentHTML = ".$current_value_aux;
			$result.= "\n clearInterval(".$nom_editor."_timerID);";
		$result.= "\n } ";
		$result.= "\n return true;";
	$result.= "\n } ";
}
$result.= "\n </script> ";

$insertat_editor = 1;
?>