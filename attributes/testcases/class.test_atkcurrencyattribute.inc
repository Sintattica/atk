<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage attribute
   * @access private
   *
   * @copyright (c)2007 Ibuildings
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * $Id: class.test_atknumberattribute.inc 5272 2007-09-10 21:06:19Z yury $
   * @author Yury Golovnya  <ygolovnya@achievo.org>
   * @version $Revision: 5272 $
   */


  /**
   * @internal includes
   */
  atkimport("atk.attributes.atkcurrencyattribute");

  /**
   * Testcases for class atkCurrencyAttribute
   *
   * @package atk
   * @subpackage attribute
   * @access private
   */
  class test_atkcurrencyattribute extends atkAttributeTestCase
  {
    private $attr;
    
    /**
     * Setup the testcase.
     */
    function setup()
    {
      
    }

    /**
     * Test method.
     */
    function test_attribute()
    {           
      $this->apiTest(new atkCurrencyAttribute(("name")));
    }              

    function test_create_default()
    {
     $attr = new atkCurrencyAttribute('name');
     
     $this->assertEqual('.', $attr->getDecimalSeparator());
     $this->assertEqual(',', $attr->getThousandsSeparator());
    }
    
    function test_create_different_separators()
    {
     $attr = new atkCurrencyAttribute('name',0,10,"EUR",2,",",".");
     
     $this->assertEqual(',', $attr->getDecimalSeparator());
     $this->assertEqual('.', $attr->getThousandsSeparator());
    }
    
    function test_thousand_seperator()
    {
      $attr = new atkCurrencyAttribute('name'); 
      $this->assertTrue($attr->getUseThousandsSeparator());
    }
      
    
  }
?>