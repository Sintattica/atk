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
     * Get the atkSmarty  instance.
     *
     * atkSmarty is a singleton.
     *
     * @static
     * @return atkSmarty The one and only instance.
     */
    function &getInstance()
    {
      static $s_smarty = NULL;
      if ($s_smarty == NULL)
      {
        atkdebug("Creating Smarty instance");
        include_once(atkconfig("atkroot")."atk/ui/smarty/Smarty.class.php");
        
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
  }

?>