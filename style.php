<?php
  chdir("../");  
  include_once("atk/class.atknode.inc");
?>

BODY
{
  font-family: <? echo $g_theme["FontFamily"]; ?>;
  font-size: <? echo $g_theme["FontSize"]; ?>pt;
  font-weight: <? echo $g_theme["FontWeight"]; ?>;
  background-color: <? echo $g_theme["BgColor"]; ?>;
  color: <? echo $g_theme["FgColor"]; ?>;
 <?
 if($g_theme["BgUrl"]!="")
 {
   echo "background: url(".$g_theme["BgUrl"].");\n";
 }
 ?>
 
}

A:link
{
  color: <? echo $g_theme["LinkColor"]; ?>;
}

A:visited
{
  color: <? echo $g_theme["VisitedColor"]; ?>;
}

A:active
{
  color: <? echo $g_theme["VisitedColor"]; ?>;
}

A:hover
{
  color: <? echo $g_theme["HoverColor"]; ?>;
}

.block
{
  font-family: <? echo $g_theme["BlockFontFamily"]; ?>;
  font-size: <? echo $g_theme["BlockFontSize"]; ?>pt;
  font-weight: <? echo $g_theme["BlockFontWeight"]; ?>;
  color: <? echo $g_theme["BlockFgColor"]; ?>;
}

.tableheader
{
  font-family: <? echo $g_theme["TableHeaderFontFamily"]; ?>;
  font-size: <? echo $g_theme["TableHeaderFontSize"]; ?>pt;
  font-weight: <? echo $g_theme["TableHeaderFontWeight"]; ?>;
  background-color: <? echo $g_theme["TableHeaderBgColor"]; ?>;
  color: <? echo $g_theme["TableHeaderFgColor"]; ?>;
}

.table
{
  font-family: <? echo $g_theme["TableFontFamily"]; ?>;
  font-size: <? echo $g_theme["TableFontSize"]; ?>pt;
  font-weight: <? echo $g_theme["TableFontWeight"]; ?>;
  color: <? echo $g_theme["TableFgColor"]; ?>;
  background-color: <? echo $g_theme["TableBgColor"]; ?>;
}
