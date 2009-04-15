<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage testcases
   *
   * @copyright (c)2008 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6065 $
   */

  /**
   * Only used for the testcases due to the protectedness of the
   * uriParts method.
   */
  atkimport('atk.front.atkfrontcontroller');

  /** 
   * @internal
   * @package atk
   * @subpackage testcases
   */
  class uriPartsFrontControllerMock extends atkFrontController
  {
    public static function uriPartsProxy($uri)
    {
      return self::uriParts($uri);
    }
  }

  /**
   * Testcases for atkNode
   * @internal
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
