<?php
  /**
   * Example module.
   */
  class mod_test extends atkModule
  {
    function getNodes()
    {
      // register nodes
      registerNode("test.test", array("admin", "add", "edit", "delete"));
    }

    function getMenuItems()
    {
      // add menuitems
      $this->menuitem("test");
      $this->menuitem("test_admin", dispatch_url("test.test", "admin"), "test", array("test.test", "admin"));
    }
  }

?>