<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage include
 *
 * @copyright (c)2005 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 6301 $
 * $Id$
 */
/** @internal includes */
require_once($GLOBALS['config_atkroot'] . 'atk/include/basics.php');
require_once($GLOBALS['config_atkroot'] . 'atk/include/compatibility.php');
if (atkConfig::getGlobal('autoload_classes', true)) {
    require_once($GLOBALS['config_atkroot'] . 'atk/include/autoload.php');
}
if (atkConfig::getGlobal('use_atkerrorhandler', true)) {
    require_once($GLOBALS['config_atkroot'] . 'atk/include/errorhandler.php');
}
require_once($GLOBALS['config_atkroot'] . 'atk/ui/class.atkoutput.php');
require_once($GLOBALS['config_atkroot'] . 'atk/session/class.atksessionmanager.php');
require_once($GLOBALS['config_atkroot'] . "atk/utils/class.atkstring.php");
require_once($GLOBALS['config_atkroot'] . 'atk/include/security.php');
require_once($GLOBALS['config_atkroot'] . 'atk/include/debugging.php');
if (atkConfig::getGlobal('lock_type') !== "") {
    require_once($GLOBALS['config_atkroot'] . 'atk/lock/class.atklock.php');
}
atkModule::atkPreloadModules();

