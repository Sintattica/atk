<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be 
 * included in the distribution.
 *
 * This file is the skeleton main include wrapper, which you can copy
 * to your application dir and modify if necessary. It is used to 
 * include popups in a safe manner. Any popup loaded with this wrapper
 * has session support and login support. 
 * Only files defined in the $config_allowed_includes array are allowed
 * to be included.
 *
 * @package atk
 * @subpackage skel
 *
 * @author Ivo Jansch <ivo@achievo.org>
 *
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 2629 $
 * $Id$
 */
/**
 * @internal includes
 */
$config_atkroot = "./";
include_once("atk.php");

Atk_SessionManager::atksession();
atksecure();

$file = $ATK_VARS["file"];
$allowed = Atk_Config::getGlobal("allowed_includes");
if (Atk_Tools::atk_in_array($file, $allowed))
    include_once(Atk_Config::getGlobal("atkroot") . $file);