<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @access private
   *
   * @copyright (c)2007 Ibuildings
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Testcases for class atkTools.
   *
   * @author Yury Golovnay <yury@achievo.org>
   * @package atk
   * @access private
   */

  atkimport("atk.atktools");

  class test_atktools extends atkTestCase
  {
    /**
     * Test instantiation of classes using atknew.
     */
    function test_atknew()
    {
      $object1 = atknew('atk.testcases.stubs.stub_atknew');
      $this->assertTrue(class_exists("stub_atknew"));      
      $this->assertNotNull($object1);
      $this->assertIsA($object1, "stub_atknew");
      $this->assertEqual(0, count($object1->getArgs()));
      
      $object2 = atknew('atk.testcases.stubs.stub_atknew', 'a', 2, $object1, array('a', 'b', 'c'));
      $this->assertNotNull($object2);
      $this->assertIsA($object2, "stub_atknew");
      $args = $object2->getArgs();
      $this->assertEqual(4, count($args));
      $this->assertEqual('a', $args[0]);
      $this->assertEqual(2, $args[1]);
      $this->assertEqual($object1, $args[2]);
      $this->assertEqual(array('a', 'b', 'c'), $args[3]);            
    }
    
    
    function test_stripQuotes()
    {      
      $this->assertEqual(stripQuotes("'1836'"),"1836","check stripQuotes '");
      $this->assertEqual(stripQuotes("\"1836\""),"1836","check stripQuotes \"");
      $this->assertEqual(stripQuotes(" '1836'  "),"1836","check stripQuotes with whitespaces");
    }

    function test_decodeKeyValueSet()
    {      
      $this->assertEqual(decodeKeyValueSet("id='1836'"),array("id"=>"1836"),"check decodeKeyValueSet");
      $this->assertEqual(decodeKeyValueSet("id = '1836'"),array("id"=>"1836"),"check decodeKeyValueSet with more spaces");
    }
    
    function test_atkArrayDiff()
    {
      $array1 = array();
      $array2 = array('element1'=>1,'element2'=>2);
      $this->assertFalse(atkArrayDiff($array1,$array2),'Diff arrays, param1 empty array');
      $this->assertEqual(atkArrayDiff($array2,$array1),$array2,'Diff arrays, param2 empty array');
      $array1 = $array2;
      $this->assertFalse(atkArrayDiff($array1,$array2),'Diff arrays, both arrays are the same');
      $array2= array('element3'=>3);
      $this->assertTrue(atkArrayDiff($array1,$array2),'Diff arrays, array1 has one element less');
      
      $array1 = array('elements'=>array('element1'=>1,
                                        'element2'=>array('name'=>'test')));
      $array2 = $array1;
      $this->assertFalse(atkArrayDiff($array1,$array2),'Diff arrays, both multidimensional arrays are the same');
      $array2['elements']['element3']=3;
      $this->assertFalse(atkArrayDiff($array1,$array2),'Diff arrays, multidimensional arrays are not the same');
      var_dump(atkArrayDiff($array1,$array2));
      
    }
    
    function test_atkArrayCompare()
    {
      $array1 = array();
      $array2 = array('element1'=>1,'element2'=>2);
      $this->assertFalse(atkArrayCompare($array1,$array2),'Compare arrays, array1 empty');
      $this->assertTrue(atkArrayCompare($array2,$array1),'Compare arrays, array2 empty');
      $array1 = $array2;
      $this->assertFalse(atkArrayCompare($array1,$array2),'Compare arrays, both arrays are the same');
      $array2= array('element3'=>3);
      $this->assertTrue(atkArrayCompare($array1,$array2),'Compare arrays, array1 has one element less');
      
      $array1 = array('elements'=>array('element1'=>1,
                                        'element2'=>array('name'=>'test')));
      $array2 = $array1;
      $this->assertFalse(atkArrayCompare($array1,$array2),'Compare arrays, both multidimensional arrays are the same');
      $array2['elements']['element3']=3;
      $this->assertFalse(atkArrayCompare($array1,$array2),'Compare arrays, multidimensional arrays are not the same');
            
    }
  }
?>