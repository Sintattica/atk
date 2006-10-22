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
 * @copyright (c)2004 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */
 
/**
 * Implements the {atkscript} plugin for use in templates.
 *
 * The atkscript plugin registers a javascript file in the current page.
 * Useful for templates that have an associated javascript that should
 * be loaded each time the template is included.
 *
 * Params:
 * file   The path of the javascript, relative to the running scripts
 *        directory.
 * prefix The prefix for the path of the javascript file, 
 *        for example $atkroot
 *
 * Example:
 * {atkscript file="javascript/default.js"}
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
function smarty_function_atkscript($params, &$smarty)
{
  $page = &atkinstance('atk.ui.atkpage');
  $page->register_script($params["prefix"].$params["file"]);        
  return "";
}
?>