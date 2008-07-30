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
   * @copyright (c)2008 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   */

/** @internal includes */
atkimport('atk.atkdatanode');

/**
 * Tests for the ATK data node.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage testcases
 */
class test_atkdatanode extends atkTestCase
{
  private $m_data = array(
    array('id' => 12, 'name' => 'aap',  'type' => 'animal'),
    array('id' =>  9, 'name' => 'noot', 'type' => 'object'),
    array('id' => 83, 'name' => 'mies', 'type' => 'animal'),
    array('id' => 42, 'name' => 'wim',  'type' => 'person'),
    array('id' => 26, 'name' => 'zus',  'type' => 'person'),
    array('id' => 58, 'name' => 'jet',  'type' => 'person')
  );

  private $m_node = null;

  /**
   * Setup.
   */
  public function setUp()
  {
    useAttrib("atknumberattribute");
    $this->m_node = new atkDataNode();
    $this->m_node->setTable('fake');
    $this->m_node->setOrder('name');
    $this->m_node->add(new atkNumberAttribute('id', AF_PRIMARY));
    $this->m_node->add(new atkAttribute('name', AF_OBLIGATORY));
    $this->m_node->add(new atkAttribute('type'));
    $this->m_node->setData($this->m_data);
  }

  /**
   * Tear down.
   */
  public function tearDown()
  {
    $this->m_node = null;
  }

  /**
   * Test the constructor.
   */
  public function test_constructor()
  {
    $node = new atkDataNode('test', NF_ADD_LINK);
    $this->assertNotNull($node);
    $this->assertEqual('test', $node->m_type);
    $this->assertTrue($node->hasFlag(NF_ADD_LINK));

    $node = new atkDataNode();
    $this->assertNotNull($node);
    $this->assertEqual('atkdatanode', $node->m_type);
  }

  /**
   * Test if no parameters and no default order is set if
   * we are given all data back unchanged.
   */
  public function test_no_params()
  {
    $this->m_node->setOrder(null);
    $this->assertEqual(count($this->m_data), $this->m_node->countDb());
    $this->assertTrue($this->m_data === $this->m_node->selectDb());
  }

  /**
   * Test selector.
   */
  public function test_selector()
  {
    $result = $this->m_node->selectDb("name = 'noot'");
    $this->assertEqual(1, count($result));
    $this->assertEqual('noot', $result[0]['name']);
    $result = $this->m_node->selectDb($this->m_node->getTable().".id=26");
    $this->assertEqual(1, count($result));
    $this->assertEqual('zus', $result[0]['name']);
    $result = $this->m_node->countDb("id=26 AND name='noot'");
    $this->assertEqual(0, $result);
    $result = $this->m_node->selectDb("id=26 AND name='zus'");
    $this->assertEqual(1, count($result));
    $this->assertEqual('zus', $result[0]['name']);
    $result = $this->m_node->selectDb("type ='person'");
    $this->assertEqual(3, count($result));
    $this->assertEqual('person', $result[0]['type']);
  }

  /**
   * Test what happens if the data consists of more columns then
   * we have defined attributes for.
   */
  public function test_columns_filter()
  {
    $row = $this->m_node->select()->firstRow();
    $this->assertNotNull($row);
    $this->assertTrue(isset($row['name']));
    $this->m_node->remove('name');
    $row = $this->m_node->select()->firstRow();
    $this->assertNotNull($row);
    $this->assertFalse(isset($row['name']));
  }

  /**
   * Test default order.
   */
  public function test_default_order()
  {
    $result = $this->m_node->selectDb();
    $this->assertEqual(count($this->m_data), count($result));
    $this->assertEqual('aap', $result[0]['name']);
    $this->assertEqual('jet', $result[1]['name']);
    $this->assertEqual('zus', $result[count($result) - 1]['name']);
  }

  /**
   * Test descending order.
   */
  public function test_desc_order()
  {
    $this->m_node->setOrder('name DESC');
    $result = $this->m_node->selectDb();
    $this->assertEqual('zus', $result[0]['name']);
    $this->assertEqual('wim', $result[1]['name']);
    $this->assertEqual('aap', $result[count($result) - 1]['name']);
  }

