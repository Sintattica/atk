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

  /**
   * @internal Includes
   */
  require_once(atkconfig("atkroot")."atk/atktools.inc");
  require_once(atkconfig("atkroot")."atk/test/simpletest/unit_tester.php");
  require_once(atkconfig("atkroot")."atk/test/simpletest/reporter.php");
  atkimport("atk.test.atktestreporter");
  atkimport("atk.test.atkattributetestcase");
  atkimport("atk.test.atktestcase");
  atkimport("atk.db.atkdb");
  atkimport("atk.db.atkddl");

  /**
   * atkTestCaseCollector is a visitor that can be passed to an
   * atkDirecoryTraverser. It detects if a file is a suitable testcase and if
   * so, adds it to the testsuite.
   * This class is used by the atkTestSuite. It should not be necessary to use
   * this class directly.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage test
   *
   */
  class atkTestCaseCollector
  {
    var $m_grouptest = null;

    function atkTestCaseCollector(&$grouptest)
    {
      $this->m_grouptest = &$grouptest;
    }

    function visitFile($fullpath)
    {
      $include = $_REQUEST["include"];
      $includefiles = explode("|",$include);
      $filename = basename($fullpath);
      if (substr($filename, 0, 11)=="class.test_" && (!$include ||($include && in_array($filename,$includefiles))) && strpos($fullpath,"/testcases/")!==false)
      {
        $elems = explode(".", $filename); // leads to 'class' 'test_...' and '.inc'
        $testclassname = $elems[1];
        include_once($fullpath);
        $this->m_grouptest->addTestClass($testclassname);
      }
    }

  }

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

      if (isset($config['test']))
      {
        // setup the test database
        $this->_setupTestDB();
      }

      $test = &new GroupTest("Full ATK test");
      $class = $_REQUEST['class'];
      $testclass = $this->getTestCaseLocationForClass($class);

      if ($class && file_exists(getClassPath($testclass)))
      {
        $this->_runTestCase($test,$testclass);
      }
      else
      {
        $this->_addTestsByTraversing($test,$module);
      }
      if($reporttype=="html")
      {
        $test->run(atknew("atk.test.atktestreporter"));
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
        $test->run(new TextReporter());
      }

      return $testresults;
    }


    /**
     * Setup the test database. Clones the database structure of the
     * default database to the test database. All data in the test
     * database will be lost!
     */
    function _setupTestDB()
    {
      $defaultDB = &atkGetDb('default');
      $testDB = &atkGetDb('test');

      $testDB->dropAll();

      $tables = $defaultDB->table_names();
      foreach($tables as $table)
      {
        $ddl = $testDB->createDDL();
        $metadata = $defaultDB->metadata($table["table_name"]);
        $ddl->loadMetaData($metadata);
        $query = $ddl->buildCreate();
        $testDB->query($query);
      }
    }

    function getTestCaseLocationForClass($class)
    {
      $exploded = explode('.',$class);
      $class = array_pop($exploded);
      return implode('.',$exploded).'.testcases.test_'.$class;
    }

    function _runTestCase(&$test,$testcase)
    {
      atkimport($testcase,true);
      $test->addTestClass(array_pop(explode(".", $testcase)));
    }

    function _addTestsByTraversing(&$test, $module)
    {
      $traverser = &atknew("atk.utils.atkdirectorytraverser");
      $traverser->addCallbackObject(new atkTestCaseCollector($test));
      if ($module!="")
      {
        if ($module=="atk") // make it possible to only test atk.
        {
          $dir = atkconfig("atkroot")."atk/";
        }
        //make it possible to run all tests from another dir than the root dir. EE.g. i case test.php isn't used.
        elseif ($module=="all")
        {
        	$dir = atkconfig("atkroot")."..";
        }
        else
        {
          $dir = moduleDir($module);
        }
        $traverser->traverse($dir);
      }
      else
      {
        $traverser->traverse(".");
        $this->_traverseExtraModules($traverser);
      }
    }

    /**
     * Traverse modules that are outside the root
     *
     * Here we actually change directories to all the module directories
     * then check if the directory is inside the application root.
     * If it is, we leave it alone, otherwise we traverse that too.
     *
     * @param object $traverser Object of the Traverser
     */
    function _traverseExtraModules(&$traverser)
    {
      global $g_modules;
      $curdir = getcwd();
      if (chdir(atkconfig('modulepath')))
      {
        $modulepath = getcwd();
        if (chdir($curdir))
        {
          foreach ($g_modules as $name => $path)
          {
            if (chdir($path))
            {
              $fullpath = getcwd();
              if (chdir($curdir))
              {
                if (substr($fullpath,0,strlen($curdir))!==$curdir)
                {
                  $traverser->traverse($path);
                }
              }
            }
          }
        }
      }
    }
  }

?>
