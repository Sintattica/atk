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

  atksession();
  atksecure();

  $suite = &atknew("atk.test.atktestsuite");
  $suite->run((PHP_SAPI != "cli") ? "html" : "text", atkArrayNvl($_REQUEST, "atkmodule"));

?>
