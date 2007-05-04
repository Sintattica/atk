<?php
/**
 * Smarty resource plug-in for fetching templates from the ATK theme.
 * 
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 */

function smarty_resource_theme_source($tpl_name, &$tpl_source, &$smarty)
{
  $theme = &atkTheme::getInstance();
  $path = $theme->tplPath($tpl_name);
  
  if (!empty($path))
  {
    $tpl_source = file_get_contents($path);
    return true;
  }
  else 
  {
    return false;
  }
}

function smarty_resource_theme_timestamp($tpl_name, &$tpl_timestamp, &$smarty)
{
  $theme = &atkTheme::getInstance();
  $path = $theme->tplPath($tpl_name);
  
  if (!empty($path))
  {
    $tpl_timestamp = filemtime($path);
    return true;
  }
  else 
  {
    return false;
  }
}

function smarty_resource_theme_secure($tpl_name, &$smarty)
{
  // assume all templates are secure
  return true;
}

function smarty_resource_theme_trusted($tpl_name, &$smarty)
{
  // not used for templates
}
?>