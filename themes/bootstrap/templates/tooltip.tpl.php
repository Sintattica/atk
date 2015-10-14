<?php
Atk_Page::getInstance()->register_script(Atk_Config::getGlobal('atkroot') . 'atk/javascript/overlibmws/overlibmws.js');
$theme = Atk_Tools::atkinstance("atk.ui.atktheme");
$image = $theme->imgPath("help");
$tooltip = htmlentities(str_replace(array("\r\n", "\r", "\n"), ' ', $tooltip));
?>

<img align="top" src="<?php echo $image ?>" border="0" style="margin-left: 3px;"
     onmouseover="return overlib( & quot;<?php echo $tooltip ?> & quot; , BGCLASS, 'overlib_bg', FGCLASS, 'overlib_fg', TEXTFONTCLASS, 'overlib_txt', WIDTH, 300);"
     onmouseout="return nd();"/>