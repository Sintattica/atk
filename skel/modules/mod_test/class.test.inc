<?php

if(!defined("CLASS_TEST_INC"))
{
  define("CLASS_TEST_INC",1);
  class test extends atkNode
  {
    function test()
    {
      $this->atkNode("type");

      $this->addAttribute(new atkAttribute("test_id",AF_PRIMARY|AF_AUTO_INCREMENT));
      $this->addAttribute(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE));
      $this->addAttribute(new atkTextAttribute("description"));

      $this->setOrder("name");
      $this->setTable("mod_test_type", "type");
    }
  }
}
?>