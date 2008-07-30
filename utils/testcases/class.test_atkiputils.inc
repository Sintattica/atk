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
   * @copyright (c)2007 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   */

  /** @internal includes */
  atkimport("atk.utils.atkiputils");

  /**
   * @internal
   * @package atk
   * @subpackage testcases 
   */
  class test_atkiputils extends atkTestCase
  {
    function test_validipstringvalidate()
    {
      $this->assertTrue(atkIpUtils::ipValidate("0.0.0.0"));
      $this->assertTrue(atkIpUtils::ipValidate("1.2.3.4"));
      $this->assertTrue(atkIpUtils::ipValidate("87.65.43.21"));
      $this->assertTrue(atkIpUtils::ipValidate("255.255.255.255"));
    }

    function test_validiplongvalidate()
    {
      $this->assertTrue(atkIpUtils::ipValidate(0));
      $this->assertTrue(atkIpUtils::ipValidate("0"));
      $this->assertTrue(atkIpUtils::ipValidate(4294967295));
      $this->assertTrue(atkIpUtils::ipValidate("4294967295"));
    }

    function test_invalidipstringvalidate()
    {
      $this->assertFalse(atkIpUtils::ipValidate("0.0.0."));
      $this->assertFalse(atkIpUtils::ipValidate(".0.0.0"));
      $this->assertFalse(atkIpUtils::ipValidate("0.0.0"));
      $this->assertFalse(atkIpUtils::ipValidate("256.1.2.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.256.2.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.2.256.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.2.3.256"));
      $this->assertFalse(atkIpUtils::ipValidate("-1.1.2.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.-1.2.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.2.-1.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.2.3.-1"));
      $this->assertFalse(atkIpUtils::ipValidate("*.1.2.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.*.2.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.2.*.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.2.3.*"));
      $this->assertFalse(atkIpUtils::ipValidate(".1.2.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1..2.3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.2..3"));
      $this->assertFalse(atkIpUtils::ipValidate("1.2.3."));
      $this->assertFalse(atkIpUtils::ipValidate("unknown"));
      $this->assertFalse(atkIpUtils::ipValidate("null"));
      $this->assertFalse(atkIpUtils::ipValidate("-"));
      $this->assertFalse(atkIpUtils::ipValidate(""));
    }

    function test_invalidiplongvalidate()
    {
      $this->assertFalse(atkIpUtils::ipValidate(-1));
      $this->assertFalse(atkIpUtils::ipValidate("-1"));
      $this->assertFalse(atkIpUtils::ipValidate(4294967296));
      $this->assertFalse(atkIpUtils::ipValidate("4294967296"));
    }

    function test_invalidipstringconverttolong()
    {
      $this->assertEqual(atkIpUtils::ipLongFormat("0.0.0."), null);
      $this->assertEqual(atkIpUtils::ipLongFormat(".0.0.0"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("0.0.0"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("256.1.2.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.256.2.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.2.256.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.2.3.256"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("-1.1.2.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.-1.2.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.2.-1.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.2.3.-1"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("x.1.2.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.x.2.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.2.x.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.2.3.x"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat(".1.2.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1..2.3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.2..3"), null);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.2.3."), null);
    }

    function test_validipstringconverttolong()
    {
      $this->assertEqual(atkIpUtils::ipLongFormat("0.0.0.0"), 0);
      $this->assertEqual(atkIpUtils::ipLongFormat("0.0.0.255"), 255);
      $this->assertEqual(atkIpUtils::ipLongFormat("0.0.1.0"), 256);
      $this->assertEqual(atkIpUtils::ipLongFormat("0.0.255.255"), 65535);
      $this->assertEqual(atkIpUtils::ipLongFormat("0.1.0.0"), 65536);
      $this->assertEqual(atkIpUtils::ipLongFormat("0.255.255.255"), 16777215);
      $this->assertEqual(atkIpUtils::ipLongFormat("1.0.0.0"), 16777216);
      $this->assertEqual(atkIpUtils::ipLongFormat("255.255.255.255"), 4294967295);
    }

    function test_invalidiplongconverttostring()
    {
      $this->assertEqual(atkIpUtils::ipStringFormat(-1), "");
      $this->assertEqual(atkIpUtils::ipStringFormat(4294967296), "");
    }

    function test_validiplongconverttostring()
    {
      $this->assertEqual(atkIpUtils::ipStringFormat(0), "0.0.0.0");
      $this->assertEqual(atkIpUtils::ipStringFormat(255), "0.0.0.255");
      $this->assertEqual(atkIpUtils::ipStringFormat(256), "0.0.1.0");
      $this->assertEqual(atkIpUtils::ipStringFormat(65535), "0.0.255.255");
      $this->assertEqual(atkIpUtils::ipStringFormat(65536), "0.1.0.0");
      $this->assertEqual(atkIpUtils::ipStringFormat(16777215), "0.255.255.255");
      $this->assertEqual(atkIpUtils::ipStringFormat(16777216), "1.0.0.0");
      $this->assertEqual(atkIpUtils::ipStringFormat(4294967295), "255.255.255.255");
    }

    function test_practicalexampleconversions()
    {
      $practicalexamples = array(
        "81.126.216.242",
        "141.53.127.196",
        "163.202.196.72",
        "17.202.196.71",
        "214.137.45.91",
        "112.45.32.217",
        "42.74.129.85",
        "82.156.239.162",
        "34.128.229.126",
        "64.246.53.176",
        "113.148.229.197",
        "62.173.201.17",
        "54.29.244.108",
      );
      foreach($practicalexamples as $ip)
        $this->assertEqual(atkIpUtils::ipStringFormat(atkIpUtils::ipLongFormat($ip)), $ip);
    }
  }
?>
