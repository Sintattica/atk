<?php

  /**
   * Captcha wrapper for displaying a captcha image and storing the displayed code
   * in the user session
   */
  
  // captcha directory
  $captchaDir = atkconfig("atkroot")."atk/ext/captcha/";

  // include captcha class 
  require($captchaDir."php-captcha.inc.php");
  //require("php-captcha.inc.php");

  // define fonts 
  $aFonts = array($captchaDir . 'captcha_fonts/VeraBd.ttf', $captchaDir. 'captcha_fonts/VeraIt.ttf', $captchaDir . 'captcha_fonts/Vera.ttf'); 

  // create new image 
  $oPhpCaptcha = new PhpCaptcha($aFonts, 200, 50); 
  $oPhpCaptcha->SetBackgroundImage($captchaDir . 'img/captcha.jpg'); 
  
  $oPhpCaptcha->Create();
?>
