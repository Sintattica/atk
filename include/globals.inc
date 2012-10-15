<?php

  /**
   * This file is part of the Achievo ATK distribution.
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
  $g_startTime = (float)$usec + (float)$sec; // can't use getmicrotime here because it isn't available yet

  /**
   * Global theme variable, containing theme data
   * @deprecated In favor of atkTheme class
   * @var array
   */
  $g_theme = array();

  /**
   * Global node list
   */
  $g_nodes = array();

  /**
   * Global module list
   */
  $g_modules = array();

  /**
   * Global menu
   */
  $g_menu = array();

  /**
   * Global moduleflags per module
   */
  $g_moduleflags = array();

  /**
   * Sticky global variables.
   * When you add 'key' to this, ATK will always pass 'key=$GLOBALS['key']'
   * in session urls and forms.
   * @var array
   */
  $g_stickyurl = array();

  /**
   * Modifiers
   */
  $g_modifiers = array();

  /**
   * Overloaders
   */
  $g_overloaders = array();

?>