<?php

require "atk/modules/class.atkmodule.inc";

/**
 * Example module.
 */
if(!defined("CLASS_MOD_TEST_INC"))
{
  define("CLASS_MOD_TEST_INC",1);

  class mod_test extends atkModule
  {
    function mod_test()
    {
      /* register nodes */
      $this->registerNode("test", array("admin", "add", "edit", "delete"));
      
      /* add menuitems */
      $this->menuitem("-");      
      $this->menuitem("mod_test");
      $this->menuitem("test_admin", "mod_test", dispatch_url("mod_test.test", "admin"), 1);
    }
  }
}

?>
