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
  }
?>