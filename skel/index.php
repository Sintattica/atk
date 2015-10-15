<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * This file is the skeleton index file, which you can copy to your
 * application dir and modify if necessary. By default, it checks
 * the setting of $config_fullscreen, and if set, launches the
 * app in a full screen window. If not set, the frameset is loaded.
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
 * @internal includes
 */
$config_atkroot = "./";
include_once("atk.php");
Atk_SessionManager::atksession();
atksecure();

$theme = Atk_Tools::atkinstance('atk.ui.atktheme');
if (Atk_Config::getGlobal("fullscreen")) {
    // Fullscreen mode. Use index.php as launcher, and launch app.php fullscreen.

    Atk_SessionManager::atksession();
    atksecure();

    $page = Atk_Tools::atknew("atk.ui.atkpage");
    $ui = Atk_Tools::atkinstance("atk.ui.atkui");
    $theme = Atk_Theme::getInstance();
    $output = Atk_Output::getInstance();

    $page->register_style($theme->stylePath("style.css"));
    $page->register_script(Atk_Config::getGlobal("atkroot") . "atk/javascript/launcher.js");

    $content = '<script language="javascript">atkLaunchApp(); </script>';
    $content .= '<br><br><a href="#" onClick="atkLaunchApp()">' . Atk_Tools::atktext('app_reopen') . '</a> &nbsp; ' .
        '<a href="#" onClick="window.close()">' . Atk_Tools::atktext('app_close') . '</a><br><br>';

    $box = $ui->renderBox(array(
        "title" => Atk_Tools::atktext("app_launcher"),
        "content" => $content
    ));

    $page->addContent($box);
    $output->output($page->render(Atk_Tools::atktext('app_launcher'), true));

    $output->outputFlush();
} else {
    if ($theme->getAttribute('useframes', true)) {
        // Regular mode. app.php can be included directly.
        include "app.php";
    } else {
        $indexpage = Atk_Tools::atknew('atk.ui.atkindexpage');
        $indexpage->generate();
    }
}