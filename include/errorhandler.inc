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

include_once($GLOBALS['config_atkroot']. 'atk/errors/class.atkerrorhandlerbase.inc');

set_error_handler('atkErrorHandler');
set_exception_handler('atkExceptionHandler');
register_shutdown_function('atkFatalHandler');
