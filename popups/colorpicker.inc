<?php   

 /* colorpicker.php
  *
  * descr.  : Show the colorpicker and returns the value to the the text field
  * input   : $form.field     -> name of the form and field from wich the picker is called
  * call    : colorpicker.php?form=[form.field]
  *
  * @author : Rene Bakx (rene@ibuildings.nl)
  * @version: $Revision$
  *
  * $Id$
  *
  */

  include_once($config_atkroot."atk.inc");  
  
  atksession("admin");
  atksecure();  
  
  useattrib("atkcolorpickerattribute");
  
 // builds matrix

   $colHeight = "11"; // height of each color element
   $colWidth  = "11";   // width of each color element
   $formRef   = $_GET["field"];
   $matrix    = colorMatrix($colHeight,$colWidth,$formRef,0,$_GET["usercol"]);
   $prefix    = $config_atkroot."atk/images/";

   $layout .= "<form name='entryform'>";
   $layout .= "<table width='100%' border='0' cellpadding='1' cellspacing='0' style='border: 1px solid #000000;'>";
   $layout .= "<tr bgcolor='#FFFFFF'>";
   $layout .= " <td valign='top' align='left'>".$matrix[0]."</td>";
   $layout .= " <td valign='top' align='left'>".$matrix[1]."</td>";
   $layout .= " <td valign='top' align='left'>".$matrix[2]."</td>";
   $layout .= "</tr>";
   $layout .= "<tr bgcolor='#FFFFFF'>"; 
   $layout .= " <td valign='top' align='left'>".$matrix[3].$matrix[4]."</td>";
   $layout .= " <td valign='top' align='left'>".$matrix[5].$matrix[6]."<br></td>";
   $layout .= " <td valign='top' align='right' class='table'>";
   $layout .= "  &nbsp;Kleurcode: &nbsp;<input type='text' name='".$formRef."' size='7' maxlength='7' value='' style='font-family: verdana; font-size: 11px;'>&nbsp;";
   $layout .= " </td>";
   $layout .= "</tr>";
   $layout .= "<tr bgcolor='#FFFFFF'>";
   $layout .= " <td colspan='2' valign='top' align='left'>".$matrix[7]."</td>";
   $layout .= " <td valign='top' align='right'>";
   $layout .= " <input type='button' name='cancel' value='Annuleren' style='width: 75px; font-family: verdana; font-size: 11px;' onClick='window.close();'>&nbsp;";
   $layout .= " <input type='button' name='close' value='Toepassen'  style='width: 75px; font-family: verdana; font-size: 11px;' onClick='remoteUpdate(\"".$formRef."\", \"".$prefix."\");'>&nbsp;<br><br>";
   $layout .= " </td>";
   $layout .= "</tr>";
   $layout .= "</table>";
   $layout .= "</form>";
   $style     = $config_atkroot."atk/style.php";

  //  Display's the picker in the current ATK style-template
   $g_layout->output("<html>");
   $g_layout->head(text("colorpicker"));
   $g_layout->output('<script language="javascript" src="'.$config_atkroot.'atk/javascript/colorpicker.js"></script>');
   $g_layout->output('<link href="'.$style.'" rel="stylesheet" type="text/css">');
   $g_layout->body();
   $g_layout->output ("<div align='center'>".$layout."</div>");
   $g_layout->output("</body>");
   $g_layout->output("</html>");
   $g_layout->outputFlush();
?>