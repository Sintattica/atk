<?php

  /**
   * This file is part of the Achievo ATK distribution.
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
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal Setup the system
   */
  $config_atkroot = "./";

  include_once($config_atkroot."atk/include/initial.inc");

  atksession();
  atksecure();

  $debugger = &atkinstance("atk.utils.atkdebugger");

  $output = &atkOutput::getInstance();

  $output->output($debugger->renderConsole());

  $config_debug = 0; // force debugging off in console
  $output->outputFlush();
?>
