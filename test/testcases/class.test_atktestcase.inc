<?php

 /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage test
   * @access private
   *
   * @copyright (c)2005-2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Testcases for the atkTestCase
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage test
   * @access private
   */
  class test_atktestcase extends atkTestCase
  {
    function test_atktestcase()
    {
    }

    function test_mockdbswap()
    {
      $db1 = &atkGetDb();
      $this->assertNotEqual($db1->m_type, "mock", 'database is not a mock');

      $this->setMockDb();

      $db2 = &atkGetDb();

      $this->assertEqual($db2->m_type, "mock", 'database is now a mock');

      $this->restoreDb();

      $db3 = &atkGetDb();

      $this->assertNotEqual($db3->m_type, "mock", 'database is now not a mock - '.$db3->m_type);

      $this->assertReference($db1, $db3, "make sure db is not copied");

    }

    function test_mocknodeswap()
    {
      // usually we use setMockNode to replace a node with a mock node. But we
      // can test the concept by mocking a nonexisting node.

      atkimport("atk.atkmocknode");
      $mocknode = &new atkMockNode("mock");

      $node1 = &atkGetNode("test.something");
      $this->assertNull($node1, "Node does not exist");

      $this->setMockNode("test.something", $mocknode);

      $node2 = &atkGetNode("test.something");

      $this->assertEqual($node2->m_type, "mock", "Node test.something should now be a mock");

      $this->restoreNode("test.something");
      $node3 = &atkGetNode("test.something");
      $this->assertNull($node1, "Node does not exist again");
    }

    function test_mocksecuritymanagerswap()
    {
      atkimport("atk.security.atkmocksecuritymanager");
      $mockmanager = &new atkMockSecurityManager();

      $mgr1 = &atkGetSecurityManager();
      $this->assertEqual(strtolower(get_class($mgr1)), "atksecuritymanager", "Must be of class atksecuritymanager");

      $this->setMockSecurityManager($mockmanager);

      $mgr2 = &atkGetSecurityManager();
      $this->assertEqual(strtolower(get_class($mgr2)), "atkmocksecuritymanager", "Must be of class atkmocksecuritymanager");

      $this->restoreSecurityManager();
      $mgr3 = &atkGetSecurityManager();
      $this->assertEqual(strtolower(get_class($mgr3)), "atksecuritymanager", "Must be of class atksecuritymanager again");

      $this->assertReference($mgr1, $mgr3, "make sure securitymanager is not copied");
    }
  }

?>
