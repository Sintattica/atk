<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * This file is the skeleton top frame file, which you can copy
   * to your application dir and modify if necessary. By default,
   * it displays the currently logged-in user and a logout link.
   *
   * @package atk
   * @subpackage skel
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal includes.
   */
  $config_atkroot = "./";
  include_once("atk.inc"); 

  atksession();
  atksecure();   
  
  $page = &atknew("atk.ui.atkpage");  
  $ui = &atknew("atk.ui.atkui");  
  $theme = &atkTheme::getInstance();
  $output = &atkOutput::getInstance();
  
  $page->register_style($theme->stylePath("style.css"));
  
  $loggedin = text("loggedin_user", "", "atk").": <b>".$g_user["name"]."</b>";  
  $content = '<br>'.$loggedin.' &nbsp; <a href="app.php?atklogout=1" target="_top">.'.ucfirst(text("logout", "", "atk")).'.</a><br>&nbsp;';
  
  $top = $ui->renderTop(array("title"=>text("topframe"),
                                            "content"=>$content));
 
  $page->addContent($top);

  $output->output($page->render(text('txt_app_title'), true));
  
  $output->outputFlush();
?>
