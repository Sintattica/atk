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
  
  atksession("popup");
  atksecure();  
  
  useattrib("atkcolorpickerattribute");
  
 // builds matrix
   $colHeight = "9"; // height of each color element
   $colWidth = "9";   // width of each color element
   $formRef   = $field;
   $matrix = colorMatrix($colHeight,$colWidth,$formRef,1,$usercol);
  //  Display's the picker in the current ATK style-template
  $g_layout->output("<html>");
  $g_layout->head("ColorPicker");
  $g_layout->output('<script language="javascript" src="'.$config_atkroot.'atk/javascript/colorpicker.js"></script>');
  $g_layout->body();
  $g_layout->output ($matrix);
  $g_layout->output("</body>");
  $g_layout->output("</html>");
  $g_layout->outputFlush();
?>
