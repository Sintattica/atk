<?php

class mod_users extends atkModule
{
  function getNodes()
  {
    /* register nodes */
    registerNode('users.employee', array('admin', 'add', 'edit', 'delete'));
    registerNode('users.customer', array('admin', 'add', 'edit', 'delete'));
    registerNode('users.supplier', array('admin', 'add', 'edit', 'delete'));
    registerNode('users.group', array('admin', 'add', 'edit', 'delete'));
  }

  function getMenuItems()
  {
    /**
     * The next line adds a submenu called 'users' to the main menu.
     */
    menuitem('users');
    menuitem('employee', dispatch_url('users.employee', 'admin'),'users');
    menuitem('customer', dispatch_url('users.customer', 'admin'),'users');
    menuitem('supplier', dispatch_url('users.supplier', 'admin'),'users');
    menuitem('group', dispatch_url('users.group', 'admin'),'users');
  }
}

?>