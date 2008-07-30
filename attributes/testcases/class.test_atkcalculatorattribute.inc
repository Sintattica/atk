<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage testcases
   *
   * @copyright (c)2000-2008 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   */

/**
 * Tests for the atkCalculatorAttribute.
 * @package atk
 * @subpackage testcases
 * @internal
 */
class test_atkCalculatorAttribute extends atkTestCase 
{
  var $m_node;
  var $m_attr;
  
  /**
   * Setup the testcase.
   */
  function setup()
  {              
    parent::setUp(); 
    useattrib('atkcalculatorattribute');
    $this->m_node = new atkNode();
    $this->m_attr = &$this->m_node->add(new atkCalculatorAttribute("total", ""));
  }
      
  /**
   * Test display() method - without it partial_refresh() don't work, because in
   * refesh mode load() method of attribute don't call
   *
   */
  public function test_display()
  {
    $this->m_attr->m_calculation = "([price]*[quantity])+[vat]";
        
    $record['price'] = '10.5';
    $record['quantity'] = '5';
    $record['vat'] = '25';
    $record['total'] = '10';
    
    $this->m_node->m_partial = 'attribute.total.refresh';
    $this->assertEqual($this->m_attr->display($record), 10.5*5+25,"check refresh of calculator attribute");    

    //test display in non-partial mode
    $this->m_node->m_partial = null;
    $this->assertEqual($this->m_attr->display($record), 10,"check refresh in non-partial mode");    

    //test display when other attribute refresh
    $this->m_node->m_partial = 'attribute.price.refresh';
    $this->assertEqual($this->m_attr->display($record), 10,"check when other attribute refresh");    
  }
}
?>
