<?php
/**
 * Tests for the atkBoolAttribute.
 */
class test_atkBoolAttribute extends atkTestCase 
{
  /**
   * Test if the attribute respects the inline label flag properly.
   */
  public function test_inline_label()
  {
    useattrib('atkboolattribute');
    $attr = new atkBoolAttribute('test');
    
    $html = $attr->edit(array(), '', 'edit');
    $this->assertTrue(strpos($html, '<label for') === false);
    $this->assertNotNull($attr->getLabel(array(), 'edit'));
    $this->assertNotNull($attr->getLabel(array(), 'view'));
    
    $attr->addFlag(AF_BOOL_INLINE_LABEL);
    $html = $attr->edit(array(), '', 'edit');
    $this->assertTrue(strpos($html, '<label for') !== false);    
    $this->assertNotNull($attr->getLabel(array(), 'edit'));   
    $this->assertNotNull($attr->getLabel(array(), 'view'));
    
    $attr->addFlag(AF_BLANK_LABEL);
    $html = $attr->edit(array(), '', 'edit');    
    $this->assertTrue(strpos($html, '<label for') !== false);    
    $this->assertNull($attr->getLabel(array(), 'edit'));
    $this->assertNotNull($attr->getLabel(array(), 'view'));  

    $attr->removeFlag(AF_BOOL_INLINE_LABEL);
    $html = $attr->edit(array(), '', 'edit');    
    $this->assertTrue(strpos($html, '<label for') === false);        
    $this->assertNull($attr->getLabel(array(), 'edit'));
    $this->assertNull($attr->getLabel(array(), 'view'));    

    $attr = new atkBoolAttribute('test', AF_BLANK_LABEL|AF_BOOL_INLINE_LABEL);
    $html = $attr->edit(array(), '', 'edit');    
    $this->assertTrue(strpos($html, '<label for') !== false);    
    $this->assertNull($attr->getLabel(array(), 'edit'));
    $this->assertNotNull($attr->getLabel(array(), 'view'));      
  }
}