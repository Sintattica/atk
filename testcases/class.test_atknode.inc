<?php
  /**
   * test_atknode class file
   *
   * @package atk
   * @subpackage testcases
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   *
   * @version $Revision: 5016 $
   * $Id$
   */

  /**
   * Testcases for atkNode
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk
   * @subpackage testcases
   */

   class test_atknode extends atkTestCase
   {
    function test_addremove()
    {
      // Load used classes
      atkimport("atk.atknode");
      atkimport("atk.attributes.atkattribute");

      // Create a node and initialize
      $node = new atkNode("testnode");
      $module = new atkModule("testmodule");
      $node->m_module = "testmodule";
      $node->init();

      // Add an attribute on a new tab and test its existance
      $node->add(new atkAttribute("nieuw"), "testtab");
      $this->assertTrue(array_key_exists("nieuw", $node->m_attribList), 'array_key_exists("nieuw", $node->m_attribList) should be true');
      $this->assertTrue(array_key_exists("nieuw", $node->m_attributeTabs), 'array_key_exists("nieuw", $node->m_attributeTabs) should be true');

      // Re-add an attribute on a new tab and test its existance
      $node->add(new atkAttribute("nieuw"), "testtab");
      $this->assertTrue(array_key_exists("nieuw", $node->m_attribList), 'array_key_exists("nieuw", $node->m_attribList) should be true');
      $this->assertTrue(array_key_exists("nieuw", $node->m_attributeTabs), 'array_key_exists("nieuw", $node->m_attributeTabs) should be true');

      // Remove the attribute and test its absense
      $node->remove("nieuw");
      $this->assertFalse(array_key_exists("nieuw", $node->m_attribList), 'array_key_exists("nieuw", $node->m_attribList) should be false');
      $this->assertFalse(array_key_exists("nieuw", $node->m_attributeTabs), 'array_key_exists("nieuw", $node->m_attributeTabs) should be false');


    }
  }
?>