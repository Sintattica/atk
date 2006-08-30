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
   * @version $Revision$
   * $Id$
   */

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
    $theme = &atkinstance("atk.ui.atktheme");
    return $theme->imgPath($params[0]);
  }

?>
