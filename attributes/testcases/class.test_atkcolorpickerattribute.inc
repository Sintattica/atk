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
 * Tests for the atkColorPickerAttribute.
 * @package atk
 * @subpackage testcases
 * @internal
 */
class test_atkcolorpickerattribute extends atkTestCase 
{
  /**
   * Test if setting and getting the colormode works
   *
   */
  public function test_colormode()
  {
    useattrib('atkcolorpickerattribute');
    $attr = new atkColorPickerAttribute('col');
    
    // initially the colormode should be default
    $this->assertNotNull($attr->getColorMode());
    $this->assertTrue(($attr->getColorMode() === CP_COLORMODE_DEFAULT));    
    $attr->setColorMode(CP_COLORMODE_HEX6);
    $this->assertTrue(($attr->getColorMode() === CP_COLORMODE_HEX6));
  }
  
  /**
   * Check the attribute in default mode.
   */
  public function test_colormode_default()
  {
    useattrib('atkcolorpickerattribute');
    $attr = new atkColorPickerAttribute('col');
    
    /**
     * Check some valid HEX values
     */
    $records = array();
    $records[] = array('col' => '12a1A3');
    $records[] = array('col' => '#123123');
    $records[] = array('col' => '#aaabbb');
    $records[] = array('col' => 'AAA');
    $records[] = array('col' => '#AAA');
    
    foreach ($records as $record)
    {
      $this->assertTrue($attr->check_color($record['col']),'color '.$record['col']);
    }
    
    /**
     * Check some valid rgb values
     */
    $records = array();
    $records[] = array('col' => 'rgb(12,12,12)');
    $records[] = array('col' => 'rgb(12,88,188)');
    
    foreach ($records as $record)
    {
      $this->assertTrue($attr->check_color($record['col'], 'rgb'),'color '.$record['col']);
    }
    
    $records = array();
    $records[] = array('col' => 'GGGGGG');
    $records[] = array('col' => '#GGGGGG');
    $records[] = array('col' => '#GGG');
    $records[] = array('col' => array());
    $records[] = array('col' => NULL);
    $records[] = array('col' =>array(21,'ff'));
    $records[] = array('col' =>array('#DD'));
    
    // Check some invalid values
    foreach ($records as $record)
    {
      $this->assertFalse($attr->check_color($record['col']),'color '.$record['col']);
    }
  }
  
  /**
   * Check the attribute in HEX6 color mode.
   */
  public function test_colormode_hex6()
  {
    useattrib('atkcolorpickerattribute');
    $attr = new atkColorPickerAttribute('col');
    $attr->setColorMode(CP_COLORMODE_HEX6);
    
    /**
     * Check some valid values
     */
    $records = array();
    $records[] = array('col' => '12a1A3');
    $records[] = array('col' => '#123123');
    $records[] = array('col' => '#aaabbb');    
    
    foreach ($records as $record)
    {
      $this->assertTrue($attr->check_color($record['col']));
    }
    
    /**
     * Check some invalid values
     */
    $records = array();
    $records[] = array('col' => 'AAA');   // hex3 values should fail in this mode
    $records[] = array('col' => '#AAA');  // hex3 values should fail in this mode  
    $records[] = array('col' => 'rgb(88,188)'); // rgb values should fail in this mode
    
    $records[] = array('col' => '#GGG');        
    $records[] = array('col' => 'GGGGGG');
    $records[] = array('col' => '#GGGGGG');
    $records[] = array('col' => array());
    $records[] = array('col' => NULL);
    $records[] = array('col' => 'rgb(12,88,#)');
    $records[] = array('col' =>array(21,'ff'));
    $records[] = array('col' =>array('#DD'));
    
    foreach ($records as $record)
    {
      $this->assertFalse($attr->check_color($record['col']),'color '.$record['col']);
    }
  }  
}
