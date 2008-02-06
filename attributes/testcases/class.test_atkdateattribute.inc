<?php
useattrib('atkdateattribute');

class DateAttributeWrapper extends atkDateAttribute 
{
  public static function resetBaseScriptsRegistered()
  {
    self::$s_baseScriptsRegistered = false;
  }
}

/**
 * Tests for the atkDateAttribute.
 */
class test_atkDateAttribute extends atkTestCase 
{
  /**
   * Constructor.
   */
  public function __construct()
  {
    // don't use test database
    $this->setUseTestDatabase(false);
  }
  
  /**
   * Test if scripts and styles are properly registered.
   */
  public function test_register_scripts_and_styles()
  {
    atkimport('atk.ui.atkpage');
    $page = atkPage::getInstance();
    
    $orgScriptFiles = $page->m_scriptfiles;
    $orgScriptCode = $page->m_scriptcode;
    $orgStylesheets = $page->m_stylesheets;

    $attr = new atkDateAttribute('xyz');
    
    // normal edit with calendar
    DateAttributeWrapper::resetBaseScriptsRegistered();
    $page->m_scriptfiles = array();
    $page->m_scriptcode = array('before' => array(), 'after' => array());
    $page->m_stylesheets = array();
    $attr->edit(array(), '', 'edit');
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(4, count($page->m_scriptfiles));
    $this->assertEqual(1, count($page->m_stylesheets));

    // second call
    $attr->edit(array(), '', 'edit');    
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(4, count($page->m_scriptfiles));
    $this->assertEqual(1, count($page->m_stylesheets));
    
    // edit without calendar
    DateAttributeWrapper::resetBaseScriptsRegistered();
    $page->m_scriptfiles = array();
    $page->m_scriptcode = array('before' => array(), 'after' => array());
    $page->m_stylesheets = array();
    $attr->addFlag(AF_DATE_NO_CALENDAR);
    $attr->edit(array(), '', 'edit');
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(1, count($page->m_scriptfiles));
    $this->assertEqual(0, count($page->m_stylesheets));    

    // second call
    DateAttributeWrapper::resetBaseScriptsRegistered();
    $page->m_scriptfiles = array();
    $page->m_scriptcode = array('before' => array(), 'after' => array());
    $page->m_stylesheets = array();
    $attr->edit(array(), '', 'edit');
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(1, count($page->m_scriptfiles));
    $this->assertEqual(0, count($page->m_stylesheets));   

    // third call with flag removed, so check if the calendar stuff is loaded
    $attr->removeFlag(AF_DATE_NO_CALENDAR);
    $attr->edit(array(), '', 'edit');
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(4, count($page->m_scriptfiles));
    $this->assertEqual(1, count($page->m_stylesheets));   
    
    // direct call to register scripts and styles (with calendar)
    DateAttributeWrapper::resetBaseScriptsRegistered();
    $page->m_scriptfiles = array();
    $page->m_scriptcode = array('before' => array(), 'after' => array());
    $page->m_stylesheets = array();
    atkDateAttribute::registerScriptsAndStyles(true);
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(4, count($page->m_scriptfiles));
    $this->assertEqual(1, count($page->m_stylesheets));       
    
    // second call
    atkDateAttribute::registerScriptsAndStyles(true);
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(4, count($page->m_scriptfiles));
    $this->assertEqual(1, count($page->m_stylesheets));   

    // direct call to register scripts and styles (without calendar)
    DateAttributeWrapper::resetBaseScriptsRegistered();
    $page->m_scriptfiles = array();
    $page->m_scriptcode = array('before' => array(), 'after' => array());
    $page->m_stylesheets = array();
    atkDateAttribute::registerScriptsAndStyles(false);
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(1, count($page->m_scriptfiles));
    $this->assertEqual(0, count($page->m_stylesheets));       
    
    // second call
    atkDateAttribute::registerScriptsAndStyles(false);
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(1, count($page->m_scriptfiles));
    $this->assertEqual(0, count($page->m_stylesheets));       
    
    // third call now with calendar enabled
    atkDateAttribute::registerScriptsAndStyles(true);
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(4, count($page->m_scriptfiles));
    $this->assertEqual(1, count($page->m_stylesheets));

    $page->m_scriptfiles = $orgScriptFiles;
    $page->m_scriptcode = $orgScriptCode;
    $page->m_stylesheets = $orgStylesheets;
  }
}