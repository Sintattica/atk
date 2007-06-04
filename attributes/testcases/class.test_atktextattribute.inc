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
      var $m_node;
      
      /**
       * Setup the testcase.
       */
      function setup()
      {              
        $this->m_node = new atkNode();
        $this->m_node->add(new atkTextAttribute("remark"));
      }
      
      /**
       * Test method. 
       */
      function test_textattribute()
      {           
        $this->apiTest($this->m_node->getAttribute("remark"));
      }              
      
      function test_encoding()
      {
        // check encoding              
        $attribute = new atkTextAttribute("remark");
        $value = "contains'quote";
        $dbvalue = $attribute->value2db(array("remark"=>$value));
        $roundtrip = $attribute->db2value(array("remark"=>$value));      
               
        $this->assertEqual($value, $roundtrip, "value2db encoding check %s");      
      }
      
    }

?>