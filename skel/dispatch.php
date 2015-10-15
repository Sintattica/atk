<?php
/**
 * This file is part of the ATK distribution on GitHub.
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
 * @version $Revision: 6083 $
 * $Id$
 */
/**
 * @internal Setup the system
 */
$config_atkroot = "./";
include_once("atk.php");

Atk_SessionManager::atksession();

$session = Atk_SessionManager::getSession();
$output = Atk_Output::getInstance();

if ($ATK_VARS["atknodetype"] == "" || $session["login"] != 1) {
    // no nodetype passed, or session expired

    $page = Atk_Tools::atknew("atk.ui.atkpage");
    $ui = Atk_Tools::atkinstance("atk.ui.atkui");
    $theme = Atk_Theme::getInstance();


    $page->register_style($theme->stylePath("style.css"));

    $destination = "index.php?atklogout=true";
    if (isset($ATK_VARS["atknodetype"]) && isset($ATK_VARS["atkaction"])) {
        $destination .= "&atknodetype=" . $ATK_VARS["atknodetype"] . "&atkaction=" . $ATK_VARS["atkaction"];
        if (isset($ATK_VARS["atkselector"])) {
            $destination .= "&atkselector=" . $ATK_VARS["atkselector"];
        }
    }

    $title = Atk_Tools::atktext("title_session_expired");
    $contenttpl = '<br>%s<br><br><input type="button" onclick="top.location=\'%s\';" value="%s"><br><br>';
    $content = sprintf($contenttpl, Atk_Tools::atktext("explain_session_expired"),
        str_replace("'", "\\'", $destination), Atk_Tools::atktext("relogin"));
    $box = $ui->renderBox(array("title" => $title, "content" => $content));

    $page->addContent($box);
    $output->output($page->render(Atk_Tools::atktext("title_session_expired"), true));
} else {
    atksecure();

    $lockType = Atk_Config::getGlobal("lock_type");
    if (!empty($lockType)) {
        atklock();
    }

    $flags = array_key_exists("atkpartial", $ATK_VARS) ? HTML_PARTIAL : HTML_STRICT;

    //Load controller   
    if ($ATK_VARS["atkcontroller"] == "") {
        $controller = Atk_Tools::atkinstance("atk.atkcontroller");
    } else {
        $controller = Atk_Tools::atkinstance($ATK_VARS["atkcontroller"]);
    }

    //Handle http request  
    $controller->dispatch($ATK_VARS, $flags);
}
$output->outputFlush();
