<?php

  atkimport("atk.utils.atkstringparser");

  class test_atkStringParser extends UnitTestCase
  {
    function testParser()
    {
      // without encoding
      $this->runParserTest("[color] [fruit]", array("color"=>"blue", "fruit"=>"apple"), "blue apple");
      $this->runParserTest("[color] [fruit]", array("color"=>"blue"), "blue ");
      $this->runParserTest("[color] [fruit]", array(), " ");
      $this->runParserTest("[color] [user]", array("color"=>"blue", "fruit"=>"apple"), "blue ");
      $this->runParserTest("untouched", array("color"=>"blue", "fruit"=>"apple"), "untouched");
      $this->runParserTest("untouched", array(), "untouched");
      
      // with encoding      
      $this->runParserTest("[color] [fruit]", array("color"=>"thing with spaces", "fruit"=>"thing with '"), "thing%20with%20spaces thing%20with%20%27", true);
    }
    
    function runParserTest($input, $data, $expected, $encode=false)
    {
      $parser = new atkStringParser($input);
      $result = $parser->parse($data, $encode);      
      $this->assertEqual($result, $expected);
    }
  }

?>