<?php

  /* first add module menuitems */
  foreach ($g_modules as $modname => $modpath)
  {
    $mod = getModule($modname);
    if (method_exists($mod,"getMenuItems")) $mod->getMenuItems(); 
  }

  if (!isset($atkmenutop)||$atkmenutop=="") $atkmenutop = "main";

?>
