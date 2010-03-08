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

    function test_validateFilter()
    {
      // Load used classes
      atkimport("atk.atknode");
      atkimport("atk.attributes.atkattribute");

      // Create a node and initialize
      $node = new atkNode("testnode");
      $module = new atkModule("testmodule");
      $node->m_module = "testmodule";
      $node->m_table = 'test_table';
      $node->m_attribList = array(
        'foo' => null,
        'bar' => null,
        'testing' => null,
      );

      $validFilters = array(
        '',
        array_shift(array_keys($node->m_attribList)) . " = 'barr'",
        "{$node->m_table}." . array_shift(array_keys($node->m_attribList)) . " = 'bar'",
        "`{$node->m_table}`.`" . array_pop(array_keys($node->m_attribList)) . "` = 'bar'",
        array_shift(array_keys($node->m_attribList)) . " IN (" .
          "SELECT distinct(`{$node->m_table}`.`" . array_pop(array_keys($node->m_attribList)) . "`) " .
            "FROM {$node->m_table} " .
            "WHERE `" . array_pop(array_keys($node->m_attribList)) . "` IN (2, 3, 4)" .
          ")",
      );

      $invalidFilters = array(
        'some invalid string here',
        'aaaaa.aaaaa',
        "NOT_IN_LIST = 'anything'",
        "moo.cow = 'bar'",
        "`moo`.`cow` = 'bar'",
        "`NOT_IN_LIST` IN (" .
          "SELECT distinct(`{$node->m_table}`.`" . array_pop(array_keys($node->m_attribList)) . "`) " .
            "FROM {$node->m_table} " .
            "WHERE `" . array_pop(array_keys($node->m_attribList)) . "` IN (2, 3, 4)" .
          ")",
      );

      foreach($validFilters as $filter)
      {
        $this->assertEqual($node->validateFilter($filter), $filter, "Filter '$filter' should be valid. Result: %s");
      }

      foreach($invalidFilters as $filter)
      {
        $this->assertNotEqual($node->validateFilter($filter), $filter, "Filter '$filter' should be invalid. Result: %s");
      }
    }
   }
?>
