<? require "atk/class.atknode.inc"; 

  $g_layout->output("<html>");
  $g_layout->head($txt_app_title);
  $g_layout->body();
  $g_layout->output("<br><div align='center'>"); 
  $g_layout->ui_top("Menu");
  $g_layout->output("<br>");

  require "config_modules.inc";

  $menu = "&nbsp;";
  for ($i=0; $i<count($modules); $i++)
  {
    // if $modules[$i] == - then place a <br> in the menu.
    if ($modules[$i]!="-")
    {
      
      $modname = $modules[$i];
      $title   = "title_".$modname;
      $url     = $modname."_url";
      $menu   .= '<a href="'.$$url.'" target="main">'.text($title).'</a>';
      
      if ($i < count($modules)-1) { $menu .= ' '.$config_menu_delimiter.' '; }
   }
   else
   {
      $menu .= '<br>';
   }
   
  }

  $g_layout->output($menu);
  $g_layout->output("<br><br>");
  $g_layout->ui_bottom();
  $g_layout->output("</div>"); 
  $g_layout->output("</body>"); 
  $g_layout->output("</html>"); 
  $g_layout->outputFlush();
?>
