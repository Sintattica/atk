<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage db
   * @access private
   *
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Include the class we are about to test.
   */
  atkimport("atk.db.atkdb");
  atkimport("atk.db.atkmysqldb");  

  /**
   * Testcases for db abstraction
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage db
   * @access private
   */
  class test_atkdb extends UnitTestCase
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
    function testVendorDateFormat()
    {  
      $db = new atkDb();
      // Check generic db date format
      $this->assertEqual($db->vendorDateFormat("Y-m-d H:i:s"), "YYYY-MM-DD HH24:MI:SS", "generic vendor dateformat check");
      // Check mysql db date format
      
      $mysqldb = new atkMysqlDb();
      $this->assertEqual($mysqldb->vendorDateFormat("Y-m-d H:i:s"), "%Y-%m-%d %H:%i:%s", "mysql dateformat check");
    }    
    
  }

?>