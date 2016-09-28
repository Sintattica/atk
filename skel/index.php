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
include_once("atk.inc");
atksession();

// check autorefresh call
if(atkConfig::getGlobal('session_autorefresh') && array_key_exists(atkConfig::getGlobal('session_autorefresh_key'), $_GET)) {
    die(session_id());
}


atksecure();

$theme = &atkinstance('atk.ui.atktheme');
if (atkconfig("fullscreen")) {
    // Fullscreen mode. Use index.php as launcher, and launch app.php fullscreen.

    atksession();
    atksecure();

    $page = &atknew("atk.ui.atkpage");
    $ui = &atkinstance("atk.ui.atkui");
    $theme = &atkTheme::getInstance();
    $output = &atkOutput::getInstance();

    $page->register_style($theme->stylePath("style.css"));
    $page->register_script(atkconfig("atkroot") . "atk/javascript/launcher.js");

    $content = '<script language="javascript">atkLaunchApp(); </script>';
    $content.= '<br><br><a href="#" onClick="atkLaunchApp()">' . atktext('app_reopen') . '</a> &nbsp; ' .
        '<a href="#" onClick="window.close()">' . atktext('app_close') . '</a><br><br>';

    $box = $ui->renderBox(array("title" => atktext("app_launcher"),
        "content" => $content));

    $page->addContent($box);
    $output->output($page->render(atktext('app_launcher'), true));

    $output->outputFlush();
} else {
    if ($theme->getAttribute('useframes', true)) {
        // Regular mode. app.php can be included directly.
        include "app.php";
    } else {
        $indexpage = &atknew('atk.ui.atkindexpage');
        $indexpage->generate();
    }
}
?>