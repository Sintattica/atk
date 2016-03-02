<?php namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;



/**
 * Wrapper class for the Smarty template engine.
 * This class instantiates Smarty and configures it for use in ATK.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage ui
 *
 */
class SmartyProvider
{
    /**
     * Get the Smarty instance.
     *
     * SmartyProvider is a singleton.
     *
     * @static
     * @return \Smarty The one and only instance.
     */
    public static function &getInstance()
    {
        static $s_smarty = null;
        if ($s_smarty == null) {
            Tools::atkdebug("Creating Smarty instance");

            // Warning: you'd think that the next line should read
            // However, for some reason (php bug?) the static variable is no longer
            // static if we do that, and a new instance will be created on each
            // call.
            $s_smarty = new \Smarty();

            // Initialize..

            $s_smarty->setTemplateDir(Config::getGlobal("template_dir")); // name of directory for templates

            //Try to create the template compile directory if it not already exists.
            if (!file_exists(Config::getGlobal("tplcompiledir"))) {
                if (!mkdir(Config::getGlobal("tplcompiledir"), 0755, true)) {
                    Tools::atkerror("Unable to create template compile directory: " . Config::getGlobal("tplcompiledir"));
                }
            }
            $s_smarty->setCompileDir(Config::getGlobal("tplcompiledir")); // name of directory for compiled templates

            $s_smarty->autoload_filters = array();    // indicates which filters will be auto-loaded

            $s_smarty->force_compile = Config::getGlobal("smarty_forcecompile");   // force templates to compile every time,
            // overrides cache settings. default false.

            $s_smarty->setCaching(Config::getGlobal("tplcaching"));
            // 0 = no caching
            // 1 = use class cache_lifetime value
            // 2 = use cache_lifetime in cache file
            // default = 0.
            $s_smarty->setCacheDir(Config::getGlobal("tplcachedir"));    // name of directory for template cache files
            $s_smarty->setUseSubDirs(Config::getGlobal("tplusesubdirs")); // use subdirs for compiled and cached templates
            $s_smarty->setCacheLifetime(Config::getGlobal("tplcachelifetime")); // number of seconds cached content will persist.
            // 0 = always regenerate cache,
            // -1 = never expires. default is one hour (3600)

            //TODO: da mappare
            //$s_smarty->cache_modified_check = true;                         // respect If-Modified-Since headers on cached content



           // $s_smarty->default_template_handler_func = 'missing_template_handler'; // function to handle missing templates

            //TODO: da mappare
            //$s_smarty->php_handling = SMARTY_PHP_ALLOW;
            // how smarty handles php tags in the templates
            // possible values:
            // SMARTY_PHP_PASSTHRU -> echo tags as is
            // SMARTY_PHP_QUOTE    -> escape tags as entities
            // SMARTY_PHP_REMOVE   -> remove php tags
            // SMARTY_PHP_ALLOW    -> execute php tags
            // default: SMARTY_PHP_PASSTHRU

            /*
            $s_smarty->default_handler = Config::getGlobal("defaulthandler");
            $s_smarty->default_modifier = Config::getGlobal("defaultmodifier");
            */

            // plugin dirs
            $s_smarty->addPluginsDir( __DIR__ . '/plugins');


            Tools::atkdebug("Instantiated new Smarty");

        }
        return $s_smarty;
    }
}