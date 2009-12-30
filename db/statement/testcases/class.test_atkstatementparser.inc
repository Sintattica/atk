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
atkimport("atk.db.statement.atkstatementparser");

/**
 * Testcases for statement parser.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage db.statement.testcases
 * 
 * @access private
 */
class test_atkStatementParser extends UnitTestCase
{
  /**
   * Test the statement parser.
   */
  public function test_parser()
  {
    $parser = new atkStatementParser("SELECT * FROM dummy");
    $this->assertEqual("SELECT * FROM dummy", $parser->getParsedQuery());
    $this->assertEqual(0, count($parser->getBindPositions()));
    
    $parser = new atkStatementParser("SELECT * FROM dummy WHERE id = ?");
    $this->assertEqual("SELECT * FROM dummy WHERE id = ?", $parser->getParsedQuery());
    $this->assertEqual(array(31 => 0), $parser->getBindPositions());
    
    $parser = new atkStatementParser("SELECT * FROM dummy WHERE id = :id");
    $this->assertEqual("SELECT * FROM dummy WHERE id = ?", $parser->getParsedQuery());
    $this->assertEqual(array(31 => 'id'), $parser->getBindPositions());
    
    $parser = new atkStatementParser("SELECT * FROM dummy WHERE id = '?'");
    $this->assertEqual("SELECT * FROM dummy WHERE id = '?'", $parser->getParsedQuery());
    $this->assertEqual(array(), $parser->getBindPositions());
    
    $parser = new atkStatementParser("SELECT * FROM dummy WHERE id = ':id'");
    $this->assertEqual("SELECT * FROM dummy WHERE id = ':id'", $parser->getParsedQuery());
    $this->assertEqual(array(), $parser->getBindPositions());
    
    $parser = new atkStatementParser("SELECT * FROM dummy WHERE x = 'x' AND y = ? AND z = 'z'");
    $this->assertEqual("SELECT * FROM dummy WHERE x = 'x' AND y = ? AND z = 'z'", $parser->getParsedQuery());
    $this->assertEqual(array(42 => 0), $parser->getBindPositions());
    
    $parser = new atkStatementParser("SELECT * FROM dummy WHERE x = 'x' AND y = :y AND z = 'z'");
    $this->assertEqual("SELECT * FROM dummy WHERE x = 'x' AND y = ? AND z = 'z'", $parser->getParsedQuery());
    $this->assertEqual(array(42 => 'y'), $parser->getBindPositions());
    
    $parser = new atkStatementParser("SELECT * FROM dummy WHERE x = ? AND y = ?");
    $this->assertEqual("SELECT * FROM dummy WHERE x = ? AND y = ?", $parser->getParsedQuery());
    $this->assertEqual(array(30 => 0, 40 => 1), $parser->getBindPositions());
    
    $parser = new atkStatementParser("SELECT * FROM dummy WHERE x = :x AND y = :y");
    $this->assertEqual("SELECT * FROM dummy WHERE x = ? AND y = ?", $parser->getParsedQuery());
    $this->assertEqual(array(30 => 'x', 40 => 'y'), $parser->getBindPositions());    

    $parser = new atkStatementParser("SELECT * FROM dummy WHERE x = :x AND y = :x");
    $this->assertEqual("SELECT * FROM dummy WHERE x = ? AND y = ?", $parser->getParsedQuery());
    $this->assertEqual(array(30 => 'x', 40 => 'x'), $parser->getBindPositions());    
    
    $parser = new atkStatementParser("SELECT * FROM dummy\nWHERE x = :x AND y = :x");
    $this->assertEqual("SELECT * FROM dummy\nWHERE x = ? AND y = ?", $parser->getParsedQuery());
    $this->assertEqual(array(30 => 'x', 40 => 'x'), $parser->getBindPositions()); 

    $parser = new atkStatementParser("SELECT * FROM dummy WHERE x = :x AND y = ? AND z = ? AND\n`xyz` = :zyx AND abc = '''\n?'''");
    $this->assertEqual("SELECT * FROM dummy WHERE x = ? AND y = ? AND z = ? AND\n`xyz` = ? AND abc = '''\n?'''", $parser->getParsedQuery());
    $this->assertEqual(array(30 => 'x', 40 => 0, 50 => 1, 64 => 'zyx'), $parser->getBindPositions());       
  }
}