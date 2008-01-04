<?php

atkimport('atk.atkmocknode');
atkimport('atk.handlers.atkattributeedithandler');

/**
 * atkAttributeEditHandler test-case.
 */
class test_atkattributeedithandler extends atkTestCase 
{
  /**
   * Tests the setMasterNode method of the atkAttributeEditHandler which
   * adds support setting a masternode that should handle the partial calls.
   */
  public function test_masternode()
  {
    $node         = &new atkMockNode("somenode");
    $masterNode   = &new atkMockNode("masternode");

    $handler = &new atkAttributeEditHandler();
    $handler->m_node = $node;
    $handler->m_action = 'attributeedit';    

    $this->assertTrue(is_object($handler->m_node)); // The m_node variable should be set.
    $this->assertNull($handler->m_masterNode);      // The masternode is not set at default.

    $handler->setMasterNode($masterNode);
    $this->assertEqual($handler->m_node->m_type, 'somenode');
    $this->assertTrue(is_object($handler->m_masterNode));
    $this->assertEqual($handler->m_masterNode->m_type, 'masternode');
  }
}