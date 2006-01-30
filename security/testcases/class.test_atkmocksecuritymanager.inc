<?php

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