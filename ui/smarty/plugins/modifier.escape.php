<?php 
function smarty_modifier_escape($string)
{
  return nl2br(htmlentities($string, ENT_QUOTES, 'utf-8'));
}
?>
