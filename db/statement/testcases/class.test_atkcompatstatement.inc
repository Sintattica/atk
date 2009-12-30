<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage db.statement
 *
 * @copyright (c) 2009 Peter C. Verhage
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */
atkimport("atk.db.statement.atkcompatstatement");
atkimport("atk.db.atkmockdb");

/**
 * Testcases for statement parser.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage db.statement.testcases
 * 
 * @access private
 */
class test_atkCompatStatement extends UnitTestCase
{
  /**
   * Test if bind parameters are replaced properly.
   */
  public function test_binds()
  {
    $db = new atkMockDb();
    $stmt = new atkCompatStatement($db, "SELECT * FROM dummy");
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt->execute();
    $history = $db->getQueryHistory();
    $this->assertEqual(1, count($history));
    $this->assertEqual("SELECT * FROM dummy", $history[0]);
    
    $db->clearQueryHistory();
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt = new atkCompatStatement($db, "SELECT * FROM dummy WHERE id = ?");
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt->execute(array(13));
    $history = $db->getQueryHistory();
    $this->assertEqual(1, count($history));
    $this->assertEqual("SELECT * FROM dummy WHERE id = '13'", $history[0]);
    
    $db->clearQueryHistory();
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt = new atkCompatStatement($db, "SELECT * FROM dummy WHERE id = :id");
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt->execute(array('id' => 13));
    $history = $db->getQueryHistory();
    $this->assertEqual(1, count($history));
    $this->assertEqual("SELECT * FROM dummy WHERE id = '13'", $history[0]);

    $db->clearQueryHistory();
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt = new atkCompatStatement($db, "SELECT * FROM dummy WHERE id = :id");
    $this->assertEqual(0, count($db->getQueryHistory()));
    $exception = null;
    try { $stmt->execute(array(13)); }
    catch (Exception $ex) { $exception = $ex; }
    $this->assertTrue($exception instanceof atkStatementException);
    $this->assertEqual(0, count($db->getQueryHistory()));
    
    $db->clearQueryHistory();
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt = new atkCompatStatement($db, "SELECT * FROM dummy WHERE name = ?");
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt->execute(array(0 => "Teal'c"));
    $history = $db->getQueryHistory();
    $this->assertEqual(1, count($history));
    $this->assertEqual("SELECT * FROM dummy WHERE name = 'Teal''c'", $history[0]);
    
    $db->clearQueryHistory();
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt = new atkCompatStatement($db, "SELECT * FROM dummy WHERE name = :name");
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt->execute(array('name' => "Teal'c"));
    $history = $db->getQueryHistory();
    $this->assertEqual(1, count($history));
    $this->assertEqual("SELECT * FROM dummy WHERE name = 'Teal''c'", $history[0]);    
    
    $db->clearQueryHistory();
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt = new atkCompatStatement($db, "SELECT * FROM dummy WHERE name = ':name'");
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt->execute(array('name' => "Teal'c"));
    $history = $db->getQueryHistory();
    $this->assertEqual(1, count($history));
    $this->assertEqual("SELECT * FROM dummy WHERE name = ':name'", $history[0]); 

    $db->clearQueryHistory();
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt = new atkCompatStatement($db, "SELECT * FROM dummy WHERE name = :name");
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt->execute(array('name' => null));
    $history = $db->getQueryHistory();
    $this->assertEqual(1, count($history));
    $this->assertEqual("SELECT * FROM dummy WHERE name = NULL", $history[0]);      

    $db->clearQueryHistory();
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt = new atkCompatStatement($db, "SELECT * FROM dummy WHERE x = :x AND y = ? AND z = ? AND\n`xyz` = :x AND abc = '''\n?'''");
    $this->assertEqual(0, count($db->getQueryHistory()));
    $stmt->execute(array('x' => 'X', 0 => 'Y', 1 => 'Z'));
    $history = $db->getQueryHistory();
    $this->assertEqual(1, count($history));
    $this->assertEqual("SELECT * FROM dummy WHERE x = 'X' AND y = 'Y' AND z = 'Z' AND\n`xyz` = 'X' AND abc = '''\n?'''", $history[0]);      
  }
  
  /**
   * Test if we get an instance of atkCompatStatement for the mock db because it doesn't
   * have it's own implementation.
   */
  public function test_fallback()
  {
    $db = new atkMockDb();
    $stmt = $db->prepare("SELECT * FROM dummy");
    $this->assertTrue($stmt instanceof atkCompatStatement);
  }
}