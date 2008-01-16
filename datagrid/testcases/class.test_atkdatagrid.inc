<?php

if (false)
{
atkimport('atk.datagrid.atkdatagrid');

/**
 * Tests for the atkDataGrid class.
 */
class test_atkDataGrid extends atkTestCase
{
  /**
   * Test the flags conversion.
   */
  public function test_flags()
  {
    $node = new atkNode("test");

    $grid = atkDataGrid::create($node, 'test');
    $this->assertTrue($grid->hasFlag(atkDataGrid::SEARCH));
    $this->assertTrue($grid->hasFlag(atkDataGrid::SORT));
    $this->assertTrue($grid->hasFlag(atkDataGrid::EXTENDED_SEARCH));
    $this->assertFalse($grid->hasFlag(atkDataGrid::LOCKING));
    $this->assertFalse($grid->hasFlag(atkDataGrid::MULTI_RECORD_ACTIONS));
    $this->assertFalse($grid->hasFlag(atkDataGrid::MULTI_RECORD_PRIORITY_ACTIONS));      
    $this->assertFalse($grid->hasFlag(atkDataGrid::EXTENDED_SORT));      
    
    $node->addFlag(NF_NO_SEARCH);
    $grid = atkDataGrid::create($node, 'test');
    $this->assertFalse($grid->hasFlag(atkDataGrid::SEARCH));
    
    $node->addFlag(NF_NO_SORT);
    $grid = atkDataGrid::create($node, 'test');
    $this->assertFalse($grid->hasFlag(atkDataGrid::SORT));

    $node->addFlag(NF_NO_EXTENDED_SEARCH);
    $grid = atkDataGrid::create($node, 'test');
    $this->assertFalse($grid->hasFlag(atkDataGrid::EXTENDED_SEARCH));

    $node->addFlag(NF_MRA);
    $grid = atkDataGrid::create($node, 'test');
    $this->assertTrue($grid->hasFlag(atkDataGrid::MULTI_RECORD_ACTIONS));
    
    $node->addFlag(NF_MRPA);
    $grid = atkDataGrid::create($node, 'test');
    $this->assertTrue($grid->hasFlag(atkDataGrid::MULTI_RECORD_PRIORITY_ACTIONS));
    
    $node->addFlag(NF_LOCK);
    $grid = atkDataGrid::create($node, 'test');
    $this->assertTrue($grid->hasFlag(atkDataGrid::LOCKING));    
    
    $node->addFlag(NF_EXT_SORT);
    $grid = atkDataGrid::create($node, 'test');
    $this->assertFalse($grid->hasFlag(atkDataGrid::SEARCH));
    $this->assertFalse($grid->hasFlag(atkDataGrid::SORT));
    $this->assertFalse($grid->hasFlag(atkDataGrid::EXTENDED_SEARCH));
    $this->assertTrue($grid->hasFlag(atkDataGrid::LOCKING));
    $this->assertTrue($grid->hasFlag(atkDataGrid::MULTI_RECORD_ACTIONS));
    $this->assertTrue($grid->hasFlag(atkDataGrid::MULTI_RECORD_PRIORITY_ACTIONS));      
    $this->assertTrue($grid->hasFlag(atkDataGrid::EXTENDED_SORT));   
  }
  
  /**
   * Test the components.
   */
  public function test_components()
  {
    $node = new atkNode("test");

    $grid = atkDataGrid::create($node, 'test');
    $this->assertEqual(5, count($grid->getComponents()));
    $this->assertEqual(array('class' => 'atk.datagrid.atkdglist', 'options' => array()), $grid->getComponent('list'));
    $this->assertEqual(array('class' => 'atk.datagrid.atkdgsummary', 'options' => array()), $grid->getComponent('summary'));
    $this->assertEqual(array('class' => 'atk.datagrid.atkdglimit', 'options' => array()), $grid->getComponent('limit'));
    $this->assertEqual(array('class' => 'atk.datagrid.atkdgpaginator', 'options' => array()), $grid->getComponent('paginator'));
    $this->assertEqual(array('class' => 'atk.datagrid.atkdgnorecordsfound', 'options' => array()), $grid->getComponent('norecordsfound'));
    
    $node->m_index = 'test';
    $grid = atkDataGrid::create($node, 'test');
    $components = $grid->getComponents();
    $this->assertEqual(6, count($components));
    $this->assertEqual(array('class' => 'atk.datagrid.atkdgindex', 'options' => array()), $grid->getComponent('index'));
    
    $grid = atkDataGrid::create($node, 'test');
    $this->assertNotNull($grid->getComponent('list'));    
    $this->assertEqual(6, count($grid->getComponents()));    
    $grid->removeComponent('list');   
    $this->assertEqual(5, count($grid->getComponents()));
    $this->assertNull($grid->getComponent('list'));
    
    $grid = atkDataGrid::create($node, 'test');
    $this->assertNull($grid->getComponent('another_paginator'));    
    $this->assertEqual(6, count($grid->getComponents()));        
    $grid->addComponent('another_paginator', 'atk.datagrid.atkdgpaginator');
    $this->assertEqual(7, count($grid->getComponents()));
    $this->assertNotNull($grid->getComponent('another_paginator'));    
  }
  
  /**
   * Test the postvars.
   */
  public function test_postvars()
  {
    global $ATK_VARS;
    
    $node = new atkNode("test");
    
    $backup = $ATK_VARS;
    
    $ATK_VARS = array();
    $grid = atkDataGrid::create($node, 'test');
    $this->assertEqual(1, count($grid->getPostvars()));
    $this->assertNotNull($grid->getPostvar('atkdg'));
    $this->assertNull($grid->getPostvar('test'));
    $this->assertEqual('value', $grid->getPostvar('test', 'value'));  

    $node->m_postvars = array('test' => 'value');
    $grid = atkDataGrid::create($node, 'test');    
    $this->assertEqual('value', $grid->getPostvar('test'));
    
    $ATK_VARS = array('atkdg' => array('test' => array('test' => 'value')));
    $grid = atkDataGrid::create($node, 'test');    
    $this->assertEqual('value', $grid->getPostvar('test'));
    $this->assertEqual('value', $grid->getPostvar('test', 'fallback'));
    
    $ATK_VARS = $backup;
  }
  
  /**
   * Dummy select handler for use in the test_select_and_count_handlers
   * test method.
   */
  public function handleSelect($grid)
  {
    $this->assertIsA($grid, 'atkDataGrid');
    return array(0 => array('test' => 'value'));
  }
  
  /**
   * Dummy count handler for use in the test_select_and_count_handlers
   * test method.
   */
  public function handleCount($grid)
  {
    $this->assertIsA($grid, 'atkDataGrid');
    return 84;
  }  
  
  /**
   * Test the select and count handlers.
   */
  public function test_select_and_count_handlers()
  {
    $node = new atkNode("test");
    $grid = atkDataGrid::create($node, 'test');
    
    $grid->setSelectHandler(array($this, 'handleSelect'));
    $grid->setCountHandler(array($this, 'handleCount'));  
    $this->assertNull($grid->getRecords());
    $this->assertTrue($grid->getCount() === null);
    $this->assertTrue(is_array($grid->getRecords(true)));
    $this->assertEqual(1, count($grid->getRecords()));
    $this->assertEqual(84, $grid->getCount());        
  }
}
}
else
{
  class test_atkdatagrid extends atkTestCase {}
}
?>
