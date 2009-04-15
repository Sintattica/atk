<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage ui
 *
 * @copyright (c) 2004 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 4271 $
 * $Id$
 */

/**
 * Modifier to encode a Smarty/PHP variable to JSON.
 * 
 * Example of usage:
 * {$var|@atkjson}
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
function smarty_modifier_atkjson($data)
{
  atkimport('atk.utils.atkjson');
  return atkJSON::encode($data);
}
?>
