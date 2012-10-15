<? 
  atkPage::getInstance()->register_script(atkconfig('atkroot').'atk/javascript/overlibmws/overlibmws.js');
  $theme = atkinstance("atk.ui.atktheme");
  $image = $theme->imgPath("help");
  $tooltip = atk_htmlentities(str_replace(array("\r\n", "\r", "\n"), ' ', $tooltip));  
?>

<img align="top" src="<?= $image ?>" border="0" style="margin-left: 3px;" onmouseover="return overlib(&quot;<?= $tooltip ?>&quot;, BGCLASS, 'overlib_bg', FGCLASS, 'overlib_fg', TEXTFONTCLASS, 'overlib_txt', WIDTH, 300);" onmouseout="return nd();" />