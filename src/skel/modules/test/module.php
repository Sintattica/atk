<?php

/**
 * Example module.
 */
class mod_test extends Atk_Module
{

    function getNodes()
    {
        // register nodes
        Atk_Tools::registerNode("test.test", array("admin", "add", "edit", "delete"));
    }

    function getMenuItems()
    {
        // add menuitems
        $this->menuitem("test");
        $this->menuitem("test_admin", Atk_Tools::dispatch_url("test.test", "admin"), "test",
            array("test.test", "admin"));
    }

}