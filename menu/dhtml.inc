<?
  // USAGE:
  //
  // Just set the following in your config.inc.php 
  //
  // $config_menu_layout   = "dhtml";
  // $config_tabselectMode = "mouseover"; (can also be "click") 

  $tabs = "";
  $divs = "";
  $tab  = 1;

  while (list ($name) = each ($g_menu))
  {
   $tabContent = "";
   $atkmenutop = $name;
   $tabName    = addslashes(text("menu_".$atkmenutop));
   $items      = 0;

   for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
   {
    $menu   = "";
    $name   = $g_menu[$atkmenutop][$i]["name"];
    $url    = session_url($g_menu[$atkmenutop][$i]["url"],SESSION_NEW);
    $enable = $g_menu[$atkmenutop][$i]["enable"];

    if (is_array($enable))
    {
     $enabled = false;
     for ($j=0;$j<(count($enable)/2);$j++)
     {
      $enabled |= is_allowed($enable[(2*$j)],$enable[(2*$j)+1]);
     }
     $enable = $enabled;
    }

    /* delimiter ? */
    if ($g_menu[$atkmenutop][$i]["name"] == "-") $menu .= "";
    
    /* normal menu item */   
    else if ($enable)        
    {            
     if($g_menu[$atkmenutop][$i]["url"]!="")
     {    
      $tabContent .= "<a target='main' class='tablink' href='$url'>".text("menu_$name")."</a>";

      if ($i < count($g_menu[$atkmenutop])-1)
      {
       $tabContent .= "&nbsp;|&nbsp;";
      }

      $items++;
     }
    }
   }

   if ($items > 0)
   {
    $tabs   .= '   rows[1]['.$tab.'] = "'.$tabName.'"'."\n";
    $divs   .= '<div id="T1'.$tab.'" class="tab-body">'.$tabContent.'</div>'."\n";
    $tab++;
   }
  }   

  // add options tab containing logout
   $tabs   .= '   rows[1]['.$tab.'] = "Opties"'."\n";
   $divs   .= '<div id="T1'.$tab.'" class="tab-body"><a class="tablink" href="index.php?atklogout=1" target="_top">'.text("logout").'</a></div>'."\n";  
?>
<HTML>
<HEAD>
 <TITLE>menu</TITLE>
 <link href="../../platform/atk/style.php" rel="stylesheet" type="text/css">
 <LINK REL="stylesheet" HREF="tabs.css">
 <script language="JavaScript">
  var tabSelectMode = "<? echo $config_tabselectMode ?>";
 </script>
 <script language="JavaScript" src="<? echo $config_atkroot; ?>atk/javascript/atk_tabs.js" type="text/javascript"></script>
 <script language="JavaScript">
   var rows     = new Array;
   var num_rows = 1;
   var top      = 0;
   var left     = 10;
   var width    = "100%";
   var tab_off  = "#198DE9";
   var tab_on   = "#EEEEE0";

   rows[1]      = new Array;
 <? echo $tabs; ?>
 </script>
 <script language="javascript"> generateTabs(); </script>
</HEAD>
<BODY>
 <? echo $divs ?>
 <script language="JavaScript">
  if (DOM) { currShow=document.getElementById('T11');}
  else if (IE) { currShow=document.all['T11'];}
  else if (NS4) { currShow=document.layers["T11"];};
  changeCont("11", "tab11");
 </script>
</BODY>
</HTML>
