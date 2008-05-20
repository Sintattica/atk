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
   * @version $Revision: 5195 $
   * $Id: class.atksimpletesttestcasecollector.inc 5195 2007-08-24 13:10:56Z marc $
   */

  atkimport("atk.test.atktestcasecollector");

  /**
   * Collector of SimpleTest testcases
   *
   * @author Boy Baukema <boy@achievo.org>
   * @package atk
   * @subpackage test
   */
  class atkSimpleTestTestCaseCollector extends atkTestCaseCollector
  {
    function isValidTestClass($fullpath, $filename, $testclassname)
    {
      if (parent::isValidTestClass($fullpath,$filename,$testclassname) &&
          !strstr($filename,'phpunit')) return true;
      return false;
    }
    
    function addTestClass($fullpath, $testclassname)
    {
      include_once($fullpath);
      $this->m_tester->addTestClass($testclassname);
    }
  }

?>