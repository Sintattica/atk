 
 
 // Esta información corresponde al archivo Dhtmled.js 
 // DHTML Editing Component Constants for JavaScript 
 // 
 // Command IDs 
 // 
 DECMD_BOLD =                      5000 
 DECMD_COPY =                      5002 
 DECMD_CUT =                       5003 
 DECMD_DELETE =                    5004 
 DECMD_DELETECELLS =               5005 
 DECMD_DELETECOLS =                5006 
 DECMD_DELETEROWS =                5007 
 DECMD_FINDTEXT =                  5008 
 DECMD_FONT =                      5009 
 DECMD_GETBACKCOLOR =              5010 
 DECMD_GETBLOCKFMT =               5011 
 DECMD_GETBLOCKFMTNAMES =          5012 
 DECMD_GETFONTNAME =               5013 
 DECMD_GETFONTSIZE =               5014 
 DECMD_GETFORECOLOR =              5015 
 DECMD_HYPERLINK =                 5016 
 DECMD_IMAGE =                     5017 
 DECMD_INDENT =                    5018 
 DECMD_INSERTCELL =                5019 
 DECMD_INSERTCOL =                 5020 
 DECMD_INSERTROW =                 5021 
 DECMD_INSERTTABLE =               5022 
 DECMD_ITALIC =                    5023 
 DECMD_JUSTIFYCENTER =             5024 
 DECMD_JUSTIFYLEFT =               5025 
 DECMD_JUSTIFYRIGHT =              5026 
 DECMD_LOCK_ELEMENT =              5027 
 DECMD_MAKE_ABSOLUTE =             5028 
 DECMD_MERGECELLS =                5029 
 DECMD_ORDERLIST =                 5030 
 DECMD_OUTDENT =                   5031 
 DECMD_PASTE =                     5032 
 DECMD_REDO =                      5033 
 DECMD_REMOVEFORMAT =              5034 
 DECMD_SELECTALL =                 5035 
 DECMD_SEND_BACKWARD =             5036 
 DECMD_BRING_FORWARD =             5037 
 DECMD_SEND_BELOW_TEXT =           5038 
 DECMD_BRING_ABOVE_TEXT =          5039 
 DECMD_SEND_TO_BACK =              5040 
 DECMD_BRING_TO_FRONT =            5041 
 DECMD_SETBACKCOLOR =              5042 
 DECMD_SETBLOCKFMT =               5043 
 DECMD_SETFONTNAME =               5044 
 DECMD_SETFONTSIZE =               5045 
 DECMD_SETFORECOLOR =              5046 
 DECMD_SPLITCELL =                 5047 
 DECMD_UNDERLINE =                 5048 
 DECMD_UNDO =                      5049 
 DECMD_UNLINK =                    5050 
 DECMD_UNORDERLIST =               5051 
 DECMD_PROPERTIES =                5052 
 
 // 
 // Enums 
 // 
 
 // OLECMDEXECOPT   
 OLECMDEXECOPT_DODEFAULT =         0  
 OLECMDEXECOPT_PROMPTUSER =        1 
 OLECMDEXECOPT_DONTPROMPTUSER =    2 
 
 // DHTMLEDITCMDF 
 DECMDF_NOTSUPPORTED =             0  
 DECMDF_DISABLED =                 1  
 DECMDF_ENABLED =                  3 
 DECMDF_LATCHED =                  7 
 DECMDF_NINCHED =                  11 
 
 // DHTMLEDITAPPEARANCE 
 DEAPPEARANCE_FLAT =               0 
 DEAPPEARANCE_3D =                 1  
 
 // OLE_TRISTATE 
 OLE_TRISTATE_UNCHECKED =          0 
 OLE_TRISTATE_CHECKED =            1 
 OLE_TRISTATE_GRAY =               2 
 var obj_editor = 0 ; 
 // Per canvis de gifs a la barra d'icones: 
 
 function load_image(name,imgfile)
 { 
   document.images[name].src = imgfile; 
   return true; 
 } 
 
 // Utilitats auxiliars per paletes de colors: 
 
 function MakeArray(n) 
 { 
   this.length=n 
   for(var j=1; j<=n; j++)
   { 
     this[n]=0 
   } 
   return this  
 } 
 
 colors= new MakeArray(140); 
 colors[0]='aliceblue' 
 colors[1]='antiquewhite' 
 colors[2]='aqua' 
 colors[3]='aquamarine' 
 colors[4]='azure' 
 colors[5]='beige' 
 colors[6]='bisque' 
 colors[7]='black' 
 colors[8]='blanchedalmond' 
 colors[9]='blue' 
 colors[10]='blueviolet' 
 colors[11]='brown' 
 colors[12]='burlywood' 
 colors[13]='cadetblue' 
 colors[14]='chartreuse' 
 colors[15]='chocolate' 
 colors[16]='coral' 
 colors[17]='cornflower' 
 colors[18]='cornsilk' 
 colors[19]='crimson' 
 colors[20]='cyan' 
 colors[21]='darkblue' 
 colors[22]='darkcyan' 
 colors[23]='darkgoldenrod' 
 colors[24]='darkgray' 
 colors[25]='darkgreen' 
 colors[26]='darkkhaki' 
 colors[27]='darkmagenta' 
 colors[28]='darkolivegreen' 
 colors[29]='darkorange' 
 colors[30]='darkorchid' 
 colors[31]='darkred' 
 colors[32]='darksalmon' 
 colors[33]='darkseagreen' 
 colors[34]='darkslateblue' 
 colors[35]='darkslategray' 
 colors[36]='darkturquoise' 
 colors[37]='darkviolet' 
 colors[38]='deeppink' 
 colors[39]='deepskyblue' 
 colors[40]='dimgray' 
 colors[41]='dodgerblue' 
 colors[42]='firebrick' 
 colors[43]='floralwhite' 
 colors[44]='forestgreen' 
 colors[45]='fuchia' 
 colors[46]='gainsboro' 
 colors[47]='ghostwhite' 
 colors[48]='gold' 
 colors[49]='goldenrod' 
 colors[50]='gray' 
 colors[51]='green' 
 colors[52]='greenyellow' 
 colors[53]='honeydew' 
 colors[54]='hotpink' 
 colors[55]='indianred' 
 colors[56]='indigo' 
 colors[57]='ivory' 
 colors[58]='khaki' 
 colors[59]='lavender' 
 colors[60]='lavenderblush' 
 colors[61]='lawngreen' 
 colors[62]='lemonchiffon' 
 colors[63]='lightblue' 
 colors[64]='lightcoral' 
 colors[65]='lightcyan' 
 colors[66]='lightgoldenrodyellow' 
 colors[67]='lightgreen' 
 colors[68]='lightgrey' 
 colors[69]='lightpink' 
 colors[70]='lightsalmon' 
 colors[71]='lightseagreen' 
 colors[72]='lightskyblue' 
 colors[73]='lightslategray' 
 colors[74]='lightsteelblue' 
 colors[75]='lightyellow' 
 colors[76]='lime' 
 colors[77]='limegreen' 
 colors[78]='linen' 
 colors[79]='magenta' 
 colors[80]='maroon' 
 colors[81]='mediumaquamarine' 
 colors[82]='mediumblue' 
 colors[83]='mediumorchid' 
 colors[84]='mediumpurple' 
 colors[85]='mediumseagreen' 
 colors[86]='mediumslateblue' 
 colors[87]='mediumspringgreen' 
 colors[88]='mediumturquoise' 
 colors[89]='mediumvioletred' 
 colors[90]='midnightblue' 
 colors[91]='mintcream' 
 colors[92]='mistyrose' 
 colors[93]='moccasin' 
 colors[94]='navajowhite' 
 colors[95]='navy' 
 colors[96]='oldlace' 
 colors[97]='olive' 
 colors[98]='olivedrab' 
 colors[99]='orange' 
 colors[100]='orangered' 
 colors[101]='orchid' 
 colors[102]='palegoldenrod' 
 colors[103]='palegreen' 
 colors[104]='paleturquoise' 
 colors[105]='palevioletred' 
 colors[106]='papayawhip' 
 colors[107]='peachpuff' 
 colors[108]='peru' 
 colors[109]='pink' 
 colors[110]='plum' 
 colors[111]='powderblue' 
 colors[112]='purple' 
 colors[113]='red' 
 colors[114]='rosybrown' 
 colors[115]='royalblue' 
 colors[116]='saddlebrown' 
 colors[117]='salmon' 
 colors[118]='sandybrown' 
 colors[119]='seagreen' 
 colors[120]='seashell' 
 colors[121]='sienna' 
 colors[122]='silver' 
 colors[123]='skyblue' 
 colors[124]='slateblue' 
 colors[125]='slategray' 
 colors[126]='snow' 
 colors[127]='springgreen' 
 colors[128]='steelblue' 
 colors[129]='tan' 
 colors[130]='teal' 
 colors[131]='thistle' 
 colors[132]='tomato' 
 colors[133]='turquoise' 
 colors[134]='violet' 
 colors[135]='wheat' 
 colors[136]='white' 
 colors[137]='whitesmoke' 
 colors[138]='yellow' 
 colors[139]='yellowgreen' 
 
 function color_table()
 { 
   var t=0,taco 
   taco='<center><br><br><table border=1 cellspacing=0 cellpadding=0>'; 
   while(t<140)
   { 
     if(t%16==0)
     { 
       if(t!=0)
       { 
         taco+='</tr>' 
       } 
       taco+='<tr>' 
     } 
     taco+='<td bgcolor="'+colors[t]+'" ><a href=javascript:canvi("'+colors[t]+'"); ><img src="atk/images/dummy.gif" border=0 width=18 height=18 alt="'+colors[t]+'"></a></td>'; 
     t++ 
   } 
   taco+='</tr></table></center>' 
   return taco 
 } 
 
 function colorpicker(ruta_funct){ 
 var pal_col, k, tc 
 pal_col=window.open("","colorpicker","screenX=80,screenY=80,width=360,height=250") 
 pal_col.document.open() 
 k=pal_col.document; 
 k.writeln("<html><head><style> td,body { font-family:Arial; font-size:8pt; } </style> <script> function canvi(hexa) { "+ruta_funct+"(hexa); window.close(); }</"+"script></head><body bgcolor=white ><center>") 
 k.writeln("<font color=black face=arial size=-1 ><b> Pick a color from the list below:</b></font>") 
 tc=color_table() 
 k.writeln(tc) 
 k.writeln("</center></body></html>") 
 k.close() 
 pal_col.focus() 
 } 
 function get_html(nom_editor) 
 { 
   var obj_ed = eval("document." + nom_editor); 
   var cont = obj_ed.DocumentHTML; 
   var texto = "" + cont 
   var complet = eval(nom_editor + '_doc_complet');
   if(!complet)
   { 
	   texto = strip_body(texto); 
   } 
   return texto 
 }   
 
 function set_html(nom_editor,contingut) { 
 var obj_ed = eval("document." + nom_editor); 
 obj_ed.DocumentHTML = contingut;
 } 
 
 function strip_body(cont) 
 { 
   var  ini_cos = cont.search(/<BODY/i); 
   if( ini_cos == -1 )
   { 
     return cont; 
   } 
   var  lon = cont.length 
   var  fi = false 
   var prob = false 
   var  i = ini_cos + 5 
   while( !fi )
   { 
     car = cont.charAt(i); 
     if( car == '>' )
     { 
       ini_cos = i + 1  
       fi = true 
     } 
     if( car == '"' || car == "'" )
     { 
       fi_com = false 
       i++ 
       if( i >= lon )
       { 
         fi = true; 
         prob = true; 
         fi_com = true; 
       } 
       while( !fi_com )
       { 
         car_aux = cont.charAt(i); 
         if( car_aux == car )
         { 
           fi_com = true 
         } 
         else 
         { 
           i++; 
         } 
         if( i >= lon )
         { 
           fi = true; 
           prob = true; 
           fi_com = true; 
         } 
       } 
     } 
     i++; 
     if( i >= lon )
     { 
       fi = true; 
       prob = true; 
     } 
   } 
   if( prob == true )
   { 
     alert('Due to problems with the HTML code of the page it is not possible to execute this action.'); 
   } 
   else 
   { 
     var fi_cos = cont.search(/<\/BODY/i); 
     var aux = cont.substring(ini_cos,fi_cos)
     cont = aux 
   } 
   return cont;
 }
 
 function clear_doc()
 { 
   if (confirm('Are you sure you want to clear the text?'))
   { 
     obj_editor.NewDocument(); 
   } 
 } 
 
 function cut() 
 { 
   obj_editor.ExecCommand(DECMD_CUT,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function copy() 
 { 
   obj_editor.ExecCommand(DECMD_COPY,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function paste()
 { 
   obj_editor.ExecCommand(DECMD_PASTE,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function undo() 
 { 
   obj_editor.ExecCommand(DECMD_UNDO,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function redo() 
 { 
   obj_editor.ExecCommand(DECMD_REDO,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function find_in_text() 
 { 
   obj_editor.ExecCommand(DECMD_FINDTEXT,OLECMDEXECOPT_PROMPTUSER); 
   //obj_editor.focus(); 
 } 
 
 function insert_link() 
 { 
   obj_editor.ExecCommand(DECMD_HYPERLINK,OLECMDEXECOPT_PROMPTUSER); 
   //obj_editor.focus(); 
 } 
 
 function insert_img() 
 { 
   obj_editor.ExecCommand(DECMD_IMAGE,OLECMDEXECOPT_PROMPTUSER); 
   //obj_editor.focus(); 
 } 
 
 function popup_doc_bgcolor()
 { 
   colorpicker('opener.doc_bgcolor_set') 
   return true; 
 } 
 
 function replacer()
 {
   // TODO: create cool own generic extension.
   alert('mijn eigen nieuwe replacer funktie');
   var content = obj_editor.DocumentHTML;
   
   var selection = obj_editor.DOM.selection.createRange();
   alert(selection.text);
   selection.text = 'ivo'; 
 }
 
 function doc_bgcolor_set(hexa_color) 
 { 
   var content = obj_editor.DocumentHTML;
   var codi, noucodi, lon, loc, k, car, car_aux, a, codi_result 
   codi = new String(content); 
   lon = codi.length 
   loc = codi.search(/<BODY/i); 
   fi = false; 
   prob = false; 
   i = loc + 5; 
   loc_bg = i 
   loc_bg_fi = i 
   while( !fi )
   { 
     car = codi.charAt(i); 
     if( car == '>' )
     { 
       loc_bg = i  
       loc_bg_fi = i 
       fi = true 
     } 
     if( codi.substr(i,8) == 'BGCOLOR=' )
     { 
       loc_bg = i + 8; 
       // Cal trobar el fi del valor de l'atribut 
       while( codi.charAt(i) != ' ' )
       { 
         i++; 
       } 
       loc_bg_fi = i; 
       fi = true; 
     } 
     if( codi.substr(i,8) == 'bgcolor=' )
     { 
       loc_bg = i + 8; 
       fi = true; 
     } 
     if( car == '"' || car == "'" )
     { 
       fi_com = false 
       i++ 
       if( i >= lon )
       { 
         fi = true; 
         prob = true; 
         fi_com = true; 
       } 
       while( !fi_com )
       { 
         car_aux = codi.charAt(i); 
         if( car_aux == car )
         { 
           fi_com = true 
         } 
         else 
         { 
           i++; 
         } 
         if( i >= lon )
         { 
           fi = true; 
           prob = true; 
           fi_com = true; 
         } 
       } 
     } 
     i++; 
     if( i >= lon )
     { 
       fi = true; 
       prob = true; 
     } 
   } 
   if( prob == true )
   { 
     alert('Due to problems with the HTML code of the page it is not possible to execute this action.'); 
   } 
   else 
   { 
     codi_result = "" + codi.substring(0,loc_bg)  
     if( loc_bg == loc_bg_fi )
     { 
       codi_result += ' BGCOLOR="' + hexa_color + '" '  
     } 
     else 
     { 
       codi_result += '"' + hexa_color + '" ' 
     } 
     codi_result += codi.substring(loc_bg_fi,lon); 
     //alert(codi_result); 
     obj_editor.DocumentHTML = codi_result; 
   } 
   return true; 
 } 
 
 function popup_insert_table() 
 { 
   var pVar = document.ObjTableInfo; 
   var NR = pVar.NumRows; 
   var NC = pVar.NumCols; 
   var TA = pVar.TableAttrs; 
   var CA = pVar.CellAttrs; 
   var funct = 'opener.insert_table' 
   var par_tab, k, tc 
   par_tab=window.open("","param_tables","screenX=80,screenY=80,width=400,height=215") 
   par_tab.document.open() 
   k=par_tab.document 
   k.writeln('<HTML><HEAD><TITLE>Definició de Taula</TITLE>') 
   k.writeln('<STYLE TYPE="text/css">')  
   k.writeln(" td,body { font-family:Arial; font-size:9pt; font-weight:bold; } ") 
   k.writeln('</STYLE>') 
   k.writeln("<script> function comprova_valors() { ") 
   k.writeln("           var nf, nc, at, ac, tit, nerr=0 , avis") 
   k.writeln("           avis = '\\nTable can´t be created due to:' ") 
   k.writeln("           nf = document.info_table.NumRows.value") 
   k.writeln("           nc = document.info_table.NumCols.value") 
   k.writeln("           at = document.info_table.TableAttrs.value") 
   k.writeln("           ac = document.info_table.CellAttrs.value") 
   k.writeln("           tit = ''") 
   k.writeln("           if( nf != parseInt(nf) || nf < 0 ){ ") 
   k.writeln("               nerr++") 
   k.writeln("               avis += '\\n\\n-The number of rows must be a positive integer.'") 
   k.writeln("           }") 
   k.writeln("           if( nc != parseInt(nc) || nc < 0 ){ ") 
   k.writeln("               nerr++") 
   k.writeln("               avis += '\\n\\n-The number of columns must be a positive integer.'") 
   k.writeln("           }") 
   k.writeln("           if( nerr == 0){ ") 
   k.writeln("               "+funct+"(nf,nc,at,ac,tit) ") 
   k.writeln("               window.close(); ") 
   k.writeln("           }") 
   k.writeln("           else") 
   k.writeln("           {") 
   k.writeln("             alert(avis)") 
   k.writeln("           }") 
   k.writeln("           return true ") 
   k.writeln("         }</"+"script>") 
   k.writeln('</HEAD><BODY bgcolor=white ><center>') 
   k.writeln('<form name=info_table onsubmit="comprova_valors();" >'); 
   k.writeln("<font color=black face=arial size=-1 ><b> Vul de eigenschappen van de tabel in, en klik op \'OK\':</b></font>") 
   k.writeln('<TABLE CELLSPACING=10><TR><TD valign=absmiddle >Aantal rijen:&nbsp;&nbsp;&nbsp;<INPUT TYPE=TEXT SIZE=3  maxlength=2 NAME=NumRows value='+NR+' ></TD>') 
   k.writeln('<TD valign=absmiddle >Kolommen:&nbsp;&nbsp;&nbsp;<INPUT TYPE=TEXT SIZE=3 maxlength=2 NAME=NumCols value='+NC+'></TD></TR>') 
   k.writeln('<TR><TD>Tabel-attributen (in htmlcodes, bijv. \'border=0\'):</TD><TD valign=absmiddle ><INPUT TYPE=TEXT SIZE=20 NAME=TableAttrs maxlength=120 value='+TA+'></TD></TR>') 
   k.writeln('<TR><TD>Cel-attributen (in htmlcodes, bijv. \'bgcolor=#ff0000\'):</TD><TD><INPUT TYPE=TEXT SIZE=20 NAME=CellAttrs value='+CA+'></TD></TR>')    
   k.writeln('<TR><TD valign=absmiddle colspan=2 align=center ><INPUT TYPE=BUTTON NAME=OK VALUE=OK onclick="comprova_valors()" ></TD></TR></TABLE></form>') 
   k.writeln('</center></BODY></HTML>') 
   k.close() 
   par_tab.focus() 
   return true 
 } 
 
 function insert_table(nf,nc,at,ac,tit) 
 { 
   var pVar = document.ObjTableInfo; 
   pVar.NumRows = nf; 
   pVar.NumCols = nc; 
   pVar.TableAttrs = at; 
   pVar.CellAttrs = ac; 
   obj_editor.ExecCommand(DECMD_INSERTTABLE,OLECMDEXECOPT_DODEFAULT, pVar); 
   return true; 
 } 
 
 function table_insertrow() 
 { 
   obj_editor.ExecCommand(DECMD_INSERTROW,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function table_deleterow() 
 { 
   obj_editor.ExecCommand(DECMD_DELETEROWS,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function table_insertcol() 
 { 
   obj_editor.ExecCommand(DECMD_INSERTCOL,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function table_deletecol() 
 { 
   obj_editor.ExecCommand(DECMD_DELETECOLS,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function table_insertcell() 
 { 
   obj_editor.ExecCommand(DECMD_INSERTCELL,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function table_deletecell() 
 { 
   obj_editor.ExecCommand(DECMD_DELETECELLS,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function table_mergecell() 
 { 
   obj_editor.ExecCommand(DECMD_MERGECELLS,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function table_splitcell() 
 { 
   obj_editor.ExecCommand(DECMD_SPLITCELL,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function FontName_onchange(sel_obj) 
 { 
   var ty = sel_obj.options[sel_obj.selectedIndex].value; 
   if( ty != 0 )
   { 
     obj_editor.ExecCommand(DECMD_SETFONTNAME, OLECMDEXECOPT_DODEFAULT, ty); 
     //obj_editor.SetFocus(); 
   } 
   sel_obj.options[0].selected = true 
 } 
 
 function FontSize_onchange(sel_obj) 
 { 
   var sz = sel_obj.options[sel_obj.selectedIndex].value; 
   if( sz != 0 )
   { 
     obj_editor.ExecCommand(DECMD_SETFONTSIZE, OLECMDEXECOPT_DODEFAULT, sz); 
     //obj_editor.focus(); 
   } 
   sel_obj.options[0].selected = true 
 } 
 
 function bold()
 {    
   obj_editor.ExecCommand(DECMD_BOLD,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
   return false; 
 } 
 
 function italic() 
 { 
   obj_editor.ExecCommand(DECMD_ITALIC,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.SetFocus(); 
 } 
 
 function underline() 
 { 
   obj_editor.ExecCommand(DECMD_UNDERLINE,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function popup_fg_color()
 { 
   colorpicker('opener.fg_color_set') 
   return true; 
 } 
 
 function fg_color_set(arr) 
 { 
   if (arr != null) 
   { 
     obj_editor.ExecCommand(DECMD_SETFORECOLOR,OLECMDEXECOPT_DODEFAULT, arr); 
   } 
 } 
 
 function popup_bg_color()
 { 
   colorpicker('opener.bg_color_set') 
   return true; 
 } 
 
 function bg_color_set(arr) 
 { 
   if (arr != null) 
   { 
     obj_editor.ExecCommand(DECMD_SETBACKCOLOR,OLECMDEXECOPT_DODEFAULT, arr); 
   } 
 } 
 
 function align_right() 
 { 
   obj_editor.ExecCommand(DECMD_JUSTIFYRIGHT,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function align_center() 
 { 
   obj_editor.ExecCommand(DECMD_JUSTIFYCENTER,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function align_left() 
 { 
   obj_editor.ExecCommand(DECMD_JUSTIFYLEFT,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function list_numbers() 
 { 
   obj_editor.ExecCommand(DECMD_ORDERLIST,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function list_bullets() 
 { 
   obj_editor.ExecCommand(DECMD_UNORDERLIST,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function indent() 
 { 
   obj_editor.ExecCommand(DECMD_INDENT,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function deindent() 
 { 
   obj_editor.ExecCommand(DECMD_OUTDENT,OLECMDEXECOPT_DODEFAULT); 
   //obj_editor.focus(); 
 } 
 
 function modifySelection(pre, post)
{
  if (!document.selection) return;
 
  var selection = obj_editor.DOM.selection.createRange();
  var current   = selection.text;
 
  if (current == '') return;
 
  selection.text = pre + current + post;
  selection.parentElement().focus();
}

function popupSelection(url,title)
{
  if (!document.selection)
  {   
   alert("Selecteer eerst het stuk tekst waar u een link van wilt maken");
   return;
  }
  
  var selection = obj_editor.DOM.selection.createRange();
  var current   = selection.text;

  if (current == '')
  {
    alert("Selecteer eerst het stuk tekst waar u een link van wilt maken");
    return;
  }
    
  NewWindow(url,title,400,400,'yes');
}