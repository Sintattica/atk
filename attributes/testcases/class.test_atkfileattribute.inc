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
 * Tests for the atkFileAttribute.
 * @internal
 * @package atk
 * @subpackage testcases
 */
class test_atkFileAttribute extends atkTestCase 
{
  /**
   * Test if the attribute respects the allowed file types (by extension).
   */
  public function test_allowed_file_types_by_extension()
  {
    useattrib('atkfileattribute');
    $attr = new atkFileAttribute('test', atkconfig('atkroot').'atk/themes/default/images/');
    $attr->setAllowedFileTypes(array('jpg', 'gif', 'png'));  
    
    $record = array($attr->fieldName() => array('filename' => 'arrow.gif', 'orgfilename' => 'arrow.gif'));
    $this->assertNull(@$record[$attr->fieldName()]['error']);
    $this->assertTrue($attr->isAllowedFileType($record));
    $this->assertNull(@$record[$attr->fieldName()]['error']);    
    
    $record = array($attr->fieldName() => array('filename' => 'test.php', 'orgfilename' => 'test.php'));
    $this->assertNull(@$record[$attr->fieldName()]['error']);
    $this->assertFalse($attr->isAllowedFileType($record));
    $this->assertEqual(UPLOAD_ERR_EXTENSION, $record[$attr->fieldName()]['error']);
    
    $attr->setAllowedFileTypes(array('jpg', 'gif', 'png', 'php'));     

    $record = array($attr->fieldName() => array('filename' => 'test.php', 'orgfilename' => 'test.php'));
    $this->assertNull(@$record[$attr->fieldName()]['error']);
    $this->assertTrue($attr->isAllowedFileType($record));
    $this->assertNull(@$record[$attr->fieldName()]['error']);    
  }
  
  /**
   * Test if the attribute respects the allowed file types (by mime-type).
   */
  public function test_allowed_file_types_by_mime_type()
  {
    if (!function_exists('getimagesize'))
    {
      atkerror('Cannot run '.__METHOD__.', method getimagesize not supported by this PHP version!');
      return;
    }
    
    $attr = new atkFileAttribute('test', atkconfig('atkroot').'atk/themes/steelblue/images/');
    $attr->setAllowedFileTypes(array('image/jpeg'));  
    
    $record = array($attr->fieldName() => array('filename' => 'contentfooterleft.jpg', 'orgfilename' => 'contentfooterleft.jpg'));
    $this->assertNull(@$record[$attr->fieldName()]['error']);
    $this->assertTrue($attr->isAllowedFileType($record));
    $this->assertNull(@$record[$attr->fieldName()]['error']);    
    
    $record = array($attr->fieldName() => array('filename' => 'menubg.gif', 'orgfilename' => 'menubg.gif'));
    $this->assertNull(@$record[$attr->fieldName()]['error']);
    $this->assertFalse($attr->isAllowedFileType($record));
    $this->assertEqual(UPLOAD_ERR_EXTENSION, $record[$attr->fieldName()]['error']);    
    
    $attr->setAllowedFileTypes(array('image/jpeg', 'image/gif'));     
    $record = array($attr->fieldName() => array('filename' => 'menubg.gif', 'orgfilename' => 'menubg.gif'));
    $this->assertNull(@$record[$attr->fieldName()]['error']);
    $this->assertTrue($attr->isAllowedFileType($record));
    $this->assertNull(@$record[$attr->fieldName()]['error']);          
  }
}
