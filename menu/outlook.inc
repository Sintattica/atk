<?php

function menu_cmp($a,$b)
{
  if ($a["order"] == $b["order"]) return 0;
  return ($a["order"] < $b["order"]) ? -1 : 1;
}
usort($g_menu[$atkmenutop],"menu_cmp");

function menu_href($url,$name="",$sessionstatus=SESSION_DEFAULT, $extraprops="")
{    
    $str = session_url($url,$sessionstatus);
    // note, we use single quotes instead of doublequotes in the href='', 
    // because we otherwise have a problem with internet explorers buggy
    // javascript parser.. 
   //echo $str;
   return "alert(\"".addslashes($name)."\")";
   

  }
?>
<HTML>

<HEAD>
<BASE TARGET="main">
<META HTTP-EQUIV="Content-Type" CONTENT="text/html;CHARSET=iso-8859-1">
<TITLE>Menu</TITLE>

<!--
  you need this style or you will get an error in ns4 on first page load!
-->
<STYLE>
  div    {
         position:absolute;
         }
</STYLE>
<!-- dit zorgt ervoor dat theme variables ook beschikbaar zijn in javascript en dus outlookmenu -->
<script language="JavaScript" type="text/javascript">

this.ol_themename       = '<?echo $g_theme["Description"];?>';
this.ol_pimage     = '<? echo $config_atkroot."atk/themes/".$g_theme["Name"]."/images/".$g_theme["ol_pimage"];?>';
this.ol_menucolor  = '<? echo $g_theme["ol_menucolor"];?>';
this.ol_wincolor   = '<? echo $g_theme["ol_wincolor"];?>'; 
this.ol_background = '<? echo $g_theme["ol_background"];?>';
this.ol_arrowup    = '<? echo $config_atkroot."atk/themes/".$g_theme["Name"]."/images/arrowup.gif";?>';
this.ol_arrowdown  = '<? echo $config_atkroot."atk/themes/".$g_theme["Name"]."/images/arrowdown.gif";?>';


</script>
<script language="JavaScript" src="<? echo $config_atkroot; ?>atk/javascript/crossbrowser.js" type="text/javascript">
</script>
<script language="JavaScript" src="<? echo $config_atkroot; ?>atk/javascript/outlook.js" type="text/javascript">

</script>

<SCRIPT>


// ---------------------------------------------------------------------------
// Example of howto: use OutlookBar
// ---------------------------------------------------------------------------

  //create OutlookBar
  var o = new createOutlookBar('Bar',0,0,screenSize.width,screenSize.height,ol_background,'white') // OutlookBar
  var p

<?php
 
  while (list ($name) = each ($g_menu))
  {
  $atkmenutop=$name;
	$panelbuttons="";
	
  for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
  {
    $menu="";
    $name = $g_menu[$atkmenutop][$i]["name"];
    $url = session_url($g_menu[$atkmenutop][$i]["url"],SESSION_NEW);
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
    if ($g_menu[$atkmenutop][$i]["name"] == "-") $menu .= "<br>";
    
    /* submenu ? */
    else if (empty($url) && $enable) 
    {
      $menu .= href('menu.php?atkmenutop='.$name,text("menu_$name"),SESSION_DEFAULT).$config_menu_delimiter;
    }
    else if (empty($url) && !$enable)
    {
      //$menu .=text("menu_$name").$config_menu_delimiter;
    }
      
    /* normal menu item */
    else if ($enable) 
		{
		if($g_menu[$atkmenutop][$i]["url"]!="")
		 {
          if(file_exists($config_atkroot.'icons/'.$atkmenutop.'_'.$name.'.gif'))
		   {
           $panelbuttons .= "p.addButton('".$config_atkroot."icons/".$atkmenutop."_".$name.".gif','".addslashes(text("menu_$name"))."','parent.main.location.href = \""."".$url."\"');\n";
           }
		   else
		   {
		   $panelbuttons .= "p.addButton('".$config_atkroot."icons/empty.gif','".addslashes(text("menu_$name"))."','parent.main.location.href = \""."".$url."\"');\n";
		   }
		 }
    }
		else 
    {
      //$menu .= text("menu_$name").$config_menu_delimiter;
	
    }

  }
  //    echo "alert('$panelbuttons')";
  // als panelbuttons is leeg dan zijn er dus geen onderliggende items en hoeft ook het panel niet getoonde te worden
   if ($panelbuttons!="")
     {
      echo "p = new createPanel('".addslashes($atkmenutop)."','".addslashes(text("menu_".$atkmenutop))."');\n";  
  		echo $panelbuttons;
			echo "o.addPanel(p);\n";	
	 }
		
	}
  
  ?>
  
  o.draw();         //draw the OutlookBar
 
//-----------------------------------------------------------------------------
//functions to manage window resize
//-----------------------------------------------------------------------------
//resize OP5 (test screenSize every 100ms)
function resize_op5() {
  if (bt.op5) {
    o.showPanel(o.aktPanel);
    var s = new createPageSize();
    if ((screenSize.width!=s.width) || (screenSize.height!=s.height)) {
      screenSize=new createPageSize();
      //need setTimeout or resize on window-maximize will not work correct!
      setTimeout("o.resize(0,0,screenSize.width,screenSize.height)",100);
    }
    setTimeout("resize_op5()",100);
  }
}

//resize IE & NS (onResize event!)
function myOnResize() {
  if (bt.ie4 || bt.ie5 || bt.ns5) {
    var s=new createPageSize();
    o.resize(0,0,s.width,s.height);
  }
  else
    if (bt.ns4) location.reload();
}

</SCRIPT>

</head>
<!-- need an onResize event to redraw outlookbar after pagesize changes! -->
<!-- OP5 does not support onResize event! use setTimeout every 100ms -->
<body onLoad="resize_op5();" onResize="myOnResize();">
</body>
</html>


