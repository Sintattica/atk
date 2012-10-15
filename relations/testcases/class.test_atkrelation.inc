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
   * @version $Revision: 6065 $
   */

/**
 * atkRelation test-case.
 * @internal
 * @package atk
 * @subpackage testcases
 */
class test_atkRelation extends atkTestCase
{
  /**
   * Tests the fixDestinationRecord method of the atkManyToOneRelation which
   * adds support to the atkManyToOneRelation for destination records which are
   * not an array yet, but have the primary key value directly set.
   */
  public function test_filter_cleanup()
  {
     // Test a normal string with extra spaces between quotes
    $testFilter     = 'SELECT foo FROM bar WHERE foo = "  1  "';
    $expectedResult = $testFilter;
    $attr = new atkRelation('test', '');
    $attr->setDestinationFilter($testFilter);
    $result = $attr->getDestinationFilter();
    $this->assertEqual($result, $expectedResult, 'Filter without need for cleanup');

    // Test a filter with redendant spaces and spaces between double quotes
    $testFilter     = 'SELECT foo
                       FROM bar
                       WHERE     foo = "  1  "';
    $expectedResult = "SELECT foo FROM bar WHERE foo = \"  1  \"";
    $attr = new atkRelation('test2', '');
    $attr->setDestinationFilter($testFilter);
    $result = $attr->getDestinationFilter();
    $this->assertEqual($result, $expectedResult, 'Filter with need for cleanup');

    // Test a filter with redendant spaces and spaces between single quotes
    $testFilter     = "SELECT foo
                       FROM bar
                       WHERE foo = 'v  a  l  u  e'";
    $expectedResult = "SELECT foo FROM bar WHERE foo = 'v  a  l  u  e'";
    $attr = new atkRelation('test2', '');
    $attr->setDestinationFilter($testFilter);
    $result = $attr->getDestinationFilter();
    $this->assertEqual($result, $expectedResult, 'Filter with need for cleanup');
    
        
    $testFilter = "person.id != '[id]' AND
                    EXISTS( SELECT 1
                             FROM `participation` p
                            JOIN corps_participation cp on p.corps_participation_id = cp.id
                            WHERE p.person_id = person.id
                             AND cp.participation_type_id = '      3')";
    
    $expectedResult = "person.id != '[id]' AND EXISTS( SELECT 1 FROM `participation` p JOIN corps_participation cp on p.corps_participation_id = cp.id WHERE p.person_id = person.id AND cp.participation_type_id = '      3')";
    $attr = new atkRelation('test2', '');
    $attr->setDestinationFilter($testFilter);
    $result = $attr->getDestinationFilter();
    
    $this->assertEqual($result, $expectedResult, 'Filter with multiple quoting');

   }
}
