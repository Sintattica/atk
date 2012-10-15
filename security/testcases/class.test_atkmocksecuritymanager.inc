<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage security
   * @access private
   *
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 3790 $
   * $Id$
   */


  /**
   * Testcases for the atkMockSecurityManager
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage security
   * @access private
   */
  class test_atkMockSecurityManager extends atkTestCase
  {
    function test_mocksecuritymanager()
    {
      atkimport("atk.security.atkmocksecuritymanager");
      $secmgr = &new atkMockSecurityManager();
      
      $secmgr->setAllowed(false, "somenode.some_privilege");
      $this->assertFalse($secmgr->allowed("somenode", "some_privilege"), "Mock must return allowed false");
      
      $secmgr->setAllowed(true, "somenode.some_privilege");
      $this->assertTrue($secmgr->allowed("somenode", "some_privilege"), "Mock must return allowed true");
      
      $secmgr->setAllowed(false);
      $this->assertFalse($secmgr->allowed("somenode", "some_otherprivilege"), "Mock must always return allowed false");
      
      $secmgr->setAllowed(true);
      $this->assertTrue($secmgr->allowed("somenode", "some_otherprivilege"), "Mock must always return allowed true");
    }
  }

?>
