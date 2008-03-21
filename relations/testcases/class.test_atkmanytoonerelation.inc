<?php
/**
 * atkManyToOneRelation test-case.
 */
class test_atkManyToOneRelation extends atkTestCase 
{
  /**
   * Tests the fixDestinationRecord method of the atkManyToOneRelation which
   * adds support to the atkManyToOneRelation for destination records which are
   * not an array yet, but have the primary key value directly set.
   */
  public function test_fix_destination_record()
  {
    $attr = new atkManyToOneRelation('test', '');
    $attr->m_destInstance = new atkNode();
    $attr->m_destInstance->add(new atkNumberAttribute('id', AF_PRIMARY));

    // first test what happens if the destination record is not a record yet
    // but does contain a primary key value
    $record = array('test' => 1234);
    $attr->fixDestinationRecord($record);
    $this->assertTrue(is_array($record['test']));
    $this->assertNotNull($record['test']['id']);
    $this->assertEqual(1234, $record['test']['id']);
    
    // check if our destination record is left unchanged if 
    // it's already an array
    $record = array('test' => array('id' => 1234));
    $attr->fixDestinationRecord($record);
    $this->assertTrue(is_array($record['test']));
    $this->assertNotNull($record['test']['id']);
    $this->assertEqual(1234, $record['test']['id']);

    // check if null values are not converted to an array
    $record = array('test' => null);
    $attr->fixDestinationRecord($record);
    $this->assertFalse(is_array($record['test']));
    $this->assertNull($record['test']);
  }
  
  /**
   * Tests the getOrderByStatement method of the atkManyToOneRelation 
   */
  public function test_get_order_by_statement()
  {
    $attr = new atkManyToOneRelation('test', '');
    $attr->m_destInstance = new atkNode();
    $attr->m_destInstance->add(new atkNumberAttribute('id', AF_PRIMARY));
    $attr->m_destInstance->add(new atkNumberAttribute('sortorder', AF_OBLIGATORY));
    $attr->m_destInstance->add(new atkAttribute('name', AF_PRIMARY));
    
    $this->assertEqual($attr->getOrderByStatement(), "test.id");
    $this->assertEqual($attr->getOrderByStatement(null), "test.id");
    $this->assertEqual($attr->getOrderByStatement("id"), "test.id");
    $this->assertEqual($attr->getOrderByStatement("doesntexists"), "test.id");
    $this->assertEqual($attr->getOrderByStatement("name"), "test.id");
    
    $attr->m_destInstance->setOrder("name");
    $this->assertEqual($attr->getOrderByStatement(), "test.name");
    
    $attr->m_destInstance->setOrder("sortorder desc");
    $this->assertEqual($attr->getOrderByStatement(), "test.sortorder desc");
    
    $attr->m_destInstance->setOrder("sortorder, name");
    $this->assertEqual($attr->getOrderByStatement(), "test.sortorder, test.name");
    
    $attr->m_destInstance->setOrder("sortorder asc , name desc");
    $this->assertEqual($attr->getOrderByStatement(), "test.sortorder asc , test.name desc");
  }
}