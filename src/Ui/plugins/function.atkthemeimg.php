<?php

use Sintattica\Atk\Ui\Theme;

/**
 * Implements the {atkthemeimg} plugin for use in templates.
 *
 * The atkthemeimg plugin retrieves an image path from the theme, while
 * respecting inheritance within the theme. In other words, if
 * a theme derives from the default theme, but does not define a
 * new version of 'someimg.jpg', then {atkthemeimg someimg.jpg} will
 * display the path to the default theme version of the same.
 *
 * Params: The name of the image to retrieve
 *
 * Example:
 * <img src="{atkthemeimg test.gif}">
 *
 * @author Ivo Jansch <ivo@achievo.org>
 *
 */
function smarty_function_atkthemeimg($params, &$smarty)
{
    $theme = Theme::getInstance();
    return $theme->imgPath($params[0]);
}