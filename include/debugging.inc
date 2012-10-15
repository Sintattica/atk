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

  if (atk_value_in_array($config_smart_debug))
  {
    $config_debug = atkConfig::smartDebugLevel($config_debug, $config_smart_debug);
  }

  if ($config_debug > 0)
  {
    ini_set('display_errors', 1);
  }

  // show server info in debug (useful in clustered environments)
  atkdebug('Server info: '.$_SERVER['SERVER_NAME'].' ('.$_SERVER['SERVER_ADDR'].')');

?>