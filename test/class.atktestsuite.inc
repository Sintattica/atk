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
   * @copyright (c)2005 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  set_time_limit(0); // no time limit

  /**
   * @internal Includes
   */
  require_once(atkconfig("atkroot")."atk/atktools.inc");
  require_once(atkconfig("atkroot")."atk/test/simpletest/unit_tester.php");
  require_once(atkconfig("atkroot")."atk/test/simpletest/reporter.php");
  atkimport("atk.test.atksimpletesttestcasecollector");
  atkimport("atk.test.atktestreporter");
  atkimport("atk.test.atkattributetestcase");
  atkimport("atk.test.atktestcase");
  atkimport("atk.db.atkdb");
  atkimport("atk.db.atkddl");

  /**
   * The atkTestSuite is a SimpleTest framework wrapper that auto-detects
   * testcases in the entire atk application and atk itself. By running
   * test.php from the ATK skel (place test.php in your application root if
   * it's not there already), you can test both ATK and your own application.
   *
   * To create a testcase, just create a file named class.test_<classname>.inc
   * where <classname> is the name of the class you want to test. The file
   * should create a valid SimpleTest testcase class.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage test
   *
   */
  class atkTestSuite
  {
    /**
     * Constructor
     */
    function atkTestSuite()
    {
      error_reporting(E_ALL & ~E_NOTICE);
    }

    /**
     * Detect if a test-database is used or not.
     *
     * @return boolean use test-database?
     * @static
     */
    function useTestDatabase()
    {
      $config = atkconfig('db');
      $mapping = atkconfig('test_db_mapping');
      return ((isset($config['test']) || count($mapping) > 1 || (count($mapping) == 1 && key($mapping) != 'default'))  &&
             !$_REQUEST['disable_test_database']);
    }

    /**
     * Detect, and run, all available tests.
     * @param string $reporttype Simpletest report type, can be 'text' or 'html'
     *                           If you are running it in cli mode, it will select
     *                           text automaticly
     * @param string $module Run only tests from a single module. Passing an
     *                       empty string (default) will run all available tests.
     *                       passing "atk" will only run atk's own tests.
     */
    function run($reporttype="html", $module="")
    {
      $config = atkconfig('db');

      if (atkTestSuite::useTestDatabase())
      {
        // setup the test database(s)
        $this->_setupTestDatabases();
      }
      $test = &new GroupTest("Test Page");

      if($_REQUEST['class']) $class = $_REQUEST['class'];
      else $class = $_REQUEST['atkclass'];

      if(file_exists(getClassPath($class)) && strpos($class, "test_")!=false) $testclass = $class;
      else $testclass = $this->getTestCaseLocationForClass($class);

      if ($class && file_exists(getClassPath($testclass)))
      {
        $this->_runTestCase($test,$testclass);
      }
      else
      {
        $traverser = new atkTestTraverser(new atkSimpleTestTestCaseCollector($test));
        $traverser->addTestsByTraversing($module);
      }
      if (isset($_REQUEST['list'])) return;

      if($reporttype=="html")
      {
        $testresults = $test->run(atknew("atk.test.atktestreporter"));
      }
      elseif($reporttype=="arrayHtml")
      {
        $testresults = $test->run(atknew("atk.test.arrayhtmlreporter"));
      }
      elseif($reporttype=="arrayText")
      {
        $testresults = $test->run(atknew("atk.test.arraytextreporter"));
      }
      else
      {
        $testresults = $test->run(new TextReporter());
      }

      return $testresults;
    }
    
    /**
     * Run/add test-cases to the given group test. Can be overriden to register
     * custom test classes.
     *
     * @param GroupTest $test group test
     * @param string $module  ATK module name
     */
    function runTestCases(&$test, $module='')
    {
      if($_REQUEST['class']) $class = $_REQUEST['class'];
      else $class = $_REQUEST['atkclass'];

      if(file_exists(getClassPath($class)) && strpos($class, "test_")!=false) $testclass = $class;
      else $testclass = $this->getTestCaseLocationForClass($class);

      if ($class)
      {
        $this->_runTestCase($test,$testclass);
      }
      else
      {
        $this->_addTestsByTraversing($test,$module);
      }      
    }

    /**
     * Setup the test database(s). Clones the database structure of the
     * default databases to the test databases. All data in the test
     * databases will be lost!
     */
    function _setupTestDatabases()
    {
      $mapping = atkconfig('test_db_mapping');
      foreach ($mapping as $normal => $test)
      {
        $normalDb = &atkGetDb($normal);
        $testDb = &atkGetDb($test);

        if($testDb->getDbStatus() <> DB_SUCCESS)
        {
          atkerror("test db '$test' is not acceptable");
          break;
        }
        
        $config = atkconfig('db');
        $clone = !isset($config[$test]['clone_structure']) || $config[$test]['clone_structure'];

        if ($clone)
        {
          $testDb->cloneAll($normalDb);
        }
        else
        {
          $testDb->toggleForeignKeys(false);
          
          if ($config['default']['db'] == $config[$test]['db']) atkerror("atkTestSuite::_setupTestDatabases() | testdatabase equals default database: '{$config['default']['db']}'! Database will not be emptied");
          else $testDb->deleteAll();           

          $testDb->commit();
          $testDb->toggleForeignKeys(true);          
        }
      }
    }

    function getTestClassPath($testclass)
    {
      $exploded = explode(".", $testclass);
      $class = array_pop($exploded);
      $path = implode('/',$exploded).'/class.'.$class.'.inc';
      return $path;
    }

    function getTestCaseLocationForClass($class)
    {
      $exploded = explode('.',$class);
      if ($exploded[0] == 'platform') array_shift($exploded);
      if ($exploded[0] == 'modules') $exploded[0] = 'module';
      if ($exploded[0] == 'platform_modules') $exploded[0] = 'module';
      $class = array_pop($exploded);
      return implode('.',$exploded).'.'.$class;
    }

    function _runTestCase(&$test,$testcase)
    {
      atkimport($testcase,true);
      $test->addTestClass(array_pop(explode(".", $testcase)));
    }
  }

?>
