<!--
 * FCKeditor - The text editor for internet
 * Copyright (C) 2003-2005 Frederico Caldeira Knabben
 * 
 * Licensed under the terms of the GNU Lesser General Public License:
 * 		http://www.opensource.org/licenses/lgpl-license.php
 * 
 * For further information visit:
 * 		http://www.fckeditor.net/
 * 
 * File Name: browser.html
 * 	This page compose the File Browser dialog frameset.
 * 
 * File Authors:
 * 		Frederico Caldeira Knabben (fredck@fckeditor.net)
-->
<?php

  // Include /config.inc.php to retrieve
  // language and theme
  global $config_platformroot, $config_atkroot, $config_language;

  $config_platformroot = "../../../../../../../";
  $config_atkroot      = $config_platformroot;

  include_once($config_atkroot."atk.inc");
  atksession("admin");

  $editingurl  = sessionLoad("editingurl", "admin");
  $theme       = &atkinstance('atk.ui.atktheme');
  $cssFile    = $theme->stylePath('fck_mcpuk_browser.css');
  
  // add trailingslash to editingurl when needed
  if (strlen($editingurl) > 1 && substr($editingurl,-1) != "/")
    $editingurl .= "/";
  
  $frameParams = "?lng=".$config_language."&themecss=".urlencode($cssFile)."&editingurl=".urlencode($editingurl);
  
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
	<head>
		<title>FCKeditor - Resources Browser</title>
		<link href="<? echo $themeDir.$cssFile; ?>" type="text/css" rel="stylesheet">
	</head>
	<frameset cols="150,*" framespacing="0" bordercolor="#f1f1e3" frameborder="no" class="Frame_none">
		<frameset rows="50,*" framespacing="0"  class="Frame_r">
			<frame src="frmresourcetype.php<? echo $frameParams; ?>" scrolling="no" frameborder="no">
			<frame name="frmFolders" id="frmFolders" src="frmfolders.php<? echo $frameParams; ?>" scrolling="auto" frameborder="no">
		</frameset>
		<frameset rows="50,*,50" framespacing="0" class="Frame_none">
			<frame name="frmActualFolder" src="frmactualfolder.php<? echo $frameParams; ?>" scrolling="no" frameborder="no">
			<frame name="frmResourcesList" id="mainWindow" src="frmresourceslist.php<? echo $frameParams; ?>" scrolling="auto" frameborder="no">
			<frameset cols="150,*,0" framespacing="0" frameborder="no" class="Frame_t">
				<frame name="frmCreateFolder" id="frmCreateFolder" src="frmcreatefolder.php<? echo $frameParams; ?>" scrolling="no" frameborder="no">
				<frame name="frmUpload" id="frmUpload" src="frmupload.php<? echo $frameParams; ?>" scrolling="no" frameborder="no">
				<frame name="frmUploadWorker" src="" scrolling="no" frameborder="no">
			</frameset>
		</frameset>
	</frameset>
</html>
