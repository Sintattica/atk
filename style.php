<?php
  chdir("../");  
  
  include_once("atk.inc");
?>

BODY
{
  font-family: <?php echo $g_theme["FontFamily"]; ?>;
  font-size: <?php echo $g_theme["FontSize"]; ?>pt;
  font-weight: <?php echo $g_theme["FontWeight"]; ?>;
  background-color: <?php echo $g_theme["BgColor"]; ?>;
  color: <?php echo $g_theme["FgColor"]; ?>;
 <?php
 if($g_theme["BgUrl"]!="")
 {
   echo "background: url(".$g_theme["BgUrl"].");\n";
 }
 ?>
 
}

A:link
{
  color: <?php echo $g_theme["LinkColor"]; ?>;
}

A:visited
{
  color: <?php echo $g_theme["VisitedColor"]; ?>;
}

A:active
{
  color: <?php echo $g_theme["VisitedColor"]; ?>;
}

A:hover
{
  color: <?php echo $g_theme["HoverColor"]; ?>;
}

.block
{
  font-family: <?php echo $g_theme["BlockFontFamily"]; ?>;
  font-size: <?php echo $g_theme["BlockFontSize"]; ?>pt;
  font-weight: <?php echo $g_theme["BlockFontWeight"]; ?>;
  color: <?php echo $g_theme["BlockFgColor"]; ?>;
}

.tableheader
{
  font-family: <?php echo $g_theme["TableHeaderFontFamily"]; ?>;
  font-size: <?php echo $g_theme["TableHeaderFontSize"]; ?>pt;
  font-weight: <?php echo $g_theme["TableHeaderFontWeight"]; ?>;
  background-color: <?php echo $g_theme["TableHeaderBgColor"]; ?>;
  color: <?php echo $g_theme["TableHeaderFgColor"]; ?>;
}

.error
{
  font-weight: bold;
  color: red
}

.table
{
  font-family: <?php echo $g_theme["TableFontFamily"]; ?>;
  font-size: <?php echo $g_theme["TableFontSize"]; ?>pt;
  font-weight: <?php echo $g_theme["TableFontWeight"]; ?>;
  color: <?php echo $g_theme["TableFgColor"]; ?>;
  background-color: <?php echo $g_theme["TableBgColor"]; ?>;
}

.backtable
{
  background-color: <?php echo $g_theme["BorderColor"]; ?>;
}

.row1
{
<?php
 if (isset($g_theme["RowColor1"]))
{ ?>
  background-color: <?php echo $g_theme["RowColor1"]; ?>;
  <?php }?>
}
.row2
{
<?php
 if (isset($g_theme["RowColor2"]))
{?>
  background-color: <?php echo $g_theme["RowColor2"]; ?>;
<?php } ?>
}
