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

$theme["recordlist_orientation"] = 'right';
$theme['modern_menu_compat'] = true;
$theme['frame_top_height'] = 149;
$theme['frame_menu_width'] = 210;
$theme['dialog_theme_name'] = 'atkdialog';
$theme['dialog_theme_load'] = false;

$theme["doctype"] = '<!DOCTYPE html>';
$theme['compatible_menus'] = array('atk.themes.bootstrap.src.atkBootstrapMenu');
$theme["highlight"] = "";
$theme["select"] = "";
$theme['useframes'] = false;
$theme['dispatcher'] = 'index.php';
$theme['menufile'] = 'index.php';
$theme['menu_params'] = "class='menulink'";
$theme["tabtype"] = "dhtml";


$theme['usecssicons'] = true;
$theme['cssicons']['recordlist']['view'] = 'glyphicon glyphicon-eye-open';
$theme['cssicons']['recordlist']['edit'] = 'glyphicon glyphicon-pencil';
$theme['cssicons']['recordlist']['delete'] = 'glyphicon glyphicon-trash';
$theme['cssicons']['lock']['lock_exclusive_head'] = 'glyphicon glyphicon-lock';
$theme['cssicons']['lock']['lock_exclusive'] = 'glyphicon glyphicon-lock';