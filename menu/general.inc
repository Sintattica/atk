<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage menu
   *
   * General include file that loads the menu for all kinds of menu
   * implementations.
   * @todo This should be moved to the menu classes.
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal Includes etc.
   */
  include_once(atkconfig("atkroot")."atk/atkmenutools.inc");
  
  $atkmenutop = @$_REQUEST["atkmenutop"];

  /* first add module menuitems */
	global $g_modules;
	
  foreach ($g_modules as $modname => $modpath)
  {
    if (!isset($g_moduleflags[$modname])||!hasFlag($g_moduleflags[$modname], MF_NOMENU))
    {
      $mod = getModule($modname);
      if (method_exists($mod,"getMenuItems")) $mod->getMenuItems();
    }
  }

  if (!isset($atkmenutop)||$atkmenutop=="") $atkmenutop = "main";
  
?>