<?php

  // captcha config values
  $config['captcha_dir']              = atkconfig("atkroot") . "atk/attributes/captcha/";
  
  $config['captcha_img_dir']          = atkconfig("atkroot") . "uploaded_files/";

  $config['captcha_fonts_dir']        = atkconfig("atkroot") . "atk/attributes/captcha/captcha_fonts/";

  // array of TypeType fonts to use - specify full path
  $config['captcha_fonts']            = array($config['captcha_fonts_dir'] . 'VeraBd.ttf', 
                                              $config['captcha_fonts_dir']. 'VeraIt.ttf', 
                                              $config['captcha_fonts_dir'] . 'Vera.ttf');
  
  // width of image
  $config['captcha_width']            = 200;
  // height of image
  $config['captcha_height']           = 50;
  // number of characters to draw
  $config['captcha_num_chars']        = 5;
  // number of noise lines to draw
  $config['captcha_num_lines']        = 70;
  // add shadow to generated characters to further obscure code
  $config['captcha_display_shadow']   = false;
  // array of characters to select from - if blank uses upper case A - Z
  $config['captcha_char_set']         = array();
  // add owner text to bottom of CAPTCHA, usually your site address
  $config['captcha_owner_text']       = '';
  // background image to use - if blank creates image with white background
  $config['captcha_background_image'] = '';
  // set the minimum font size that can be selected
  $config['captcha_min_font_size']    = 16;
  // set the maximum font size that can be used
  $config['captcha_max_font_size']    = 25;
  // determines whether or not to use colour to draw lines and characters
  $config['captcha_use_colour']       = false;
  // set the output file type
  $config['captcha_file_type']        = 'jpeg';

?>