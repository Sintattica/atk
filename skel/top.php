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
   * @version $Revision: 5036 $
   * $Id$
   */

  /**
   * @internal includes.
   */
  $config_atkroot = "./";
  include_once("atk.inc");

  atksession();
  atksecure();

  $page = &atkNew("atk.ui.atkpage");
  $ui = &atkInstance("atk.ui.atkui");
  $theme = &atkInstance("atk.ui.atktheme");
  $output = &atkInstance("atk.ui.atkoutput");

  $page->register_style($theme->stylePath("style.css"));
  $page->register_style($theme->stylePath("top.css"));

  $vars = array("logintext" => atkText("logged_in_as", "atk"),
                "logouttext" => ucfirst(atkText("logout", "atk")),
                "logoutlink" => "app.php?atklogout=1",
                "logouttarget" => "_top",
                "centerpiece" => "",
                "searchpiece" => "",
                "title" => atkText("app_title"),
                "user" => atkArrayNvl(atkGetUser(), "name"));

  // Backwards compatible $vars[content], that is what will render when the
  // box.tpl is used instead of a top.tpl. This happens in old themes.
  $contenttpl = '<br />[logintext]: <b>[user]</b> &nbsp; <a href="[logoutlink]" target="[logouttarget]">[logouttext] </a>&nbsp;<br /><br />';
  $stringparser = &atkNew("atk.utils.atkstringparser", $contenttpl);
  $vars["content"] = $stringparser->parse($vars);

  $top = $ui->renderBox($vars, "top");

  $page->addContent($top);

  $output->output($page->render($vars["title"], true));

  $output->outputFlush();

?>