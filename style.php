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

.webfx-tree-container {
	margin: 0px;
	padding: 0px;
	font: icon;
	white-space: nowrap;
}

.webfx-tree-item {
	padding: 0px;
	margin: 0px;
	font: icon;
	color: black;
	white-space: nowrap;
}

.webfx-tree-item a, .webfx-tree-item a:active, .webfx-tree-item a:hover {
	margin-left: 3px;
	padding: 1px 2px 1px 2px;
}

.webfx-tree-item a {
	color: black;
	text-decoration: none;
}

.webfx-tree-item a:hover {
	color: blue;
	text-decoration: underline;
}

.webfx-tree-item a:active {
	background: highlight;
	color: highlighttext;
	text-decoration: none;
}

.webfx-tree-item img {
	vertical-align: middle;
	border: 0px;
}

.webfx-tree-icon {
	width: 16px;
	height: 16px;
}

.skin0{
 position:absolute;
 width:150px;
 border:1px solid black;
 background-color:F0F0F0;
 font-family:Verdana;
 line-height:20px;
 cursor:default;
 font-size:12px;
 z-index:100;
 visibility:hidden;
}

.menuitems{
padding-left:10px;
padding-right:10px;
}
