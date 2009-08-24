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
      $this->assertParserResult("[color] [fruit]", array("color"=>"blue", "fruit"=>"apple"), "blue apple");
      $this->assertParserResult("[color] [fruit]", array("color"=>"blue"), "blue ");
      $this->assertParserResult("[color] [fruit]", array("color"=>"blue"), "blue [fruit]", false, false);      
      $this->assertParserResult("[color] [fruit]", array(), " ");
      $this->assertParserResult("[color] [fruit]", array(), "[color] [fruit]", false, false);
      
      $this->assertParserResult("[color] [user]", array("color"=>"blue", "fruit"=>"apple"), "blue ");
      $this->assertParserResult("untouched", array("color"=>"blue", "fruit"=>"apple"), "untouched");
      $this->assertParserResult("untouched", array(), "untouched");
      $this->assertParserResult('[fruit.banana.color]',array('fruit'=>array('banana'=>array('color'=>'yellow'))),'yellow');
      $this->assertParserResult('[fruit.banana.color.hex]',array('fruit'=>array('banana'=>array('color'=>'yellow'))),'yellow');
      $this->assertParserResult('[fruit.banana.color.hex]',array('fruit'=>array('banana'=>array('color'=>'yellow'))),'[fruit.banana.color.hex]', false, false);
      
      
      // with encoding      
      $this->assertParserResult("[color] [fruit]", array("color"=>"thing with spaces", "fruit"=>"thing with '"), "thing%20with%20spaces thing%20with%20%27", true);
    }
    
    public function test_arrayaccess()
    {
      // only array objects
      $this->assertParserResult("[color] [fruit]", new ArrayObject(array("color"=>"blue", "fruit"=>"apple")), "blue apple");
      $this->assertParserResult("[color] [fruit]", new ArrayObject(array("color"=>"blue")), "blue ");
      $this->assertParserResult("[color] [fruit]", new ArrayObject(array("color"=>"blue")), "blue [fruit]", false, false);
      $this->assertParserResult("[color] [fruit]", new ArrayObject(), " ");
      $this->assertParserResult("[color] [fruit]", new ArrayObject(), "[color] [fruit]", false, false);
      $this->assertParserResult("[color] [user]", new ArrayObject(array("color"=>"blue", "fruit"=>"apple")), "blue ");
      $this->assertParserResult("untouched", new ArrayObject(array("color"=>"blue", "fruit"=>"apple")), "untouched");
      $this->assertParserResult("untouched", new ArrayObject(array()), "untouched");      
      $data = new ArrayObject(array('fruit' => new ArrayObject(array('banana' => new ArrayObject(array('color' => 'yellow'))))));
      $this->assertParserResult('[fruit.banana.color]', $data, 'yellow');      
      $this->assertParserResult('[fruit.banana.color.hex]', $data, 'yellow');      
      $this->assertParserResult('[fruit.banana.color.hex]', $data, '[fruit.banana.color.hex]', false, false);      
      
      
      // combination of arrays and array objects
      $data = array('fruit' => new ArrayObject(array('banana' => array('color' => 'yellow'))));
      $this->assertParserResult('[fruit.banana.color.hex]', $data, 'yellow');
    }
    
    private function assertParserResult($input, $data, $expected, $encode=false, $replaceUnknownFields=true)
    {
      $parser = new atkStringParser($input);
      $result = $parser->parse($data, $encode, $replaceUnknownFields);      
      $this->assertEqual($result, $expected);
    }
  }

?>