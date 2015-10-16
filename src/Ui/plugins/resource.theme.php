<?php

use Sintattica\Atk\Ui\Theme;
use Sintattica\Atk\Core\Config;

/**
 * Smarty resource plug-in for fetching templates from the ATK theme.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
function smarty_resource_theme_source($tpl_name, &$tpl_source, &$smarty)
{
    $theme = Theme::getInstance();
    $path = $theme->tplPath($tpl_name);

    if (!empty($path)) {
        $tpl_source = file_get_contents($path);

        if (Config::getGlobal('debug') >= 3) {
            $tpl_source = "\n<!-- START [{$path}] -->\n" .
                $tpl_source .
                "\n<!-- END [{$path}] -->\n";
        }

        return true;
    } else {
        return false;
    }
}

/**
 * Timestamp function for the theme resource
 */
function smarty_resource_theme_timestamp($tpl_name, &$tpl_timestamp, &$smarty)
{
    $theme = Theme::getInstance();
    $path = $theme->tplPath($tpl_name);

    if (!empty($path)) {
        $tpl_timestamp = filemtime($path);
        return true;
    } else {
        return false;
    }
}

/**
 * Security function for the theme resource.
 */
function smarty_resource_theme_secure($tpl_name, &$smarty)
{
    // assume all templates are secure
    return true;
}

/**
 * Trusted function for the theme resource.
 */
function smarty_resource_theme_trusted($tpl_name, &$smarty)
{
    // not used for templates
}