<?php
  $config_atkroot = "./";
  require_once($config_atkroot."atk/class.atknode.inc"); 

  atksecure();

  $g_layout->output("<html>");
  $g_layout->head($txt_app_title);
  $g_layout->body();
  $g_layout->ui_top("Top Frame");
  $g_layout->output("<br> This is  your first top frame<br>&nbsp;");
  $g_layout->ui_bottom();
  $g_layout->outputFlush();
?>
