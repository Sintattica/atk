<?php
  useattrib("atktextattribute"); 

  class test extends atkNode
  {
    function test()
    {
      $this->atkNode("type");

      $this->add(new atkAttribute("test_id",AF_AUTOKEY));
      $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE));
      $this->add(new atkTextAttribute("description"));

      $this->setOrder("name");
      $this->setTable("mod_test_table");
    }
  }

?>
