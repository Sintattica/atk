<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage test
   *
   * @copyright (c)2005 Ibuildings.nl
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * atkTestCase is a specialization of SimpleTest's UnitTestCase. It
   * contains utility methods that can be used by testcases, such as
   * the ability to swap the default database driver with a mock
   * version
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage test
   *
   */
  class atkTestCase extends UnitTestCase
  {
    var $m_restoreDb = array();
    var $m_restoreNode = array();
    var $m_restoreSecMgr = NULL;

    function setMockDb($conn="default")
    {
      global $config_db;

      $config_db["mock"]["driver"] = "mock";

      $mockdb = &atkGetDb("mock");
      $this->m_restoreDb[$conn] = &atkDb::setInstance($conn, $mockdb);
    }

    function restoreDb($conn="default")
    {
      //$copy = $this->m_restoreDb[$conn];
      atkDb::setInstance($conn, $this->m_restoreDb[$conn]);
      //$this->m_restoreDb[$conn]="";
    }

    function &setMockNode($nodename, &$mocknode)
    {
      $this->m_restoreNode[$nodename] = &atkSetNode($nodename, $mocknode);
    }

    function restoreNode($nodename)
    {
      atkSetNode($nodename, $this->m_restoreNode[$nodename]);
    }

    function &setMockSecurityManager(&$mockmanager)
    {
      $this->m_restoreSecMgr = &atkSetSecurityManager($mockmanager);
    }

    function restoreSecurityManager()
    {
      atkSetSecurityManager($this->m_restoreSecMgr);
    }
  }

?>
