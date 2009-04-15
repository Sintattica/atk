<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   * @access private
   *
   * @copyright (c)2005 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5798 $
   * $Id$
   */

  /**
   * Include the class we are about to test.
   */
  atkimport("atk.utils.atkstringparser");

   /**
   * Testcases for class atkstringparser.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   * @access private
   */
  class test_atkStringParser extends UnitTestCase
  {
    public function test_array()
    {
      // without encoding
      $this->runParserTest("[color] [fruit]", array("color"=>"blue", "fruit"=>"apple"), "blue apple");
      $this->runParserTest("[color] [fruit]", array("color"=>"blue"), "blue ");
      $this->runParserTest("[color] [fruit]", array(), " ");
      $this->runParserTest("[color] [user]", array("color"=>"blue", "fruit"=>"apple"), "blue ");
      $this->runParserTest("untouched", array("color"=>"blue", "fruit"=>"apple"), "untouched");
      $this->runParserTest("untouched", array(), "untouched");
      $this->runParserTest('[fruit.banana.color.hex]',array('fruit'=>array('banana'=>array('color'=>'yellow'))),'yellow');
      
      // with encoding      
      $this->runParserTest("[color] [fruit]", array("color"=>"thing with spaces", "fruit"=>"thing with '"), "thing%20with%20spaces thing%20with%20%27", true);
    }
    
    public function test_arrayaccess()
    {
      // only array objects
      $this->runParserTest("[color] [fruit]", new ArrayObject(array("color"=>"blue", "fruit"=>"apple")), "blue apple");
      $this->runParserTest("[color] [fruit]", new ArrayObject(array("color"=>"blue")), "blue ");
      $this->runParserTest("[color] [fruit]", new ArrayObject(), " ");
      $this->runParserTest("[color] [user]", new ArrayObject(array("color"=>"blue", "fruit"=>"apple")), "blue ");
      $this->runParserTest("untouched", new ArrayObject(array("color"=>"blue", "fruit"=>"apple")), "untouched");
      $this->runParserTest("untouched", new ArrayObject(array()), "untouched");      
      $data = new ArrayObject(array('fruit' => new ArrayObject(array('banana' => new ArrayObject(array('color' => 'yellow'))))));
      $this->runParserTest('[fruit.banana.color.hex]', $data, 'yellow');      
      
      // combination of arrays and array objects
      $data = array('fruit' => new ArrayObject(array('banana' => array('color' => 'yellow'))));
      $this->runParserTest('[fruit.banana.color.hex]', $data, 'yellow');
    }
    
    private function runParserTest($input, $data, $expected, $encode=false)
    {
      $parser = new atkStringParser($input);
      $result = $parser->parse($data, $encode);      
      $this->assertEqual($result, $expected);
    }
  }

?>