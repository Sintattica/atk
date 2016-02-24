<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage themes
 *
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 6021 $
 * $Id$
 */
/**
 * Theme definition
 */
$theme["tabtype"] = "dhtml";
$theme["recordlist_orientation"] = 'right';
$theme['compatible_menus'] = array('modern', 'smart');
$theme['modern_menu_compat'] = true;
$theme['frame_top_height'] = 149;
$theme['frame_menu_width'] = 210;
$theme['dialog_theme_name'] = 'atkdialog';
$theme['dialog_theme_load'] = false;

/* recordlist font-awesome icons (activable with $theme['usecssicons'] = true; and overridable) */
$theme['cssicons']['recordlist']['view'] = 'fa fa-search';
$theme['cssicons']['recordlist']['edit'] = 'fa fa-pencil';
$theme['cssicons']['recordlist']['delete'] = 'fa fa-trash-o';
$theme['cssicons']['recordlist']['copy'] = 'fa fa-files-o';
$theme['cssicons']['recordlist']['editcopy'] = 'fa fa-files-o';
$theme['cssicons']['recordlist']['select'] = 'fa fa-hand-o-up';
$theme['cssicons']['lock']['lock_exclusive_head'] = 'fa fa-lock';
$theme['cssicons']['lock']['lock_exclusive'] = 'fa fa-lock';