<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @access private
   *
   * @copyright (c)2005 Ibuildings
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */


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