  /**
   * Test order override.
   */
  public function test_order_override()
  {
    $result = $this->m_node->selectDb(null, 'id asc');
    $this->assertEqual('noot', $result[0]['name']);
    $this->assertEqual(9, $result[0]['id']);
    $this->assertEqual(83, $result[count($result) - 1]['id']);
  }

  /**
   * Test invalid column order.
   * (Elements should not be ordered.)
   */
  public function test_invalid_column_order()
  {
    $result = $this->m_node->selectDb(null, 'invalid_column');
    $this->assertEqual(count($this->m_data), count($result));
    $this->assertTrue($this->m_data === $result);
  }

  /**
   * Test order using table name.
   */
  public function test_order_with_table_name()
  {
    $this->m_node->setOrder($this->m_node->getTable().'.name desc');
    $result = $this->m_node->selectDb();
    $this->assertEqual(count($this->m_data), count($result));
    $this->assertEqual('zus', $result[0]['name']);
    $this->assertEqual('wim', $result[1]['name']);
    $this->assertEqual('aap', $result[count($result) - 1]['name']);
  }

  /**
   * Test order using an invalid table name.
   * (Elements should not be ordered.)
   */
  public function test_order_with_invalid_table_name()
  {
    $result = $this->m_node->selectDb(null, 'invalid.name');
    $this->assertEqual(count($this->m_data), count($result));
    $this->assertTrue($this->m_data === $result);
  }

  /**
   * Test case insensitivy of order.
   */
  public function test_order_case_insensitive()
  {
    $data = $this->m_data;
    $data[5]['name'] = 'JET';
    $this->m_node->setData($data);
    $result = $this->m_node->selectDb();
    $this->assertEqual(count($this->m_data), count($result));
    $this->assertEqual('aap', $result[0]['name']);
    $this->assertEqual('JET', $result[1]['name']);
    $this->assertEqual('zus', $result[count($result) - 1]['name']);
  }

  /**
   * Test if it makes a difference if a column uses a number attribute or not.
   */
  public function test_numeric_order()
  {
    $result = $this->m_node->selectDb(null, 'id');
    $this->assertEqual('noot', $result[0]['name']);
    $this->assertEqual(9, $result[0]['id']);
    $this->m_node->add(new atkAttribute('id', AF_PRIMARY));
    $result = $this->m_node->selectDb(null, 'id');
    $this->assertEqual('aap', $result[0]['name']);
    $this->assertEqual(12, $result[0]['id']);
    $this->assertEqual('noot', $result[count($result) - 1]['name']);
    $this->assertEqual(9, $result[count($result) - 1]['id']);
  }

  /**
   * Test limit.
   */
  public function test_limit()
  {
    $this->m_node->setOrder(null);
    $result = $this->m_node->selectDb(null, null, array('limit' => 2));
    $this->assertEqual(2, count($result));
    $this->assertEqual($this->m_data[0]['name'], $result[0]['name']);
    $this->assertEqual($this->m_data[1]['name'], $result[1]['name']);
    $result = $this->m_node->selectDb(null, null, array('limit' => 2, 'offset' => 2));
    $this->assertEqual(2, count($result));
    $this->assertEqual($this->m_data[2]['name'], $result[0]['name']);
    $this->assertEqual($this->m_data[3]['name'], $result[1]['name']);
    $result = $this->m_node->selectDb(null, null, array('offset' => 3));
    $this->assertEqual(3, count($result));
    $this->assertEqual($this->m_data[3]['name'], $result[0]['name']);
    $this->assertEqual($this->m_data[4]['name'], $result[1]['name']);
    $result = $this->m_node->selectDb(null, null, array('offset' => 7));
    $this->assertEqual(0, count($result));
  }

  /**
   * Test combined parameters.
   */
  public function test_combined_params()
  {
    $result = $this->m_node->countDb("type='person'");
    $this->assertEqual(3, $result);
    $result = $this->m_node->selectDb("type='person'", "name desc", array('limit' => 2));
    $this->assertEqual(2, count($result));
    $this->assertEqual('zus', $result[0]['name']);
    $this->assertEqual('wim', $result[1]['name']);
    $result = $this->m_node->countDb("type='animal'");
    $this->assertEqual(2, $result);
    $result = $this->m_node->selectDb("type='animal'", 'name', array('limit' => 1, 'offset' => 1));
    $this->assertEqual(1, count($result));
    $this->assertEqual('mies', $result[0]['name']);
  }
}
