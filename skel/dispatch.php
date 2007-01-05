<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * This file is the skeleton dispatcher file, which you can copy
   * to your application dir and modify if necessary. By default, it
   * checks the $atknodetype and $atkaction postvars and creates the
   * node and dispatches the action.
   *
   * @package atk
   * @subpackage skel
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal Setup the system
   */
  $config_atkroot = "./";
  include_once("atk.inc");

  atksession();

  $session = &atkSessionManager::getSession();
  $output = &atkOutput::getInstance();

  if($ATK_VARS["atknodetype"]=="" || $session["login"]!=1)
  {
    // no nodetype passed, or session expired

    $page = &atknew("atk.ui.atkpage");
    $ui = &atkinstance("atk.ui.atkui");
    $theme = &atkTheme::getInstance();
    

    $page->register_style($theme->stylePath("style.css"));

    $destination = "";
    if(isset($ATK_VARS["atknodetype"]) && isset($ATK_VARS["atkaction"]))
    {
      $destination = "&atknodetype=".$ATK_VARS["atknodetype"]."&atkaction=".$ATK_VARS["atkaction"];
      if (isset($ATK_VARS["atkselector"])) $destination.="&atkselector=".$ATK_VARS["atkselector"];
    }

    $box = $ui->renderBox(array("title"=>atktext("title_session_expired"),
                                "content"=>'<br><br>'.atktext("explain_session_expired").'<br><br><br><br>
                                           <a href="index.php?atklogout=true'.$destination.'" target="_top">'.atktext("relogin").'</a><br><br>'));

    $page->addContent($box);
    $output->output($page->render(atktext("title_session_expired"), true));
  }
  else
  {
    atksecure();

    $lockType = atkconfig("lock_type");
    if (!empty($lockType)) atklock();

    // Create node
    $obj = &getNode($ATK_VARS["atknodetype"]);

    $flags = array_key_exists("atkpartial", $ATK_VARS) ? HTML_PARTIAL : HTML_STRICT;

    //Handle http request   
    $controller = &atkinstance("atk.atkcontroller");
    $controller->dispatch($ATK_VARS, $flags);
  }
  $output->outputFlush();
?>
