<?php namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Module;
use Sintattica\Atk\Ui\Smarty\Smarty;


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
     * @return Smarty The one and only instance.
     */
    public static function &getInstance()
    {
        static $s_smarty = null;
        if ($s_smarty == null) {
            Tools::atkdebug("Creating Smarty instance");

            // Warning: you'd think that the next line should read
            // $s_smarty = & new Smarty();
            // However, for some reason (php bug?) the static variable is no longer
            // static if we do that, and a new instance will be created on each
            // call.
            $s_smarty = new Smarty();

            // Initialize..

            $s_smarty->template_dir = Config::getGlobal("tplroot"); // name of directory for templates
            //Try to create the template compile directory if it not already exists.
            if (!file_exists(Config::getGlobal("tplcompiledir"))) {
                if (!mkdir(Config::getGlobal("tplcompiledir"), 0755, true)) {
                    Tools::atkerror("Unable to create template compile directory: " . Config::getGlobal("tplcompiledir"));
                }
            }
            $s_smarty->compile_dir = Config::getGlobal("tplcompiledir"); // name of directory for compiled templates

            $s_smarty->autoload_filters = array();    // indicates which filters will be auto-loaded

            $s_smarty->force_compile = Config::getGlobal("smarty_forcecompile");   // force templates to compile every time,
            // overrides cache settings. default false.

            $s_smarty->caching = Config::getGlobal("tplcaching");     // enable caching. can be one of 0/1/2.
            // 0 = no caching
            // 1 = use class cache_lifetime value
            // 2 = use cache_lifetime in cache file
            // default = 0.
            $s_smarty->cache_dir = Config::getGlobal("tplcachedir");    // name of directory for template cache files
            $s_smarty->use_sub_dirs = Config::getGlobal("tplusesubdirs"); // use subdirs for compiled and cached templates
            $s_smarty->cache_lifetime = Config::getGlobal("tplcachelifetime"); // number of seconds cached content will persist.
            // 0 = always regenerate cache,
            // -1 = never expires. default is one hour (3600)
            $s_smarty->cache_modified_check = true;                         // respect If-Modified-Since headers on cached content

            $s_smarty->default_template_handler_func = 'missing_template_handler'; // function to handle missing templates

            $s_smarty->php_handling = SMARTY_PHP_ALLOW;
            // how smarty handles php tags in the templates
            // possible values:
            // SMARTY_PHP_PASSTHRU -> echo tags as is
            // SMARTY_PHP_QUOTE    -> escape tags as entities
            // SMARTY_PHP_REMOVE   -> remove php tags
            // SMARTY_PHP_ALLOW    -> execute php tags
            // default: SMARTY_PHP_PASSTHRU

            $s_smarty->default_handler = Config::getGlobal("defaulthandler");
            $s_smarty->default_modifier = Config::getGlobal("defaultmodifier");

            // plugin dirs
            $s_smarty->plugins_dir = array(
                __DIR__.'/Smarty/plugins',
                __DIR__.'/plugins'
            );

            //$s_smarty->register_compiler_function("tpl","tpl_include");
            Tools::atkdebug("Instantiated new Smarty");
        }
        return $s_smarty;
    }

    /**
     * Add a plugin dir to Smarty.
     * @static
     * @param String $path The plugin dir to add
     */
    function addPluginDir($path)
    {
        $smarty = SmartyProvider::getInstance();
        $smarty->plugins_dir[] = $path;
    }

    /**
     * Returns the full path for the Smarty plug-in at the
     * given path (or inside the given module's plugins directory)
     * with the given name and type.
     *
     * @param string $path ATK path (without plugin filename!)
     * @param string $name plug-in name
     * @param string $type plug-in type (function, block etc.)
     *
     * @return string full path to plug-in
     *
     * @static
     * @private
     */
    function getPathForPlugin($path, $name, $type)
    {

        $fullPath = Tools::getClassPath($path, false) . '/' . $type . '.' . $name . '.php';
        return $fullPath;
    }

    /**
     * Register function / tag.
     *
     * NOTE: you should only use this function for
     *       tags with special names!
     *
     * @param string $moduleOrPath
     * @param string $tag
     * @param string $name
     * @param bool $cacheable
     * @param string $cache_attrs
     */
    function addFunction($moduleOrPath, $tag, $name = "", $cacheable = true, $cache_attrs = null)
    {
        $smarty = SmartyProvider::getInstance();

        $name = empty($name) ? $tag : $name;
        $function = "__smarty_function_$name";

        $path = SmartyProvider::getPathForPlugin($moduleOrPath, $name, 'function');

        eval('
      function ' . $function . '($params, &$smarty)
      {
        include_once("' . $path . '");
        return smarty_function_' . $name . '($params, $smarty);
      }
    ');

        $smarty->register_function($tag, $function, $cacheable, $cache_attrs);
    }

    /**
     * Register dynamic function / tag.
     *
     * @param string $moduleOrPath
     * @param string $tag
     * @param string $cache_attrs
     */
    function addDynamicFunction($moduleOrPath, $tag, $cache_attrs = null)
    {
        SmartyProvider::addFunction($moduleOrPath, $tag, $tag, false, $cache_attrs);
    }

    /**
     * Register compiler function / tag.
     *
     * NOTE: you should only use this function for
     *       tags with special names!
     *
     * @param string $moduleOrPath
     * @param string $tag
     * @param string $name
     * @param string $cacheable
     */
    function addCompilerFunction($moduleOrPath, $tag, $name = "", $cacheable = true)
    {
        $smarty = SmartyProvider::getInstance();

        $name = empty($name) ? $tag : $name;
        $function = "__smarty_compiler_$name";

        $path = self::getPathForPlugin($moduleOrPath, $name, 'compiler');

        eval('
      function ' . $function . '($tag_arg, &$smarty)
      {
        include_once("' . $path . '");
        return smarty_compiler_' . $name . '($tag_arg, $smarty);
      }
    ');

        $smarty->register_compiler_function($tag, $function, $cacheable);
    }

    /**
     * Register block / tag.
     *
     * NOTE: you should only use this function for
     *       tags with special names!
     *
     * @param string $moduleOrPath
     * @param string $tag
     * @param string $name
     * @param bool $cacheable
     */
    function addBlock($moduleOrPath, $tag, $name = "", $cacheable = true)
    {
        $smarty = SmartyProvider::getInstance();

        $name = empty($name) ? $tag : $name;
        $function = "__smarty_block_$name";

        $path = SmartyProvider::getPathForPlugin($moduleOrPath, $name, 'block');

        eval('
      function ' . $function . '($params, $content, &$smarty, &$repeat)
      {
        include_once("' . $path . '");
        return smarty_block_' . $name . '($params, $content, $smarty, $repeat);
      }
    ');

        $smarty->register_block($tag, $function, $cacheable);
    }

    /**
     * Register dynamic function / tag.
     *
     * @param string $moduleOrPath
     * @param string $tag
     */
    function addDynamicBlock($moduleOrPath, $tag)
    {
        SmartyProvider::addBlock($moduleOrPath, $tag, $tag, false);
    }

    /**
     * Register modifier
     *
     * @param string $moduleOrPath
     * @param string $tag
     * @param string $name
     */
    function addModifier($moduleOrPath, $tag, $name = "")
    {
        $smarty = SmartyProvider::getInstance();

        $name = empty($name) ? $tag : $name;
        $function = "__smarty_modifier_$name";

        $path = SmartyProvider::getPathForPlugin($moduleOrPath, $name, 'modifier');

        eval('
      function ' . $function . '($variable)
      {
        include_once("' . $path . '");
        return smarty_modifier_' . $name . '(func_get_args());
      }
    ');

        $smarty->register_modifier($tag, $function);
    }

    /**
     * Add output filter
     *
     * @param string $function The function to use as outputfilter
     */
    function addOutputFilter($function)
    {
        $smarty = SmartyProvider::getInstance();
        $smarty->register_outputfilter($function);
    }

}

/**
 * After this line, we register the base ATK dynamic Smarty plug-ins. Unfortunately
 * Smarty's plug-in system doesn't allow the detection of dynamic plug-ins based solely
 * on the plug-in's filename. Non-dynamic plug-ins should be placed in the plugins/ subdir,
 * but shouldn't be registered here (Smarty will detect them automatically).
 */
SmartyProvider::addDynamicFunction('atk.ui.plugins', 'atkfrontcontroller');
