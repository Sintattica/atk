<?php
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

    $grid = new atkDataGrid('test', $node);
    $this->assertTrue($grid->hasFlag(atkDataGrid::SEARCH));
    $this->assertTrue($grid->hasFlag(atkDataGrid::SORT));
    $this->assertTrue($grid->hasFlag(atkDataGrid::EXTENDED_SEARCH));
    $this->assertFalse($grid->hasFlag(atkDataGrid::LOCKING));
    $this->assertFalse($grid->hasFlag(atkDataGrid::MULTI_RECORD_ACTIONS));
    $this->assertFalse($grid->hasFlag(atkDataGrid::MULTI_RECORD_PRIORITY_ACTIONS));      
    $this->assertFalse($grid->hasFlag(atkDataGrid::EXTENDED_SORT));      
    
    $node->addFlag(NF_NO_SEARCH);
    $grid = new atkDataGrid('test', $node);
    $this->assertFalse($grid->hasFlag(atkDataGrid::SEARCH));
    
    $node->addFlag(NF_NO_SORT);
    $grid = new atkDataGrid('test', $node);
    $this->assertFalse($grid->hasFlag(atkDataGrid::SORT));

    $node->addFlag(NF_NO_EXTENDED_SEARCH);
    $grid = new atkDataGrid('test', $node);
    $this->assertFalse($grid->hasFlag(atkDataGrid::EXTENDED_SEARCH));

    $node->addFlag(NF_MRA);
    $grid = new atkDataGrid('test', $node);
    $this->assertTrue($grid->hasFlag(atkDataGrid::MULTI_RECORD_ACTIONS));
    
    $node->addFlag(NF_MRPA);
    $grid = new atkDataGrid('test', $node);
    $this->assertTrue($grid->hasFlag(atkDataGrid::MULTI_RECORD_PRIORITY_ACTIONS));
    
    $node->addFlag(NF_LOCK);
    $grid = new atkDataGrid('test', $node);
    $this->assertTrue($grid->hasFlag(atkDataGrid::LOCKING));    
    
    $node->addFlag(NF_EXT_SORT);
    $grid = new atkDataGrid('test', $node);
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

    $grid = new atkDataGrid('test', $node);
    $this->assertEqual('atk.datagrid.atkdglist', $grid->getComponent('list'));
    $this->assertEqual('atk.datagrid.atkdgsummary', $grid->getComponent('summary'));
    $this->assertEqual('atk.datagrid.atkdglimit', $grid->getComponent('limit'));
    $this->assertEqual('atk.datagrid.atkdgpaginator', $grid->getComponent('paginator'));
    $this->assertIsA($grid->getComponent('list', true), 'atkDGList');
    $this->assertIsA($grid->getComponent('summary', true), 'atkDGSummary');
    $this->assertIsA($grid->getComponent('limit', true), 'atkDGLimit');
    $this->assertIsA($grid->getComponent('paginator', true), 'atkDGPaginator');
    
    $node->m_index = 'test';
    $grid = new atkDataGrid('test', $node);
    $components = $grid->getComponents();
    $this->assertEqual('atk.datagrid.atkdgindex', $components['paginator']);
    $this->assertEqual('atk.datagrid.atkdgindex', $grid->getComponent('paginator'));
    $components = $grid->getComponents(true);
    $this->assertIsA($components['list'], 'atkDGList');
    $this->assertIsA($components['summary'], 'atkDGSummary');
    $this->assertIsA($components['limit'], 'atkDGLimit');
    $this->assertIsA($components['paginator'], 'atkDGIndex');   

    $grid = new atkDataGrid('test', $node);
    $this->assertNotNull($grid->getComponent('list'));    
    $this->assertEqual(4, count($grid->getComponents()));    
    $grid->removeComponent('list');   
    $this->assertEqual(3, count($grid->getComponents()));
    $this->assertNull($grid->getComponent('list'));
    
    $grid = new atkDataGrid('test', $node);
    $this->assertNull($grid->getComponent('another_paginator'));    
    $this->assertEqual(4, count($grid->getComponents()));        
    $grid->addComponent('another_paginator', 'atk.datagrid.atkdgpaginator');
    $this->assertEqual(5, count($grid->getComponents()));
    $this->assertNotNull($grid->getComponent('another_paginator'));    
  }
  
  /**
   * Test the postvars.
   */
  public function test_postvars()
  {
    $node = new atkNode("test");
    
    $node->m_postvars = array();
    $grid = new atkDataGrid('test', $node);
    $this->assertEqual(array(), $grid->getPostvars());
    $this->assertNull($grid->getPostvar('test'));
    $this->assertEqual('value', $grid->getPostvar('test', 'value'));  

    $node->m_postvars = array('test' => 'value');
    $grid = new atkDataGrid('test', $node);    
    $this->assertEqual(array(), $grid->getPostvars());
    $this->assertNull($grid->getPostvar('test'));
    
    $node->m_postvars = array('atkdg' => array('test' => array('test' => 'value')));
    $grid = new atkDataGrid('test', $node);    
    $this->assertEqual($node->m_postvars['atkdg']['test'], $grid->getPostvars());
    $this->assertEqual('value', $grid->getPostvar('test'));
    $this->assertEqual('value', $grid->getPostvar('test', 'fallback'));
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
    $grid = new atkDataGrid('test', $node);
    
    $grid->setSelectHandler(array($this, 'handleSelect'));
    $grid->setCountHandler(array($this, 'handleCount'));  
    $this->assertNull($grid->getRecords());
    $this->assertTrue($grid->getCount() === null);
    $this->assertTrue(is_array($grid->getRecords(true)));
    $this->assertEqual(1, count($grid->getRecords()));
    $this->assertEqual(84, $grid->getCount());        
  }
}