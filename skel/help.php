<?
/* help.php
*
*  @Author : Rene Bakx (rene@ibuildings.nl)
*  @Version: $Revision$
*
* descr.  : Opens a new page in the same atk-template style as the atk-application, in a new pop-up screen and shows a help page.
*  input   :  $node     -> name of the to be included text file ( help.[node].inc) 
*  call     : help.php?node=[node]
*
* $Id$
* $Log$
* Revision 1.1  2001/07/04 10:51:07  ivo
* help file moved to skel
*
* Revision 4.2  2001/06/29 13:15:12  ivo
* Added cvs headers to file.
*
*
*/

  chdir("../");    
  include("atk/class.atknode.inc");  

//  Renders the help screen
  $language = strtok(atkconfig("languagefile"),".");
  $title ="$txt_title_".$node;
  $file   = "help/".$language."/help.".$node.".inc";
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


