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
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: $
 * $Id$
 */
/* INITIALIZE ATK GLOBALS */

list($usec, $sec) = explode(" ", microtime());

/**
 * Current microtime, to see when exactly this request started.
 * @var float
 */
$GLOBALS['g_startTime'] = (float) $usec + (float) $sec; // can't use getmicrotime here because it isn't available yet

/**
 * Global theme variable, containing theme data
 * @deprecated In favor of atkTheme class
 * @var array
 */
$GLOBALS['g_theme'] = array();

/**
 * Global node list
 */
$GLOBALS['g_nodes'] = array();

/**
 * Global module list
 */
$GLOBALS['g_modules'] = array();

/**
 * Global menu
 */
$GLOBALS['g_menu'] = array();

/**
 * Global moduleflags per module
 */
$GLOBALS['g_moduleflags'] = array();

/**
 * Sticky global variables.
 * When you add 'key' to this, ATK will always pass 'key=$GLOBALS['key']'
 * in session urls and forms.
 * @var array
 */
$GLOBALS['g_stickyurl'] = array();

/**
 * Modifiers
 */
$GLOBALS['g_modifiers'] = array();

/**
 * Overloaders
 */
$GLOBALS['g_overloaders'] = array();
?>