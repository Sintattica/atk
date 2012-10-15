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
   * @version $Revision: 6065 $
   */

/** 
 * @internal includes
 */
atkimport('atk.atkmocknode');
atkimport('atk.handlers.atkattributeedithandler');

/**
 * atkAttributeEditHandler test-case.
 * @internal
 * @package atk
 * @subpackage testcases
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
