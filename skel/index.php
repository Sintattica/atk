<?php
  require_once("atk/class.atknode.inc");
 
  $g_layout->output('<html>');
  $g_layout->head(text("app_title"));
  
  if(strtolower($config_menu_pos) == "top")
  {
    if($config_top_frame==1)
    {
       $g_layout->output('
        <frameset rows="70,*" frameborder="0" border="0">
          <frame name="top" scrolling="no" noresize src="top.php" marginwidth="0" marginheight="0">
       ');    
    }
    $g_layout->output('
               <frameset rows="100,*" frameborder="0" border="0">
                  <frame name="menu" scrolling="no"   noresize src="menu.php"   marginwidth="0" marginheight="0">
                  <frame name="main" scrolling="auto" noresize src="welcome.php" marginwidth="0" marginheight="0">
    ');
    if($config_top_frame==1) { $g_layout->output('</frameset>'); }
    $g_layout->output('
                   <noframes>
                    <body bgcolor="#CCCCCC" text="#000000">
                      <p>Your browser doesnt support frames, but this is required to run <? echo $txt_app_title; ?></p>
                    </body>
                  </noframes>
               </frameset>
               </html>
                   ');
  }
  if(strtolower($config_menu_pos) == "bottom")
  {
    if($config_top_frame==1)
    {
       $g_layout->output('
        <frameset rows="70,*" frameborder="0" border="0">
          <frame name="top" scrolling="no" noresize src="top.php" marginwidth="0" marginheight="0">
       ');    
    }
    $g_layout->output('
               <frameset rows="*,100" frameborder="0" border="0">
                  <frame name="main" scrolling="auto" noresize src="welcome.php" marginwidth="0" marginheight="0">
                  <frame name="menu" scrolling="no"   noresize src="menu.php"   marginwidth="0" marginheight="0">
    ');
    if($config_top_frame==1) { $g_layout->output('</frameset>'); }
    
    $g_layout->output('
                   <noframes>
                    <body bgcolor="#CCCCCC" text="#000000">
                      <p>Your browser doesnt support frames, but this is required to run <? echo $txt_app_title; ?></p>
                    </body>
                  </noframes>
               </frameset>
               </html>
                   ');
  }
  elseif(strtolower($config_menu_pos) == "left")
  {
    if($config_top_frame==1)
    {
       $g_layout->output('
        <frameset rows="70,*" frameborder="0" border="0">
          <frame name="top" scrolling="no" noresize src="top.php" marginwidth="0" marginheight="0">
       ');    
    }
    $g_layout->output('
      <frameset cols="190,*" frameborder="0" border="0">
        <frame name="menu" scrolling="no" noresize src="menu.php" marginwidth="0" marginheight="0">
        <frame name="main" scrolling="auto" noresize src="welcome.php" marginwidth="0" marginheight="0">
    ');
    if($config_top_frame==1) { $g_layout->output('</frameset>'); }

    $g_layout->output('
        <noframes>
          <body bgcolor="#CCCCCC" text="#000000">
            <p>Your browser doesnt support frames, but this is required to run <? echo $txt_app_title; ?></p>
          </body>
        </noframes>
      </frameset>
      </html>
       ');
  }
  elseif(strtolower($config_menu_pos)=="right")
  {
    if($config_top_frame==1)
    {
       $g_layout->output('
        <frameset rows="70,*" frameborder="0" border="0">
          <frame name="top" scrolling="no" noresize src="top.php" marginwidth="0" marginheight="0">
       ');    
    }
    $g_layout->output('
      <frameset cols="*,190" frameborder="0" border="0">
        <frame name="main" scrolling="auto" noresize src="welcome.php" marginwidth="0" marginheight="0">
        <frame name="menu" scrolling="no" noresize src="menu.php" marginwidth="0" marginheight="0">
    ');
    if($config_top_frame==1) { $g_layout->output('</frameset>'); }

    $g_layout->output('
        <noframes>
          <body bgcolor="#CCCCCC" text="#000000">
            <p>Your browser doesnt support frames, but this is required to run <? echo $txt_app_title; ?></p>
          </body>
        </noframes>
      </frameset>
      </html>
       ');
  }

  $g_layout->outputFlush();
?>
