<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage skel
   *
   * @copyright (c)2005 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal includes 
   */
  $config_atkroot = "./";
  include_once("atk.inc");

  // Start session
  atksession();

  // Require ATK authentication if not running in text mode
  if(PHP_SAPI != "cli")
  {
    atksecure();
  }

  // Let the atktestsuite run the requested tests in an appropriate format
  $suite = &atknew("atk.test.atktestsuite");
  $suite->run((PHP_SAPI != "cli") ? "html" : "text", atkArrayNvl($_REQUEST, "atkmodule"));

?>
