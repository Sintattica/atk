<?php

 /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *   
   * Help popup.
   *
   * Opens a new page in the same atk-template style as the
   * atk-application, in a new pop-up screen and shows a help page.
   * input   : $node -> name of the to node for which help is retrieved.
   *
   * This file should only be included from inside the include.php wrapper.
   *
   * @package atk
   * @subpackage utils
   *
   * @author Rene Bakx <rene@ibuildings.nl>
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal include default include file.
   */
  include_once($config_atkroot."atk.inc");  
  
  atksession();
  atksecure();  

//  Renders the help screen  
  $title1 ="txt_title_".$node;
  $title = $$title1;
  $file   = $config_atkroot."help/".atkconfig('language')."/help.".$node.".inc";
  $data = '<div align="left">';
  $data .= implode("<br>",file($file));
  $data .='</div>';
    
  $page = &atknew("atk.ui.atkpage");  
  $ui = &atknew("atk.ui.atkui");  
  $theme = &atkTheme::getInstance();
  $output = &atkOutput::getInstance();
  
  $page->register_style($theme->stylePath("style.css"));
  
  $res = $ui->renderBox(array("title"=>$title,
                                 "content"=>$data));
  $res.='<br><div align="right"><a href="javascript:window.close();">'.text("close").'</a></div>';
  
  $page->addContent($res);

  $output->output($page->render(text('txt_app_title').' - '.text('help'), true));
  
  $output->outputFlush();

?>