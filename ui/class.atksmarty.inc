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
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Wrapper class for the Smarty template engine.
 * This class instantiates Smarty and configures it for use in ATK.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage ui
 *
 */
class atkSmarty
{

  /**
   * Get the Smarty  instance.
   *
   * atkSmarty is a singleton.
   *
   * @static
   * @return Smarty The one and only instance.
   */
  function &getInstance()
  {
    static $s_smarty = NULL;
    if ($s_smarty == NULL)
    {
      atkdebug("Creating Smarty instance");
      if (!class_exists("Smarty"))
      {
        include_once(atkconfig("atkroot")."atk/ui/smarty/Smarty.class.php");
      }

      // Warning: you'd think that the next line should read
      // $s_smarty = & new Smarty();
      // However, for some reason (php bug?) the static variable is no longer
      // static if we do that, and a new instance will be created on each
      // call.
      $s_smarty = new Smarty();

      // Initialize..

      $s_smarty->template_dir    = atkconfig("tplroot"); // name of directory for templates
      $s_smarty->compile_dir     = atkconfig("tplcompiledir"); // name of directory for compiled templates

      $s_smarty->autoload_filters = array();    // indicates which filters will be auto-loaded

      $s_smarty->force_compile   = atkconfig("smarty_forcecompile");   // force templates to compile every time,
                                                                      // overrides cache settings. default false.

      $s_smarty->caching         = atkconfig("tplcaching");     // enable caching. can be one of 0/1/2.
                                                                      // 0 = no caching
                                                                      // 1 = use class cache_lifetime value
                                                                      // 2 = use cache_lifetime in cache file
                                                                      // default = 0.
      $s_smarty->cache_dir       = atkconfig("tplcachedir");    // name of directory for template cache files
      $s_smarty->use_sub_dirs    = atkconfig("tplusesubdirs"); // use subdirs for compiled and cached templates
      $s_smarty->cache_lifetime  = atkconfig("tplcachelifetime"); // number of seconds cached content will persist.
  									                                                        // 0 = always regenerate cache,
                                                                            // -1 = never expires. default is one hour (3600)
      $s_smarty->cache_modified_check = true;                         // respect If-Modified-Since headers on cached content

      $s_smarty->default_template_handler_func = 'missing_template_handler'; // function to handle missing templates

      $s_smarty->php_handling    =  SMARTY_PHP_ALLOW;
                                        // how smarty handles php tags in the templates
                                        // possible values:
                                        // SMARTY_PHP_PASSTHRU -> echo tags as is
                                        // SMARTY_PHP_QUOTE    -> escape tags as entities
                                        // SMARTY_PHP_REMOVE   -> remove php tags
                                        // SMARTY_PHP_ALLOW    -> execute php tags
                                        // default: SMARTY_PHP_PASSTHRU

      $s_smarty->default_handler = atkconfig("defaulthandler");
      $s_smarty->default_modifier = atkconfig("defaultmodifier");

      // plugin dirs
      $s_smarty->plugins_dir = array(atkconfig("atkroot")."atk/ui/smarty/plugins",
                                     atkconfig("atkroot")."atk/ui/plugins");
      $customplugindir = atkconfig("tplplugindir");
      if ($customplugindir!="") $s_smarty->plugins_dir[] = $customplugindir;

      //$s_smarty->register_compiler_function("tpl","tpl_include");
      atkdebug("Instantiated new Smarty");
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
    $smarty = &atkSmarty::getInstance();
    $smarty->plugins_dir[] = $path;
  }

  /**
   * Returns the full path for the Smarty plug-in at the
   * given path (or inside the given module's plugins directory)
   * with the given name and type.
   *
   * @param string $moduleOrPath module name or full ATK path (without plugin filename!)
   * @param string $name         plug-in name
   * @param string $type         plug-in type (function, block etc.)
   *
   * @return string full path to plug-in
   *
   * @static
   * @private
   */
  function getPathForPlugin($moduleOrPath, $name, $type)
  {
    if (moduleExists($moduleOrPath))
    {
      $path = 'module.'.$moduleOrPath.'.plugins';
    }
    else
    {
      $path = $moduleOrPath;
    }

    $fullPath = getClassPath($path,false).'/'.$type.'.'.$name.'.php';
    return $fullPath;
  }

  /**
   * Register function / tag.
   *
   * NOTE: you should only use this function for
   *       tags with special names!
   */
  function addFunction($moduleOrPath, $tag, $name="", $cacheable=true, $cache_attrs=null)
  {
    $smarty = &atkSmarty::getInstance();

    $name = empty($name) ? $tag : $name;
    $function = "__smarty_function_$name";

    $path = atkSmarty::getPathForPlugin($moduleOrPath, $name, 'function');

    eval('
      function '.$function.'($params, &$smarty)
      {
        include_once("'.$path.'");
        return smarty_function_'.$name.'($params, $smarty);
      }
    ');

    $smarty->register_function($tag, $function, $cacheable, $cache_attrs);
  }

  /**
   * Register dynamic function / tag.
   */
  function addDynamicFunction($moduleOrPath, $tag, $cache_attrs=null)
  {
    atkSmarty::addFunction($moduleOrPath, $tag, $tag, false, $cache_attrs);
  }

  /**
   * Register compiler function / tag.
   *
   * NOTE: you should only use this function for
   *       tags with special names!
   */
  function addCompilerFunction($moduleOrPath, $tag, $name="", $cacheable=true)
  {
    $smarty = &atkSmarty::getInstance();

    $name = empty($name) ? $tag : $name;
    $function = "__smarty_compiler_$name";

    $path = atkSmarty::getPathForPlugin($moduleOrPath, $name, 'compiler');

    eval('
      function '.$function.'($tag_arg, &$smarty)
      {
        include_once("'.$path.'");
        return smarty_compiler_'.$name.'($tag_arg, $smarty);
      }
    ');

    $smarty->register_compiler_function($tag, $function, $cacheable);
  }

  /**
   * Register block / tag.
   *
   * NOTE: you should only use this function for
   *       tags with special names!
   */
  function addBlock($moduleOrPath, $tag, $name="", $cacheable=true)
  {
    $smarty = &atkSmarty::getInstance();

    $name = empty($name) ? $tag : $name;
    $function = "__smarty_block_$name";

    $path = atkSmarty::getPathForPlugin($moduleOrPath, $name, 'block');

    eval('
      function '.$function.'($params, $content, &$smarty, &$repeat)
      {
        include_once("'.$path.'");
        return smarty_block_'.$name.'($params, $content, $smarty, $repeat);
      }
    ');

    $smarty->register_block($tag, $function, $cacheable);
  }

  /**
   * Register dynamic function / tag.
   */
  function addDynamicBlock($moduleOrPath, $tag)
  {
    atkSmarty::addBlock($moduleOrPath, $tag, $tag, false);
  }

  /**
   * Register modifier
   */
  function addModifier($moduleOrPath, $tag, $name="")
  {
    $smarty = &atkSmarty::getInstance();

    $name = empty($name) ? $tag : $name;
    $function = "__smarty_modifier_$name";

    $path = atkSmarty::getPathForPlugin($moduleOrPath, $name, 'modifier');

    eval('
      function '.$function.'($variable)
      {
        include_once("'.$path.'");
        return smarty_modifier_'.$name.'(func_get_args());
      }
    ');

    $smarty->register_modifier($tag, $function);
  }

  function addOutputFilter($function)
  {
    $smarty = &atkSmarty::getInstance();
    $smarty->register_outputfilter($function);
  }
}

/**
 * After this line, we register the base ATK dynamic Smarty plug-ins. Unfortunately
 * Smarty's plug-in system doesn't allow the detection of dynamic plug-ins based solely
 * on the plug-in's filename. Non-dynamic plug-ins should be placed in the plugins/ subdir,
 * but shouldn't be registered here (Smarty will detect them automatically).
 */
atkSmarty::addDynamicFunction('atk.ui.plugins', 'atkfrontcontroller');