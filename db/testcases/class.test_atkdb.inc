<?php

/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage db
 * @access private
 *
 * @copyright (c)2005 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 5804 $
 * $Id$
 */

/**
 * Include the class we are about to test.
 */
atkimport("atk.db.atkdb");
atkimport("atk.db.atkmysqldb");

/**
 * Testcases for db abstraction
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage db
 * @access private
 */
class test_atkdb extends UnitTestCase
{
  private $m_mockDb;
  
  /**
   * Setup.
   */
  public function setUp()
  {
    atkimport('atk.db.atkmockdb');
    $this->m_mockDb = new atkMockDb();    
  }
  
  /**
   * Returns the mock database.
   */
  private function getMockDb()
  {
    return $this->m_mockDb;
  }
  
  /**
   * Test if the data retrieval methods like getRows etc. work properly.
   */
  public function test_data_retrieval()
  {
    $query = "SELECT x, y, z FROM table";
    
    $rows = array(
      array('id' => 1, 'nl' => 'aap', 'en' => 'monkey'), 
      array('id' => 2, 'nl' => 'noot', 'en' => 'nut'),
      array('id' => 3, 'nl' => 'mies', 'en' => 'mies'),
    );

    $this->getMockDb()->setResult($rows, $query); // no limit
    $this->getMockDb()->setResult(array($rows[2]), $query, 0, 1); // when an explicit limit of 0,1 is given we use the 3rd row!    
    
    $this->assertEqual($this->getMockDb()->getRows($query), $rows);
    $this->assertEqual($this->getMockDb()->getRow($query, true), $rows[2]); // explicit limit
    $this->assertEqual($this->getMockDb()->getRow($query, false), $rows[0]); // no limit
    $this->assertEqual($this->getMockDb()->getValue($query), 3); // explicit limit
    $this->assertEqual($this->getMockDb()->getValue($query, null, 1), 'mies'); // explicit limit
    $this->assertEqual($this->getMockDb()->getValue($query, null, 1, false), 'aap'); // no limit
    $this->assertEqual($this->getMockDb()->getValue($query, null, 'en'), 'mies'); // explicit limit
    $this->assertEqual($this->getMockDb()->getValue($query, null, 'en', false), 'monkey'); // no limit    
    $this->assertEqual($this->getMockDb()->getValue($query, null, 0, false), 1); // no limit
    $this->assertEqual($this->getMockDb()->getValues($query), array(1, 2, 3));
    $this->assertEqual($this->getMockDb()->getValues($query, 'nl'), array('aap', 'noot', 'mies'));
    $this->assertEqual($this->getMockDb()->getValues($query, 2), array('monkey', 'nut', 'mies'));
    $this->assertEqual($this->getMockDb()->getValuesAssoc($query), array(1 => 'aap', 2 => 'noot', 3 => 'mies'));
    $this->assertEqual($this->getMockDb()->getValuesAssoc($query, 'nl', 'en'), array('aap' => 'monkey', 'noot' => 'nut', 'mies' => 'mies'));
    $this->assertEqual($this->getMockDb()->getValuesAssoc($query, 2, 'nl'), array('monkey' => 'aap', 'nut' => 'noot', 'mies' => 'mies'));
    $this->assertEqual($this->getMockDb()->getValuesAssoc($query, 0, 1), array(1 => 'aap', 2 => 'noot', 3 => 'mies'));    
    
    $rowsAssoc = array(
      1 => array('id' => 1, 'nl' => 'aap', 'en' => 'monkey'), 
      2 => array('id' => 2, 'nl' => 'noot', 'en' => 'nut'),
      3 => array('id' => 3, 'nl' => 'mies', 'en' => 'mies')
    );
    
    $this->assertEqual($this->getMockDb()->getRowsAssoc($query), $rowsAssoc);
    $this->assertEqual($this->getMockDb()->getRowsAssoc($query, 0), $rowsAssoc);
    $this->assertNotEqual($this->getMockDb()->getRowsAssoc($query, 1), $rowsAssoc);    
    $this->assertNotEqual($this->getMockDb()->getRowsAssoc($query, 2), $rowsAssoc);    
    $this->assertEqual($this->getMockDb()->getRowsAssoc($query, 'id'), $rowsAssoc);
    $this->assertNotEqual($this->getMockDb()->getRowsAssoc($query, 'nl'), $rowsAssoc);
    
    $rowsAssoc = array(
      'aap'  => array('id' => 1, 'nl' => 'aap', 'en' => 'monkey'), 
      'noot' => array('id' => 2, 'nl' => 'noot', 'en' => 'nut'),
      'mies' => array('id' => 3, 'nl' => 'mies', 'en' => 'mies')
    );
    
    $this->assertNotEqual($this->getMockDb()->getRowsAssoc($query), $rowsAssoc);
    $this->assertNotEqual($this->getMockDb()->getRowsAssoc($query, 0), $rowsAssoc);    
    $this->assertEqual($this->getMockDb()->getRowsAssoc($query, 1), $rowsAssoc);
    $this->assertNotEqual($this->getMockDb()->getRowsAssoc($query, 2), $rowsAssoc);    
    $this->assertNotEqual($this->getMockDb()->getRowsAssoc($query, 'id'), $rowsAssoc);    
    $this->assertEqual($this->getMockDb()->getRowsAssoc($query, 'nl'), $rowsAssoc);    
    
    $this->getMockDb()->setResult(array(), $query); // no limit
    $this->getMockDb()->setResult(array(), $query, 0, 1); // when an explicit limit of 0,1 is given we use the 3rd row!
    $this->assertEqual($this->getMockDb()->getValue($query, 'x'), 'x');
    $this->assertEqual($this->getMockDb()->getValue($query, 'x', 0, false), 'x');

    $this->getMockDb()->setResult(array(array('y' => 'y')), $query); // no limit
    $this->getMockDb()->setResult(array(array('z' => 'z')), $query, 0, 1); // when an explicit limit of 0,1 is given we use the 3rd row!
    $this->assertEqual($this->getMockDb()->getValue($query, 'x'), 'z');
    $this->assertEqual($this->getMockDb()->getValue($query, 'x', 0, false), 'y');
  }
}
?>