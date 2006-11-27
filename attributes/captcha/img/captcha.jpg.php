<?php

  /**
   * Captcha wrapper for displaying a captcha image and storing the displayed code
   * in the user session
   */
  
  $config_atkroot = "./";
  
  chdir("../../../../");

  $current = getcwd();

  // include atk
  require("atk.inc");
  // include captcha class 
  require("atk/attributes/captcha/php-captcha.inc.php");
  
  // create new image 
  $oPhpCaptcha = new PhpCaptcha(atkConfig::get("captcha", "captcha_fonts"), atkConfig::get("captcha", "captcha_width"), atkConfig::get("captcha", "captcha_height")); 
  $oPhpCaptcha->SetBackgroundImage(atkConfig::get("captcha", "captcha_dir") . 'img/captcha.jpg'); 
  
  $oPhpCaptcha->Create();
?>