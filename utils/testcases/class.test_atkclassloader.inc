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
   * @version $Revision$
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
  class test_atkclassloader extends UnitTestCase
  {        
    /**
     * Setup the testcase.
     */
    function setup()
    {          
    }
    
    /**
     * Test method. 
     */
    function test()
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