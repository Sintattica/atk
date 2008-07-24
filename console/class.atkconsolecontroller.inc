<?php

/**
* ATK console controller base class. Useful for creating command line
* scripts. Has out of the box support for key/value parameters and
* supports multiple actions that can be handled by a single controller.
*/

include_once('..\..\defaultconfig.inc.php');
 
class atkConsoleController
{
	
	protected $controllerName;
 /**
  * Constructor
  */
	protected function __construct($cName)
	{
	 	$this->controllerName=$cName;
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
  
	public static function run($argv=null)
	{
		
		$className;
		$actionName;
		$paramArray = array();
		
		if($argv==null)
		{
			$argv = $_SERVER['argv'];	
		}else
		{
			$className = $argv[1];			
			if((stripos($argv[2],"=")) OR ($argv[2]===null)){
				$actionName="default";								
				$firstParam = explode("=", $argv[2]);
				$paramArray[$firstParam[0]]=$firstParam[1];							
			}else{
				$actionName=$argv[2];
			}
			for($i=3;$i<sizeof($argv);$i++){
				$tempParam = explode("=", $argv[$i]);
				$paramArray[$tempParam[0]]=$tempParam[1];
			}					
		
		}
			
		$incStr="../../";
		$incPath=explode(".",$className);
		$className=array_pop($incPath);
		for($i=0;$i<sizeof($incPath);$i++)
		{
			$incStr .= $incPath[$i]."/";	
		}
		$incStr = $incStr."class.".$className.".inc";
		include_once($incStr);
		
		$consoleClass = new $className($className);
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
		//echo "Temp Directory:".$config_atktempdir; // This doesn't work,var no value, so below direct code needs to be modified
	
		mkdir("..\..\atktmp\console",0777);	
		$today = date("Ymd");		
		$lcn = "..\\..\\atktmp\\console\\".$this->controllerName."_".$today."_info.log"; 	
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
		//echo "Temp Directory:".$config_atktempdir; // This doesn't work,var no value, so below direct code needs to be modified
	
		mkdir("..\..\atktmp\console",0777);	
		$today = date("Ymd");		
		$lcn = "..\\..\\atktmp\\console\\".$this->controllerName."_".$today."_error.log";
		$fhandle = fopen($lcn, "a");
		
		$results = print_r($data, true);
		
		if(fwrite($fhandle, $message."|Data:".$results."\r\n") === FALSE) 
		{
    	echo "Error writing Console Log File";
    }
	}
} 