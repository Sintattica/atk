<?
  // Setup the system
  require "atk/class.atknode.inc";

  require "class.".$atknodetype.".inc";
  $obj = new $atknodetype();

  if (count($HTTP_GET_VARS)>0)
  {
    $obj->dispatch($HTTP_GET_VARS);
  }
  else
  {
    $obj->dispatch($HTTP_POST_VARS);
  }

  $g_layout->outputFlush();

?>

