<?php

/**
 * atkRelation test-case.
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
    $expectedResult = "SELECT foo\n FROM bar\n WHERE foo = \"  1  \"";
    $attr = new atkRelation('test2', '');
    $attr->setDestinationFilter($testFilter);
    $result = $attr->getDestinationFilter();
    $this->assertEqual($result, $expectedResult, 'Filter with need for cleanup');

    // Test a filter with redendant spaces and spaces between single quotes
    $testFilter     = "SELECT foo
                       FROM bar
                       WHERE foo = 'v  a  l  u  e'";
    $expectedResult = "SELECT foo\n FROM bar\n WHERE foo = 'v  a  l  u  e'";
    $attr = new atkRelation('test2', '');
    $attr->setDestinationFilter($testFilter);
    $result = $attr->getDestinationFilter();
    $this->assertEqual($result, $expectedResult, 'Filter with need for cleanup');

   }
}