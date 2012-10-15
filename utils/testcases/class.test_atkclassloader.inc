<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   * @access private
   *
   * @copyright (c)2005 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6102 $
   * $Id$
   */

  /**
   * Include the class we are about to test.
   */
  atkimport("atk.utils.atkclassloader");  

  /**
   * Testcases for class atkclassloader.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   * @access private
   */
  class test_atkclassloader extends atkTestCase 
  {        
    /**
     * Setup the testcase.
     */
    function setup()
    {        
      parent::setUp();   
    }
    
    /**
     * Test overloading
     */
    function test_overloading()
    {
      // base
      $a = &atknew("atk.utils.atkstringparser", "hmm");
      $this->assertIsA($a, "atkStringParser", "original stringparser");
      $this->assertFalse(atkClassLoader::hasOverloader("atk.utils.atkstringparser"), "stringparser has no overloader");
      
      // overloaded
      atkClassLoader::addOverloader("atk.utils.atkstringparser", "atk.utils.atkdirectorytraverser");
      $b = &atknew("atk.utils.atkstringparser", "hmm");
      $this->assertIsA($b, "atkDirectoryTraverser", "overloaded stringparser with dirtraverser");
      $this->assertTrue(atkClassLoader::hasOverloader("atk.utils.atkstringparser"), "stringparser has an overloader");
      
      // Info
      $this->assertEqual(atkClassLoader::resolveClass("atk.utils.atkstringparser"), "atk.utils.atkdirectorytraverser", "atkClassLoader::resolveClass retrieves final class");
      // Also test the bwc convenience method.
      $this->assertEqual(atkResolveClass("atk.utils.atkstringparser"), "atk.utils.atkdirectorytraverser", "atkResolveClassretrieves final class");
      
      // restored
      atkClassLoader::removeOverloader("atk.utils.atkstringparser");
      $c = &atknew("atk.utils.atkstringparser", "hmm");
      $this->assertIsA($c, "atkStringParser", "original stringparser back");
      $this->assertFalse(atkClassLoader::hasOverloader("atk.utils.atkstringparser"), "stringparser has no overloader anymore");
    }
    
    /**
     * Test invokeFromString method. 
     */
    function test_stringinvocation()
    {  
      // Call with nonexistant class
      $this->assertFalse(atkClassLoader::invokeFromString("nonexistingclass#nomethod"));
      
      // call without method
      $this->assertFalse(atkClassLoader::invokeFromString("atk.utils.atkstringparser"));
      
      // call with empty method
      $this->assertFalse(atkClassLoader::invokeFromString("atk.utils.atkstringparser#"));
      
      // correct calls
      $this->assertTrue(atkClassLoader::invokeFromString("atk.utils.testcases.test_atkclassloader#methodtrue"));
      $this->assertEqual("Hello World", atkClassLoader::invokeFromString("atk.utils.testcases.test_atkclassloader#methodhelloworld"), 'with returnvalue %s');
      
      // call with 2 params
      $this->assertEqual("Blue Banana", atkClassLoader::invokeFromString("atk.utils.testcases.test_atkclassloader#methodparams", array("Blue", "Banana")), "with params %s");      
    }    
    
    /**
     * Test if we can mount a class path.
     */
    function test_mount_class_path()
    {
      $path = atkconfig('atkroot').'../fake/path/';

      $this->assertEqual(atkClassLoader::getClassPath('test_prefix.myclass'), atkconfig('atkroot').'test_prefix/class.myclass.inc'); 
      $this->assertEqual(atkClassLoader::getClassPath('test_prefix.nested.myclass'), atkconfig('atkroot').'test_prefix/nested/class.myclass.inc');      
      
      atkClassLoader::mountClassPath('test_prefix', $path);
      
      $this->assertEqual(atkClassLoader::getClassPath('test_prefix.myclass'), $path.'class.myclass.inc'); 
      $this->assertEqual(atkClassLoader::getClassPath('test_prefix.nested.myclass'), $path.'nested/class.myclass.inc');      
    }
    
    /**
     * Utility method for classloader test.
     */
    function methodtrue()
    {
      return true;
    }
    
    /**
     * Utility method for classloader test.
     */
    function methodHelloWorld()
    {
      return "Hello World";
    }
    
    /**
     * Utility method for classloader test.
     */
    function methodParams($params)
    {
      return implode(" ", $params);
    }
    
  }

?>