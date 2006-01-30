<?

  require_once(atkconfig("atkroot")."atk/test/simpletest/unit_tester.php");  
  
  /**
   * Tests the tabs security
   *
   * ATK has now seperate security settings for different tabs within 
   * a node. This testcase tests the functionality.
   *
   * @author harrie <harrie@ibuildings.nl>
   */
  class test_tabsecurity extends atkTestCase 
  {
    function test_tabAllowed()
    { 
      global $g_nodes;

      // fake g_nodes
      // (advanced is a required tab)
      $g_nodes = array("unittest"=>array("testnode"=>array("tab_advanced")));
      
      $tabs = array("default", "advanced");
      
      $secMgr = &new atkMockSecurityManager();
      $secMgr->setAllowed(false);            
      $this->setMockSecurityManager($secMgr);
      
      atkimport("atk.atknode");
      $myNode = new atkNode("testnode");
      $myNode->m_module="unittest";
      $myNode->checkTabRights($tabs);       
      
      $this->restoreSecurityManager();     
      
      $this->assertEqual($tabs,array("default"),"Checking tabrights method");
    }
    
    function test_tabAllowed_backward_comp()
    { 
      global $g_nodes;

      // fake g_nodes
      // (advanced is a required tab)
      $g_nodes = array("unittest"=>array("testnode"=>array()));
      
      $tabs = array("default", "advanced");
      
      $secMgr = &new atkMockSecurityManager();
      $secMgr->setAllowed(false);      
      $this->setMockSecurityManager($secMgr);
      
      atkimport("atk.atknode");
      $myNode = new atkNode("testnode");
      $myNode->m_module="unittest";
      $myNode->checkTabRights($tabs);
      
      $this->assertEqual($tabs,array("default","advanced"),"Checking tabrights method (backward compatibility)");
      
      $this->restoreSecurityManager();
    }
  }

?>