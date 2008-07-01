<?php
useattrib('atkdateattribute');

class DateAttributeWrapper extends atkDateAttribute 
{
  public static function resetBaseScriptsRegistered()
  {
    self::$s_baseScriptsRegistered = false;
  }
  
  public static function getBaseScriptsRegistered()
  {
    return self::$s_baseScriptsRegistered;
  }
}

/**
 * Tests for the atkDateAttribute.
 */
class test_atkDateAttribute extends atkTestCase 
{
  
  private $attr;
  
  /**
   * Constructor.
   */
  public function __construct()
  {
    // don't use test database
    $this->setUseTestDatabase(false);
  }
  
  function setUp()
  {
    $this->attr = new DateAttributeWrapper('xyz');
    $this->attr->resetBaseScriptsRegistered();
    
    $this->attr->setDateMax(array('year' => '2010','month' => 1, 'day' => 1));
    $this->attr->setDateMin(array('year' => '2008','month' => 1, 'day' => 1));
    $this->attr->setFormatEdit('d m Y');
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
    
    // normal edit with calendar
    DateAttributeWrapper::resetBaseScriptsRegistered();
    $page->m_scriptfiles = array();
    $page->m_scriptcode = array('before' => array(), 'after' => array());
    $page->m_stylesheets = array();
    $this->attr->edit(array(), '', 'edit');
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(4, count($page->m_scriptfiles));
    $this->assertEqual(1, count($page->m_stylesheets));

    // second call
    $this->attr->edit(array(), '', 'edit');    
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(4, count($page->m_scriptfiles));
    $this->assertEqual(1, count($page->m_stylesheets));
    
    // edit without calendar
    DateAttributeWrapper::resetBaseScriptsRegistered();
    $page->m_scriptfiles = array();
    $page->m_scriptcode = array('before' => array(), 'after' => array());
    $page->m_stylesheets = array();
    $this->attr->addFlag(AF_DATE_NO_CALENDAR);
    $this->attr->edit(array(), '', 'edit');
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(1, count($page->m_scriptfiles));
    $this->assertEqual(0, count($page->m_stylesheets));    

    // second call
    DateAttributeWrapper::resetBaseScriptsRegistered();
    $page->m_scriptfiles = array();
    $page->m_scriptcode = array('before' => array(), 'after' => array());
    $page->m_stylesheets = array();
    $this->attr->edit(array(), '', 'edit');
    $this->assertEqual(1, count($page->m_scriptcode['before']));
    $this->assertWantedPattern("/m_months_long/", $page->m_scriptcode['before'][0]);
    $this->assertEqual(1, count($page->m_scriptfiles));
    $this->assertEqual(0, count($page->m_stylesheets));   

    // third call with flag removed, so check if the calendar stuff is loaded
    $this->attr->removeFlag(AF_DATE_NO_CALENDAR);
    $this->attr->edit(array(), '', 'edit');
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
     
  public function test_simplemode_default()
  {
    $this->assertFalse($this->attr->getSimpleMode());
  }
  
  public function test_no_simplemode_draw()
  {
    $this->attr->setSimpleMode(false);
    $this->assertEqual('<script language="javascript">var atkdateattribute_xyz = {"format":"d m Y","min":"20080101","max":"20100101","emptyfield":true,"weekday":false};</script><select id="xyz[day]" name="xyz[day]" class="atkdateattribute" onChange="AdjustDate(this, \'xyz\');"><option value="0"></option></select> <select id="xyz[month]" name="xyz[month]" class="atkdateattribute" onChange="AdjustDate(this, \'xyz\');"><option value="0"></option><option value="1" >01</option><option value="2" >02</option><option value="3" >03</option><option value="4" >04</option><option value="5" >05</option><option value="6" >06</option><option value="7" >07</option><option value="8" >08</option><option value="9" >09</option><option value="10" >10</option><option value="11" >11</option><option value="12" >12</option></select> <select id="xyz[year]" name="xyz[year]" class="atkdateattribute" onChange="AdjustDate(this, \'xyz\');"><option value="0"></option><option value="2008" >2008</option><option value="2009" >2009</option><option value="2010" >2010</option></select>',$this->attr->draw());
  }
  
  public function test_simplemode_draw()
  {
    $this->attr->setSimpleMode(true);
    $this->assertEqual('<select id="xyz[day]" name="xyz[day]" class="atkdateattribute" onChange=""><option value="0"></option><option value="1" >01</option><option value="2" >02</option><option value="3" >03</option><option value="4" >04</option><option value="5" >05</option><option value="6" >06</option><option value="7" >07</option><option value="8" >08</option><option value="9" >09</option><option value="10" >10</option><option value="11" >11</option><option value="12" >12</option><option value="13" >13</option><option value="14" >14</option><option value="15" >15</option><option value="16" >16</option><option value="17" >17</option><option value="18" >18</option><option value="19" >19</option><option value="20" >20</option><option value="21" >21</option><option value="22" >22</option><option value="23" >23</option><option value="24" >24</option><option value="25" >25</option><option value="26" >26</option><option value="27" >27</option><option value="28" >28</option><option value="29" >29</option><option value="30" >30</option><option value="31" >31</option></select> <select id="xyz[month]" name="xyz[month]" class="atkdateattribute" onChange=""><option value="0"></option><option value="1" >01</option><option value="2" >02</option><option value="3" >03</option><option value="4" >04</option><option value="5" >05</option><option value="6" >06</option><option value="7" >07</option><option value="8" >08</option><option value="9" >09</option><option value="10" >10</option><option value="11" >11</option><option value="12" >12</option></select> <select id="xyz[year]" name="xyz[year]" class="atkdateattribute" onChange=""><option value="0"></option><option value="2008" >2008</option><option value="2009" >2009</option><option value="2010" >2010</option></select>',$this->attr->draw());
  }
  
  public function test_simplemode_scripts_registered()
  {
    $this->attr->draw();
    $this->assertTrue($this->attr->getBaseScriptsRegistered());
  }
  
  public function test_simplemode_scripts_not_registered()
  {
    $this->attr->setSimpleMode(true);
    $this->attr->draw();
    $this->assertFalse($this->attr->getBaseScriptsRegistered());
  }
  
  public function test_sortyear_default()
  {
    $this->assertEqual(atkDateAttribute::SORT_YEAR_ASC,$this->attr->getYearSorting());
  }
  
  public function test_sortyear_set()
  {
    $this->attr->setYearSorting(atkDateAttribute::SORT_YEAR_ASC);
    $this->assertEqual(atkDateAttribute::SORT_YEAR_ASC,$this->attr->getYearSorting());
    $this->attr->setYearSorting(atkDateAttribute::SORT_YEAR_DESC);
    $this->assertEqual(atkDateAttribute::SORT_YEAR_DESC,$this->attr->getYearSorting());
    $this->attr->setYearSorting('bogus');
    $this->assertEqual(atkDateAttribute::SORT_YEAR_DESC,$this->attr->getYearSorting());
  }
  
  public function test_sortyear_draw()
  {
    $this->attr->addFlag(AF_DATE_NO_CALENDAR);
    $this->attr->setFormatEdit('Y');
    
    $this->attr->setYearSorting(atkDateAttribute::SORT_YEAR_ASC);
    $this->assertEqual('<script language="javascript">var atkdateattribute_xyz = {"format":"Y","min":"20080101","max":"20100101","emptyfield":true,"weekday":true};</script><select id="xyz[year]" name="xyz[year]" class="atkdateattribute" onChange="AdjustDate(this, \'xyz\');"><option value="0"></option><option value="2008" >2008</option><option value="2009" >2009</option><option value="2010" >2010</option></select>',$this->attr->draw());
    $this->attr->setYearSorting(atkDateAttribute::SORT_YEAR_DESC);    
    $this->assertEqual('<script language="javascript">var atkdateattribute_xyz = {"format":"Y","min":"20080101","max":"20100101","emptyfield":true,"weekday":true};</script><select id="xyz[year]" name="xyz[year]" class="atkdateattribute" onChange="AdjustDate(this, \'xyz\');"><option value="0"></option><option value="2010" >2010</option><option value="2009" >2009</option><option value="2008" >2008</option></select>',$this->attr->draw());
  }
  
  
}