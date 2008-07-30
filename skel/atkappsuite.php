<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage skel
   *
   * @copyright (c)2008 Ibuildings
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5798 $
   */

  /**
   * @internal includes and initialization
   */
  ini_set('display_errors',true);

  require_once 'PHPUnit/Framework.php';
  require_once 'PHPUnit/TextUI/TestRunner.php';

  /**
   * Example testsuite for PHPUnit testcases.
   *
   * For non-standard ATK applications you will have to adjust
   * the $config_atkroot and $config_application_dir variables in the loadATK static method.
   *
   * Will use ATK to scoure through your application for class.test_*.phpunit.inc files
   * and use those as phpunit testcases.
   *
   * To run this testsuite in it's default configuration,
   * just make sure this file is located in the application root and run:
   *
   * @package atk
   * @subpackage skel
   * phpunit atkappsuite
   */
  class atkAppSuite extends PHPUnit_Framework_TestCase
  {
    /**
     * PHPUnit workaround:
     * The masses and masses of globals that we use in the ATK config vars
     * are not available in PHPUnit, so include
     */
    protected static function loadATK()
    {
      global $config_atkroot;
      $config_atkroot = "./";
      $config_application_dir = $config_atkroot;

      require_once($config_atkroot."atk/class.atkconfig.inc");
      require_once($config_atkroot."atk/defaultconfig.inc.php");

      $vars = get_defined_vars();
      foreach ($vars as $varname=>$value)
      {
        if((substr($varname, 0, strlen('config_'))=='config_') OR (substr($varname, 0, strlen('g_'))=='g_'))
        {
          global ${$varname};
          ${$varname} = $value;
        }
      }
      require_once($config_application_dir."atk.inc");
    }

    /**
     * Build a testsuite from all the tests in the modules.
     *
     * @return PHPUnit_Framework_TestSuite The complete modules test suite.
     */
    public static function suite()
    {
      self::loadATK();
      $suite = new PHPUnit_Framework_TestSuite('PHPUnit Framework');

      atkimport('atk.test.atktesttraverser');
      atkimport('atk.test.atkphpunittestcasecollector');

      atkTestTraverser::create(new atkPHPUnitTestCaseCollector($suite))->addTestsByTraversing();

      return $suite;
    }
  }

?>
