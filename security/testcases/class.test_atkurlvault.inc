<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage security
   *
   * @copyright (c)2007 Ivo Jansch
   * @access private
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Testcases for the atkUrlVault
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage security
   * @access private
   */
  class test_atkUrlVault extends atkTestCase
  {
    function test_vault()
    {
      $vault = &atkinstance("atk.security.atkurlvault");

      $size = $vault->getSize();

      $url_ok = "dispatch.php?atknodetype=something&id=3";
      $url_notok = "dispatch.php?atknodetype=something&id=4";

      $vault->addUrl($url_ok);

      // Test the vault
      $this->assertTrue($vault->containsUrl($url_ok), "Url in vault should be ok");
      $this->assertFalse($vault->containsUrl($url_notok), "Url not in vault should be not ok");

      // Test the max. capacity
      $vault->setSize(3);

      $url1 = "dispatch.php?1";
      $url2 = "dispatch.php?2";
      $url3 = "dispatch.php?3";
      $url4 = "dispatch.php?4";

      // All elements should fit in the vault.
      $vault->addUrl($url1);
      $this->assertTrue($vault->containsUrl($url1), "Url 1 is in the vault");
      $vault->addUrl($url2);
      $this->assertTrue($vault->containsUrl($url2), "Url 2 is in the vault");
      $vault->addUrl($url3);
      $this->assertTrue($vault->containsUrl($url3), "Url 3 is in the vault");

      // Once we add a fourth, vault reaches its capacity. Oldest should be deleted.
      $vault->addUrl($url4);
      $this->assertTrue($vault->containsUrl($url4), "Url 4 is now in the vault");
      $this->assertTrue($vault->containsUrl($url2), "Url 2 is still in the vault");
      $this->assertFalse($vault->containsUrl($url1), "Url 1 is no longer in the vault");

      // Check getSize()
      $this->assertEqual($vault->getSize(), 3, "Size should be 3");
    }
  }

?>