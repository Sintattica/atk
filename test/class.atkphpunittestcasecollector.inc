<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage test
   *
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5149 $
   * $Id: class.atkphpunittestcasecollector.inc 5149 2007-08-08 15:59:28Z boy $
   */
  atkimport("atk.test.atktestcasecollector");

  /**
   * Collector of PHPUnit testcases
   *
   * @author Boy Baukema <boy@achievo.org>
   * @package atk
   * @subpackage test
   */
  class atkPHPUnitTestCaseCollector extends atkTestCaseCollector
  {
    function isValidTestClass($fullpath, $filename, $testclassname)
    {
      if (parent::isValidTestClass($fullpath,$filename,$testclassname) &&
          strstr($filename,'phpunit')) return true;
      return false;
    }

    function addTestClass($fullpath, $testclassname)
    {
      $this->m_tester->addTestFile($fullpath);
    }
  }

?>