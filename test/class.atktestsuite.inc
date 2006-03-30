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
      if (substr($filename, 0, 11)=="class.test_" && (!$include ||($include && in_array($filename,$includefiles))))
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
      $test = &new GroupTest("Full ATK test");

      $traverser = &atknew("atk.utils.atkdirectorytraverser");
      $traverser->addCallbackObject(new atkTestCaseCollector($test));
      
      if ($module!="")
      {        
        if ($module=="atk") // make it possible to only test atk.
        {
          $dir = atkconfig("atkroot")."atk/";
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
        $this->_traverseExtraModules();
      }
      if($reporttype=="html")
      {
        $test->run(atknew("atk.test.atktestreporter"));
      }
      else
      {
        $test->run(new TextReporter());
      }
    }
    
    /**
     * Traverse modules that are outside the root
     * 
     * Here we actually change directories to all the module directories
     * then check if the directory is inside the application root.
     * If it is, we leave it alone, otherwise we traverse that too.
     */
    function _traverseExtraModules()
    {
      global $g_modules;
      $curdir = getcwd();
      if ($chdir)
      {
        $modulepath = getcwd();
        if ($chdir)
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