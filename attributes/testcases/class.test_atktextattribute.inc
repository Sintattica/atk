<?php

    /**
     * This file is part of the Achievo ATK distribution.
     * Detailed copyright and licensing information can be found
     * in the doc/COPYRIGHT and doc/LICENSE files which should be 
     * included in the distribution.
     *
     * @package atk
     * @subpackage attributes
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
    atkimport("atk.attributes.atktextattribute");
  
    /**
     * Testcases for class atkTextAttribute.
     *
     * @author Ivo Jansch <ivo@achievo.org>
     * @package atk
     * @subpackage attributes
     * @access private
     */
    class test_atktextattribute extends atkAttributeTestCase
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
      function test_textattribute()
      {           
        $this->apiTest(new atkTextAttribute("remark"));
      }              
      
      function test_encoding()
      {
        // check encoding      
        global $g_db;
        $attribute = new atkTextAttribute("remark");
        $dbvalue = "contains'quote";
        $internalvalue = $attribute->db2value(array("remark"=>$dbvalue));      
        $roundtrip = $attribute->value2db(array("remark"=>$internalvalue));
        $this->assertEqual($g_db->escapeSQL($dbvalue), $roundtrip, "value2db encoding check %s");      
      }
      
    }

?>