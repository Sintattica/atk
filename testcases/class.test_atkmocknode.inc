<?php

  class test_atkMockNode extends atkTestCase
  {
    function test_mocknode()
    {      
      atkimport("atk.atkmocknode");
      $mock = &new atkMockNode("test");
      
      // First test if original result is retrieved when no
      // result is set.
      $result = $mock->getTabs("edit");    
      $this->assertTrue((is_array($result)&&count($result)==0), "default getTabs should empty array");
      
      // Now test mocking the call
      $mock->setResult("getTabs", array("apple", "banana"));
      
      $result = $mock->getTabs("edit");            
      
      $this->assertTrue((is_array($result) && count($result)==2 && $result[1]=="banana"), "mocked getTabs should return apple and banana");
    }
    
  }

?>