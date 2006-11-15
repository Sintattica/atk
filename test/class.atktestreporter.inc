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
   * @copyright (c)2005 Marcus Baker
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * This class is an extension to the default HtmlReporter of the SimpleTest
   * framework. The difference is that this reporter also displays succesful
   * passes. (Thanks to Marcus of http://www.lastcraft.com for the example
   * code).
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Marcus Baker <marcus@lastcraft.com>
   * @package atk
   * @subpackage utils
   *
   */
  class atkTestReporter extends HtmlReporter
  {
    function paintPass($message)
    {
      parent::paintPass($message);
      $passline = "<i>".date('Y-m-d H:i:s')."</i>";
      $passline.= "<span class=\"pass\">[PASS]</span> ";
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      $passline.= implode("-&gt;", $breadcrumb);
      $passline.= "-&gt;$message<br />\n";
      echo str_pad($passline, 4096);
      flush();
    }

    function paintFail($message)
    {
      SimpleReporter::paintFail($message);
      $failline = "<i>".date('Y-m-d H:i:s')."</i>";
      $failline.= "<b><span class=\"fail\">[FAIL]</span> ";
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      $failline.= implode("-&gt;", $breadcrumb);
      $failline.= "-&gt;$message</b><br />\n";
      echo str_pad($failline, 4096);
      flush();
    }

    function _getCss()
    {
      return '.fail {color: red; font-family: courier; } .pass { color: green; font-family: courier;}';
    }
  }
  
  /**
   * This class is an extension to the default HtmlReporter of the SimpleTest
   * framework. The difference is that this reporter also displays succesful
   * passes and returns an array with the tesresults. 
   *
   * @author Rene vd Ouden <rene@ibuildings.nl>
   *
   */
require_once(atkconfig("atkroot")."atk/test/simpletest/reporter.php");
 class ArrayHtmlReporter extends HtmlReporter
 {
 	var $_results;
 	function arrayHtmlReporter()
 	{
 	  $this->_results = array();
 	}
 	
 	function paintException($message)
 	{
 	  SimpleReporter::paintException($message);
      print "<span class=\"fail\">Exception</span>: ";
      $breadcrumb = $this->getTestList();
      array_shift($breadcrumb);
      print implode(" -&gt; ", $breadcrumb);
      print " -&gt; " . $message . "<br />\n";
 	}
 	
 	function paintPass($message)
 	{
 		SimpleReporter::paintPass($message);
		$breadcrumb = $this->getTestList();
		array_shift($breadcrumb);
		$test = implode("->", $breadcrumb);

		$result["time"] = date('Y-m-d H:i:s');
		$result["status"] = "Passed";
		$result["test"] = $test;
		$result["message"] = $message;
		$this->_results[] = $result;
		echo str_pad(nl2br("  [<span class=\"pass\">Passed</span>] ".$result['time']." | ".$result['test']." : ".$result['message'] ." \n"), 4096);
		flush();
 	}
 	
 	function paintFail($message)
 	{
 		SimpleReporter::paintFail($message);
		$breadcrumb = $this->getTestList();
		array_shift($breadcrumb);
		$test = implode("->", $breadcrumb);

		$result["time"] = date('Y-m-d H:i:s');
		$result["status"] = "Failed";
		$result["test"] = $test;
		$result["message"] = $message;
		$this->_results[] = $result;
		echo str_pad(nl2br("  [<span class=\"fail\">Failed</span>] ".$result['time']." | ".$result['test']." : ".$result['message'] ." \n"), 4096);
		flush();
 	}
 	
 	function _getCss()
    {
      return '.fail {color: red; font-family: courier; } .pass { color: green; font-family: courier;}';
    }
 	
 	function getStatus()
 	{
 		return $this->_results;
 	}
 }
 
/**
  * This class is an extension to the default SimpleReporter of the SimpleTest
  * framework. The difference is that this reporter also displays succesful
  * passes and returns an array with the tesresults. 
  *
  * @author Rene vd Ouden <rene@ibuildings.nl>
  *
  */
   class ArrayTextReporter extends SimpleReporter
   {
   	var $_results;
   	function arrayTextReporter()
   	{
   		$this->SimpleReporter();
   		$this->_results = array();
   	}
   	
   	function paintPass($message)
   	{
   		parent::paintPass($message);
		$breadcrumb = $this->getTestList();
		array_shift($breadcrumb);
		$test = implode("->", $breadcrumb);
		
		$result["time"] = date('Y-m-d H:i:s');
		$result["status"] = "Passed";
		$result["test"] = $test;
		$result["message"] = $message;
		$this->_results[] = $result;
		echo ("  [".$result['status']."] ".$result['time']." | ".$result['test']." : ".$result['message'] ." \n");
   	}
   	
   	function paintFail($message)
   	{
   		parent::paintFail($message);
		$breadcrumb = $this->getTestList();
		array_shift($breadcrumb);
		$test = implode("->", $breadcrumb);
		
		$result["time"] = date('Y-m-d H:i:s');
		$result["status"] = "Failed";
		$result["test"] = $test;
		$result["message"] = $message;
		$this->_results[] = $result;
		echo ("  [".$result['status']."] ".$result['time']." | ".$result['test']." : ".$result['message'] ." \n");
   	}
   	
   	function paintFooter($test_name)
   	{
   		echo $this->getTestCaseProgress() . "/" . $this->getTestCaseCount();
        echo " test cases complete:\n";
        echo $this->getPassCount() . " passes, ";
        echo $this->getFailCount() . " fails and ";
        echo $this->getExceptionCount() . " exceptions.";
   	}
   	
   	function getStatus()
   	{
   		return $this->_results;
   	}
  }

?>