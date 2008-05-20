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
   * $Id: class.atktesttraverser.inc 5149 2007-08-08 15:59:28Z boy $
   */

  /**
   * Traverses through a structure in search of testcases
   *
   * @author Boy Baukema <boy@achievo.org>
   * @package atk
   * @subpackage test
   */
  class atkTestTraverser
  {
    /**
     * @var atkTestCaseCollector
     */
    var $m_collector;

    /**
     * Traverses through a structure in search of testcases.
     * Leaves checking of files and adding of testcases to
     * visitor.
     *
     * @param atkTestCaseCollector $collector Visitor that checks
     *                                        files and adds testcases
     * @return atkTestTraverser
     */
    function atkTestTraverser($collector)
    {
      $this->m_collector = $collector;
    }

    /**
     * @param atkTestCaseCollector $collector
     * @return atkTestTraverser
     */
    function create($collector)
    {
      return new self($collector);
    }

    /**
     * Starts the process of searching and adding tests
     * The main public method.
     *
     * @param unknown_type $module
     */
    function addTestsByTraversing($module="")
    {
      $traverser = &atknew("atk.utils.atkdirectorytraverser");
      $traverser->addCallbackObject($this->m_collector);

      if ($module) $this->_traverse($traverser, $this->_getDirectoryForModule($module), false);
      else         $this->_traverse($traverser);
    }

    /**
     * Gets the directory (to traverse) for a module.
     * Knows the special modules 'atk' and 'all'
     *
     * @param string $module
     * @return string
     */
    function _getDirectoryForModule($module)
    {
      if     ($module=="atk") $dir = atkconfig("atkroot")."atk/";
      elseif ($module=="all") $dir = atkconfig("atkroot")."..";
      else
      {
        $dir = moduleDir($module);
        if (($dir == "") && (in_array($module, atkconfig("module_dirs"))))
        {
          $dir = $module;
        }
      }
      return $dir;
    }

    function _traverse($traverser, $dir='.', $extramods=true)
    {
      $this->_preTraverse();

      $traverser->traverse($dir);
      if ($extramods) $this->_traverseExtraModules($traverser);

      $this->_postTraverse();
    }

    function _preTraverse()
    {
      if (isset($_REQUEST['list']))
      {
        $ret = "<h1>Testcase list ($dir)</h1>";
    	  $ret .='<table border="1" cellspacing="0" cellpadding="5">';
    	  $ret .='<tr><th>Path</th><th>&nbsp;</th></tr>';
      	echo $ret;
      }
    }

    function _postTraverse()
    {
      if (isset($_REQUEST['list']))
      {
    	  $ret ='</table>';
      	echo $ret;
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
      if (chdir(atkconfig('module_path')))
      {
        if (chdir($curdir))
        {
          foreach ($g_modules as $path)
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
