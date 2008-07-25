<?php

/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage console
 *
 * @copyright (c)2000-2008 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * ATK console controller base class. Useful for creating command line
 * scripts. Has out of the box support for key/value parameters and
 * supports multiple actions that can be handled by a single controller.
 */

class atkConsoleController
{

  /**
   * Constructor
   */
  public function __construct()
  {
  }

  /**
   * Reads arguments from the given $argv parameter (if null given uses
   * $_SERVER['argv']). The first argument should be the full ATK class
   * name of the console controller, the (optional) second argument should
   * contain the action name (defaults to default if none given) any
   * following argument should be in the form key=value and should contain
   * the parameters for the controller action.
   *
   * Some examples:
   * console.php module.example.console.examplecontroller default output="Hello World"
   * console.php module.example.console.examplecontroller output="Hello World"
   *
   * Both of these examples instantiate the ExampleController class and call the
   * defaultAction method (all action methods should be in the form <action>Action).
   * Parameters will be passed as key value array to the action method.
   *
   * @param string|array $argv either an argument string or array of arguments
   *
   * @return void
   */

  public static function run($argv = null)
  {

    $className;
    $actionName;
    $paramArray = array();

    if($argv==null)
    {
      $argv = $_SERVER['argv'];
    }
    else
    {
      $className = $argv[1];
      if((stripos($argv[2],"=")) || ($argv[2]===null))
      {
        $actionName="default";
        $firstParam = explode("=", $argv[2]);
        $paramArray[$firstParam[0]] = $firstParam[1];
      }
      else
      {
        $actionName = $argv[2];
      }
      for($i=3;$i<sizeof($argv);$i++)
      {
        $tempParam = explode("=", $argv[$i]);
        $paramArray[$tempParam[0]] = $tempParam[1];
      }

    }

    $consoleClass = atknew($className);
    $funcName = $actionName."Action";
    $consoleClass->$funcName($paramArray);

  }

  /**
   * Useful method for outputting log data to a log file. Files should be placed
   * in the ATK temp directory in a subdirectory called console/. Each file should
   * have the following name <controller>_<yyyymmdd>_info.log. The controller part
   * should be replaced by a lower case version of the controller class name, the
   * yyyymmdd should be replaced by the current date.
   *
   * If the console directory doesn't exist yet inside the ATK temp directory it
   * should be created automatically.
   *
   * @param string $message info message
   * @param mixed  $data    data that should be logged (will be outputted using print_r)
   */
  
  protected function info($message, $data=null)
  {
    $tempDir = atkconfig(atktempdir);

    $controllerName = strtolower(get_class($this));

    mkdir($tempDir."console",0777);
    $today = date("Ymd");
    $lcn = $tempDir."/console//".$controllerName."_".$today."_info.log";

    $fhandle = fopen($lcn, "a");

    $results = print_r($data, true);

    if(fwrite($fhandle, $message."|Data:".$results."\r\n") === FALSE)
    {
      echo "Error writing Console Log File";
    }
  }

  /**
   * Useful method for outputting error data to a log file. Files should be placed
   * in the ATK temp directory in a subdirectory called console/. Each file should
   * have the following name <controller>_<yyyymmdd>_error.log. The controller part
   * should be replaced by a lower case version of the controller class name, the
   * yyyymmdd should be replaced by the current date.
   *
   * If the console directory doesn't exist yet inside the ATK temp directory it
   * should be created automatically.
   *
   * @param string $message error message
   * @param mixed  $data    data that should be logged (will be outputted using print_r)
   */
  
  protected function error($message, $data=null)
  {
    $tempDir = atkconfig("atktempdir");

    $controllerName = strtolower(get_class($this));

    mkdir($tempDir."console",0777);
    $today = date("Ymd");
    $lcn = $tempDir."/console//".$controllerName."_".$today."_error.log";

    $fhandle = fopen($lcn, "a");

    $results = print_r($data, true);

    if(fwrite($fhandle, $message."|Data:".$results."\r\n") === FALSE)
    {
      echo "Error writing Console Log File";
    }
  }
}