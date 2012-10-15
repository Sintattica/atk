<?php
require_once './bootstrap.php';

class AllTests
{
  public static function suite()
  {
    $collector = new PHPUnit_Runner_IncludePathTestCollector(array(dirname(__FILE__)));
    $suite = new PHPUnit_Framework_TestSuite('Application');
    $suite->addTestFiles($collector->collectTests());
    return $suite;
  }
}