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

  list($usec, $sec) = explode(" ", microtime());

  /**
   * Current microtime, to see when exactly this request started.
   * @var float
   */
  $g_startTime = (float)$usec + (float)$sec; // can't use getmicrotime here because it isn't available yet

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
    
  require_once($config_atkroot."atk/class.atkconfig.inc");
  require_once($config_atkroot."atk/defaultconfig.inc.php");

  // Auto-register modules
  if (file_exists("$config_module_path/config.modules.inc"))
    include_once("$config_module_path/config.modules.inc");

  include_once($config_atkroot."config.inc.php");
  require_once($config_atkroot."atk/atktools.inc");

  /**
   * WARNING: Do NOT use this global variable, it is deprecated .
   * Re-indroduced to ensure backwards compatibility.   
   * @deprecated in favor of the atkSelf() function.
   */
  $PHP_SELF = atkSelf();
  
  /**
   * WARNING: Do NOT use this global variable, it is deprecated .
   * Set this because all PHP_SELF variables must have the same value.
   * @deprecated in favor of the atkSelf() function.
   */
  $_SERVER["PHP_SELF"] = $PHP_SELF;
  
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

  if (atk_value_in_array($config_smart_debug))
    $config_debug = atkConfig::smartDebugLevel($config_debug, $config_smart_debug);

  // Include locking functionality?
  $lockType = atkconfig("lock_type");
  if (!empty($lockType))
    require_once($config_atkroot."atk/lock/class.atklock.inc");

  $g_stickyurl = array();

  // preload modules
  atkPreloadModules();

?>