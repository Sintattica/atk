<?php

  /**
   * Example module.
   */

  class mod_test extends atkModule
  {
    function getNodes()
    {
      /* register nodes */
      registerNode("mode_test.test", array("admin", "add", "edit", "delete"));
    }
    
    function getMenuItems()
    {    
      /* add menuitems */     
      menuitem("mod_test");
      menuitem("test_admin", dispatch_url("mod_test.test", "admin"), "mod_test", array("mod_test.test", "admin"));
    }
  }

?>