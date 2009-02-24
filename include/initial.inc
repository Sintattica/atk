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
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /** @internal includes */
  require_once($config_atkroot.'atk/include/basics.inc');

  require_once($config_atkroot.'atk/include/compatibility.inc');
  if (atkconfig('autoload_classes',    true)) require_once($config_atkroot.'atk/include/autoload.inc');
  if (atkconfig('use_atkerrorhandler', true)) require_once($config_atkroot.'atk/include/errorhandler.inc');
  require_once($config_atkroot.'atk/ui/class.atkoutput.inc');
  require_once($config_atkroot.'atk/session/class.atksessionmanager.inc');
  require_once($config_atkroot."atk/utils/class.atkstring.inc");
  require_once($config_atkroot.'atk/include/security.inc');
  require_once($config_atkroot.'atk/modules/atkmoduletools.inc');
  require_once($config_atkroot.'atk/include/debugging.inc');
  if (atkconfig('lock_type')!=="") require_once($config_atkroot.'atk/lock/class.atklock.inc');
  atkPreloadModules();

?>
