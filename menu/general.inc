<?php

  include_once(atkconfig("atkroot")."atk/atkmenutools.inc");
  
  $atkmenutop = $_REQUEST["atkmenutop"];

  /* first add module menuitems */
  foreach ($g_modules as $modname => $modpath)
  {
    if (!hasFlag($g_moduleflags[$modname], MF_NOMENU))
    {
      $mod = getModule($modname);
      if (method_exists($mod,"getMenuItems")) $mod->getMenuItems();
    }
  }

  if (!isset($atkmenutop)||$atkmenutop=="") $atkmenutop = "main";

?>
