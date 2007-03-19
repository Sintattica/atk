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

  /**
   * Global theme variable, must be declared before the includes
   */
  $g_theme = array();

  // Global node list
  $g_nodes = array();
  // Global module list
  $g_modules = array();
  // Global menu
  $g_menu = array();
  // Global moduleflags per module
  $g_moduleflags = array();

  // modifiers
  $g_modifiers = array();
  $g_overloaders = array();

  // Another workaround. We use PHP_SELF a lot. Without register_globals=On, we
  // need to make it global ourselves. Todo: replace EVERY reference to
  // PHP_SELF with _SERVER["PHP_SELF"] eventually.
  $PHP_SELF = $_SERVER["PHP_SELF"];

  require_once($config_atkroot."atk/class.atkconfig.inc");
  require_once($config_atkroot."atk/defaultconfig.inc.php");

  // Auto-register modules
  if (file_exists("$config_module_path/config.modules.inc"))
    include_once("$config_module_path/config.modules.inc");

  include_once($config_atkroot."config.inc.php");
  require_once($config_atkroot."atk/atktools.inc");
  require_once($config_atkroot.'atk/include/autoload.inc');

  // Once atktools is loaded, we can register our own error handler.
  if (atkconfig("use_atkerrorhandler", true))
  {
    set_error_handler("atkErrorHandler");
    error_reporting(E_ALL);
  }

  require_once($config_atkroot."atk/ui/class.atkoutput.inc");
  require_once($config_atkroot."atk/session/class.atksessionmanager.inc");
  require_once($config_atkroot."atk/include/security.inc");
  require_once($config_atkroot."atk/modules/atkmoduletools.inc");

  if (is_array($config_smart_debug))
    $config_debug = atkConfig::smartDebugLevel($config_debug, $config_smart_debug);

  // Include locking functionality?
  $lockType = atkconfig("lock_type");
  if (!empty($lockType))
    require_once($config_atkroot."atk/lock/class.atklock.inc");

  $g_stickyurl = array();

  // preload modules
  atkPreloadModules();

?>