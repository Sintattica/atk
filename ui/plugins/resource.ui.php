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
 * @copyright (c) 2008 Ibuildings.nl
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 5230 $
 * $Id$
 */

/**
 * Smarty resource plug-in for fetching templates from the ATK UI.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
function smarty_resource_ui_source($tpl_name, &$tpl_source, &$smarty)
{
  $ui = atkUI::getInstance();
  $path = $ui->templatePath($tpl_name);

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

/**
 * Timestamp function for the ui resource
 */
function smarty_resource_ui_timestamp($tpl_name, &$tpl_timestamp, &$smarty)
{
  $ui = atkUI::getInstance();
  $path = $ui->templatePath($tpl_name);

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

/**
 * Security function for the ui resource.
 */
function smarty_resource_ui_secure($tpl_name, &$smarty)
{
  // assume all templates are secure
  return true;
}

/**
 * Trusted function for the ui resource.
 */
function smarty_resource_ui_trusted($tpl_name, &$smarty)
{
  // not used for templates
}