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
 * @version $Revision: 5798 $
 * $Id$
 */
 
/**
 * Implements the {atkloadscript} plugin for use in templates.
 *
 * The atkloadscript plugin registers a javascript code in the current page
 * to be used in the onLoad of the body.
 * 
 * Params:
 * 0/code The javascript code to load.
 *
 * Example:
 * {atkscript "alert('Hello World!');"}
 *
 * @author Boy Baukema <boy@achievo.org>
 */
  function smarty_function_atkloadscript($params)
  {
    $page = &atkinstance('atk.ui.atkpage');
    $page->register_loadscript($params[0]?$params[0]:$params['code']);
  }

?>