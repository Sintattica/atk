<?
  include_once("atk/class.atknode.inc");

  $g_layout->output("<html>");
  $g_layout->head($txt_app_title);
  $g_layout->body();
  $g_layout->output("<br>");
  $g_layout->ui_top($txt_app_shorttitle);

  $g_layout->output ("<br><br><br><br>$txt_app_description<br><br>");

  $g_layout->ui_bottom();
  $g_layout->output("</body>");
  $g_layout->output("</html>");
  
  $g_layout->outputFlush();

?>
