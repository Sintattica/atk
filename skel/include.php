<?php
  $config_atkroot = "./";
  include_once("atk.inc");

  atksession();
  atksecure();

  $file = $ATK_VARS["file"];
  $allowed = atkconfig("allowed_includes");
  if (atk_in_array($file, $allowed))
    include_once(atkconfig("atkroot").$file);
?>
