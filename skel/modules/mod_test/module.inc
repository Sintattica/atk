<?php

  require_once("atk/modules/class.atkmodule.inc");

  /**
   * Example module.
   */

  class mod_test extends atkModule
  {
    function mod_test()
    {
      /* register nodes */
      $this->registerNode("test", array("admin", "add", "edit", "delete"));
      
      /* add menuitems */
      $this->menuitem("-");      
      $this->menuitem("mod_test");
      $this->menuitem("test_admin", dispatch_url("mod_test.test", "admin"), "mod_test", 1);
    }
  }

?>