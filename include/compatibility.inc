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
/**
 * WARNING: Do NOT use this global variable, it is deprecated .
 * Re-indroduced to ensure backwards compatibility.
 * @deprecated in favor of the atkSelf() function.
 */
$GLOBALS['PHP_SELF'] = atkSelf();

/**
 * WARNING: Do NOT use this global variable, it is deprecated .
 * Set this because all PHP_SELF variables must have the same value.
 * @deprecated in favor of the atkSelf() function.
 */
$_SERVER["PHP_SELF"] = atkSelf();
?>