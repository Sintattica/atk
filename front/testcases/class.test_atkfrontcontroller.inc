<?php
  /**
   * test_atkfrontcontroller class file
   *
   * @package atk
   * @subpackage testcases
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   *
   * @version $Revision: 5016 $
   * $Id: class.test_atknode.inc 5016 2007-06-27 04:47:20Z guido $
   */


  /**
   * Only used for the testcases due to the protectedness of the
   * uriParts method.
   */
  atkimport('atk.front.atkfrontcontroller');
  class uriPartsFrontControllerMock extends atkFrontController
  {
    public static function uriPartsProxy($uri)
    {
      return self::uriParts($uri);
    }
  }

  /**
   * Testcases for atkNode
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk
   * @subpackage testcases
   */
   class test_atkfrontcontroller extends atkTestCase
   {
    function test_uriParts()
    {
      $uri = 'dummy/pizza/tomato';
      $res = uriPartsFrontControllerMock::uriPartsProxy($uri);
      $this->assertTrue(is_array($res));
      $this->assertEqual(3,count($res));
      $this->assertEqual(array('dummy','pizza','tomato'),$res);

      $uri = '/' .$uri;
      $res = uriPartsFrontControllerMock::uriPartsProxy($uri);
      $this->assertTrue(is_array($res));
      $this->assertEqual(3,count($res));
      $this->assertEqual(array('dummy','pizza','tomato'),$res);

      $uri = $uri . '/cheese';
      $res = uriPartsFrontControllerMock::uriPartsProxy($uri);
      $this->assertTrue(is_array($res));
      $this->assertEqual(4,count($res));
      $this->assertEqual(array('dummy','pizza','tomato','cheese'),$res);
    }
  }
?>