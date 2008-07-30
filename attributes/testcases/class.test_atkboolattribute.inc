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
   * @copyright (c)2000-2008 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   */

/**
 * Tests for the atkBoolAttribute.
 * @internal
 * @package atk
 * @subpackage testcases
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
