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
  $node = $_GET["node"];
  $title = atktext("title_$node", isset($_GET["module"])?$_GET["module"]:"");
  $helpbase = $config_atkroot;
  if (isset($_GET["module"])) $helpbase = moduleDir($_GET["module"]);
  $file   = $helpbase."help/".atkconfig('language')."/help.".$node.".inc";
  $data = '<div align="left">';
  $data .= implode("<br>",file($file));
  $data .='</div>';

  $page = &atknew("atk.ui.atkpage");
  $ui = &atkinstance("atk.ui.atkui");

  $output = &atkOutput::getInstance();

  $page->register_style($ui->stylePath("style.css"));

  $res = $ui->renderBox(array("title"=>$title,
                                 "content"=>$data));
  $res.='<br><div align="right"><a href="javascript:window.close();">'.atktext("close").'</a></div>';

  $page->addContent($res);

  $output->output($page->render(atktext('app_title').' - '.atktext('help'), true));

  $output->outputFlush();

?>