<?php

 /* help.php
  *
  * descr.  : Opens a new page in the same atk-template style as the atk-application, in a new pop-up screen and shows a help page.
  * input   : $node     -> name of the to be included text file ( help.[node].inc) 
  * example : help.php?node=[node]
  *
  * @author : Rene Bakx (rene@ibuildings.nl)
  * @version: $Revision$
  *
  * $Id$
  *
  */
  
  include("atk.inc");  
  
  atksession();
  atksecure();  

//  Renders the help screen
  $language = strtok(atkconfig("languagefile"),".");
  $title1 ="txt_title_".$node;
  $title = $$title1;
  $file   = $config_atkroot."help/".$language."/help.".$node.".inc";
  $data = '<div align="left">';
  $data .= implode("<br>",file($file));
  $data .='</div>';
  $g_layout->output("<html>");
  $g_layout->head("Help: ".$txt_app_shorttitle);
  $g_layout->body();
  $g_layout->output("<br>");
  $g_layout->ui_top($title);
  $g_layout->output ($data);
  $g_layout->ui_bottom();
  $g_layout->output("<br>");
  $g_layout->output('<div align="right"><a href="javascript:window.close();">'.text("close").'</a></div');
  $g_layout->output("</body>");
  $g_layout->output("</html>");
  $g_layout->outputFlush();

?>


