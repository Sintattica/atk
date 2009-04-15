<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage test
   *
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 4362 $
   * $Id$
   */
  
  /**
   * atkPluginTestCase is a testcase class for testing Smarty plugins.
   * 
   * It is different from the regular atkTestCase in that it features
   * utility methods to call smarty plugins.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage test
   *
   */
  class atkPluginTestCase extends atkTestCase 
  {
    /**
     * Run an arbitrary Smarty plugin
     *
     * @param String $type The type of plugin (block, function)
     * @param String $plugin The name of the plugin
     * @param array $params Any parameter to pass to the plugin
     * @return String the result of the plugin.
     */
    function runPlugin($type, $plugin, $params=array())
    {
      $smarty = &atkSmarty::getInstance();
      include_once(atkconfig("atkroot")."atk/ui/smarty/internals/core.load_plugins.php");
      include_once($smarty->_get_plugin_filepath($type, $plugin));
      $funcname = "smarty_function_".$plugin;
      return $funcname($params, $smarty);
    }
    
    /**
     * Run a Smarty plugin of the 'function' type. 
     *     
     * @param String $plugin The name of the plugin
     * @param array $params Any parameter to pass to the plugin
     * @return String the result of the plugin.
     */
    function runFunction($plugin, $params=array())
    {
      return $this->runPlugin("function", $plugin, $params);
    }
    
    /**
     * Run a Smarty plugin of the 'block' type. 
     *     
     * @param String $plugin The name of the plugin
     * @param array $params Any parameter to pass to the plugin
     * @return String the result of the plugin.
     */
    function runBlock($plugin, $params=array())
    {
      return $this->runPlugin("block", $plugin, $params);
    }
  }

?>