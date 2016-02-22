<?php

use Sintattica\Atk\Ui\Theme;

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
    $theme = Theme::getInstance();
    return $theme->iconPath($params['name'], $params['type']);
}
