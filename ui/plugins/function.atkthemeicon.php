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
 * @copyright (c)2006 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 4362 $
 * $Id: function.atkthemeimg.php 4362 2006-11-30 16:06:25Z lineke $
 */

/**
 * Implements the {atkthemeimg} plugin for use in templates.
 *
 * The atkthemeicon plugin retrieves an icon path from the theme, while
 * respecting inheritance within the theme. In other words, if
 * a theme derives from the default theme, but does not define a
 * new version of 'someimg.jpg', then {atkthemeimg someimg.jpg} will
 * display the path to the default theme version of the same.
 *
 * Params: 
 * - name: the name of the icon to retrieve
 * - type: the icon type (recordlist etc.)
 *
 * Example:
 * <img src="{atkthemeicon name='delete' type='recordlist'}">
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 *
 */
function smarty_function_atkthemeicon($params, &$smarty)
{
  $theme = &atkinstance("atk.ui.atktheme");
  return $theme->iconPath($params['name'], $params['type']);
}