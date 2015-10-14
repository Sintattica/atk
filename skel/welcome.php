<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * This file is the skeleton main welcome file, which you can copy
 * to your application dir and modify if necessary. By default, it
 * displays a welcome message from the language file (app_description).
 *
 * @package atk
 * @subpackage skel
 *
 * @author Ivo Jansch <ivo@achievo.org>
 *
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 4845 $
 * $Id$
 */
/**
 * @internal includes..
 */
$config_atkroot = "./";
include_once("atk.php");

$page = &Atk_Tools::atknew("atk.ui.atkpage");
$ui = &Atk_Tools::atkinstance("atk.ui.atkui");
$theme = &Atk_Theme::getInstance();
$output = &Atk_Output::getInstance();

$page->register_style($theme->stylePath("style.css"));
$box = $ui->renderBox(array("title" => Atk_Tools::atktext("app_shorttitle"),
    "content" => "<br><br>" . Atk_Tools::atktext("app_description") . "<br><br>"));

$page->addContent($box);
$output->output($page->render(Atk_Tools::atktext('app_shorttitle'), true));

$output->outputFlush();