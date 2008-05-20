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
   * @version $Revision: 5149 $
   * $Id: class.atktestcasecollector.inc 5149 2007-08-08 15:59:28Z boy $
   */

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
    /**
     * Test method specific tester
     *
     * @var object
     */
    var $m_tester;

    function atkTestCaseCollector(&$tester)
    {
      $this->m_tester = &$tester;
    }

    function visitFile($fullpath)
    {
      $fullpath = atkCleanPath($fullpath);
      $filename = basename($fullpath);
      list( ,$testclassname) = explode(".", $filename);

      if ($this->isValidTestClass($fullpath,$filename, $testclassname))
      {
        if (isset($_REQUEST['list']))
        {
          $ret ='<tr><td>'.$fullpath.'</td><td><a href="'.$_SERVER['PHP_SELF'].'?showpassed&atkclass='.getClassName($fullpath).'">Test</a><br></td></tr>';
    	    echo $ret;
        }
        else
        {
          $this->addTestClass($fullpath,$testclassname);
        }
      }
    }

    /**
     * Test wether a file is a valid class that we want to execute
     *
     * @param string $fullpath Path of the file (/dir/dir/file.ext)
     * @param string $filename The filename (file.ext)
     * @param string $testclassname The name of the class (test_sumthin)
     * @return bool Valid test class?
     */
    function isValidTestClass($fullpath, $filename, $testclassname)
    {
      $include = atkArrayNvl($_REQUEST, "include");
      $includefiles = explode("|",$include);

      if (substr($filename, 0, 11)=="class.test_")
      {
        if (!$include ||($include && in_array($filename,$includefiles)))
        {
          if (strpos($fullpath,"/testcases/")!==false)
          {
            if (substr($filename, -4) == '.inc')
            {
              if (!$this->_isTestCaseInDisabledModule($fullpath))
              {
                return true;
              }
            }
          }
        }
      }
      return false;
    }

    /**
     * Checks wether a testcase is in a disbled module or not
     *
     * @todo I decomposed this from atkTestSuite, but I doubt very much
     *       that it actually works and even if it does it is implemented
     *       in an ugly way and should be rewritten.
     * @param string $fullpath The full path of the testcase
     * @return bool
     */
    function _isTestCaseInDisabledModule($fullpath)
    {
      $module_dirs = atkconfig("module_dirs");

      if (is_array($module_dirs) && count($module_dirs) > 0)
      {
        $testcase_in_module_dir = false;
        foreach ($module_dirs as $dir)
        {
          $dirBegin = substr($fullpath, 0, strlen($dir));
          if($dirBegin == $dir)
          {
            $testcase_in_module_dir = true;
            $module_array = explode("/", substr($fullpath, strlen($dir)));
            $module = $module_array[1];
            break;
          }
        }

        if ($testcase_in_module_dir && !moduleExists($module))
        {
          // testcase found but it is in a module that is not active
          return true;
        }
      }
      return false;
    }

    /**
     * Add a testclass to the tester to be tested.
     * Override per testmethod.
     *
     * @param string $fullpath Path of the testclass
     * @param string $testclassname
     * @abstract
     */
    function addTestClass($fullpath, $testclassname)
    {
      atkerror("Abstract method 'atkTestCaseCollector::addTestClass($fullpath,$testclassname)'called?");
    }
  }

?>