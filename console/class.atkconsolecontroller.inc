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
 * @copyright (c)2008 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 */

/**
 * ATK console controller base class. Useful for creating command line
 * scripts. Has out of the box support for key/value parameters and
 * supports multiple actions that can be handled by a single controller.
 * 
 * @package atk
 * @subpackage console
 */
class atkConsoleController
{
  /**
   * Controller name.
   *
   * @var string
   */
  private $m_name;
  
  /**
   * Debug enabled?
   * 
   * @var bool
   */
  private $m_debug = false;
  
  /**
   * Constructor.
   */
  public function __construct()
  {
    $this->m_name = strtolower(get_class($this));
  }
  
  /**
   * Returns the controller name.
   */
  protected function getName()
  {
    return $this->m_name;
  }
  
  /**
   * Enabled/disable debug mode.
   * 
   * @param bool $enable enable debug mode?
   */
  protected function setDebugEnabled($enable)
  {
    $this->m_debug = $enable;
  }
  
  /**
   * Is debugging enabled?
   * 
   * @return bool debugging enabled?
   */
  protected function isDebugEnabled()
  {
    return $this->m_debug;
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
   * console.php modules/example/console/class.examplecontroller.inc output="Hello World"
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
    $params = array();
    
    if ($argv == null)
    {
      $argv = $_SERVER['argv'];
    }
    
    $class = $argv[1];
    if ((stripos($argv[2], "=")) || ($argv[2] === null))
    {
      $action = "default";
      list($key, $value) = explode("=", $argv[2]);
      $params[$key] = $value;
    }
    else
    {
      $action = $argv[2];
    }
    
    for ($i = 3; $i < count($argv); $i++)
    {
      if (strpos($argv[$i], "=") !== false)
      {
        list($key, $value) = explode("=", $argv[$i]);
        $params[$key] = $value;
      }
      else
      {
        $params[] = $argv[$i];
      }
    }

    if (preg_match("!class.([^.]+).inc$!", $class, $matches)) // user supplied path relative to script
    {
      include_once($class);
      $controller = new $matches[1]();
    }
    else if (atkimport($class)) // user supplied ATK class path
    {
      $controller = atknew($class);
    }
    else 
    {
      die('Unknown console controller "'.$class.'".'."\n");
    }

    $controller->executeAction($action, $params);
  }
  
  /**
   * Tries to execute the given action.
   *
   * @param string $action action name
   * @param array  $params action parameters
   */
  protected function executeAction($action, $params)
  {
    // translate dashes to underscores so we can support actions like --list
    $method = str_replace('-', '_', $action)."Action";
    
    if (!method_exists($this, $method))
    {
      echo "Unknown action {$action} for controller ".$this->getName().".\n";
      die;
    }
    
    $this->$method($params);
  }

  /**
   * Useful method for outputting log data to a log file. 
   * 
   * Log files are be placed in the ATK temp directory in a subdirectory 
   * called console/. Each file uses the naming convention <controller>_<yyyymmdd>.log.
   * The controller part is replaced by a lower case version of the controller class name
   * and the yyyymmdd part is replaced by the current date.
   *
   * If the console directory doesn't exist yet inside the ATK temp directory it's
   * created automatically.
   *
   * @param string $message info message
   * @param mixed  $data    data that should be logged (optional)
   */
  public function info($message, $data=null)
  {
    $this->log('info', $message, $data);
  }

  /**
   * Useful method for outputting error data to a log file. 
   * 
   * Log files are be placed in the ATK temp directory in a subdirectory 
   * called console/. Each file uses the naming convention <controller>_<yyyymmdd>.log.
   * The controller part is replaced by a lower case version of the controller class name
   * and the yyyymmdd part is replaced by the current date.
   *
   * If the console directory doesn't exist yet inside the ATK temp directory it's
   * created automatically.
   *
   * @param string $message error message
   * @param mixed  $data    data that should be logged (optional)
   */
  public function error($message, $data=null)
  {
    $this->log('error', $message, $data);
  }
  
  /**
   * Internal logging method.
   * 
   * Log files are be placed in the ATK temp directory in a subdirectory 
   * called console/. Each file uses the naming convention <controller>_<yyyymmdd>_<type>.log.
   * The controller part is replaced by a lower case version of the controller class name, the
   * yyyymmdd part is replaced by the current date and the type is replaced by the value of
   * the $type parameter.
   *
   * @param string $type    type (max 5 chars)
   * @param string $message message
   * @param mixed  $data    optional data
   */
  protected function log($type, $message, $data)
  {
    $filename = "console/".$this->getName()."_".date("Ymd").".log";
    
    $type = substr($type, 0, 5);
    
    $lines = "[".date("Y-m-d H:i:s")."] [$type] ".str_repeat(" ", 5 - strlen($type))."{$message}\n";
    if ($data != null)
    {
      $dump = print_r($data, true);
      foreach (explode("\n", $dump) as $line)
      {
        $lines .= str_repeat(" ", 30).$line."\n";
      }
    }
    
    atkimport('atk.utils.atktmpfile');
    $file = new atkTmpFile($filename);
    $file->appendToFile($lines);
    
    if ($this->isDebugEnabled())
    {
      echo $lines;
      flush();
    }
  }
  
  /**
   * Lists all the action methods. Useful for exploring controllers. You can access this
   * method by using --list as the console controller action.
   */
  public function __listAction()
  {
    echo "Actions for ".$this->getName().":\n\n";
    
    $ref = new ReflectionObject($this);
    foreach ($ref->getMethods(ReflectionMethod::IS_PUBLIC) as $method)
    {
      if (preg_match("/(.+?)Action/i", $method->getName(), $matches))
      {
        $action = $matches[1];
        if (substr($action, 0, 2) == '__') continue;
        $comment = $method->getDocComment();
        
        echo "$action:\n";
        
        $lines = '';
        foreach (explode("\n", $comment) as $line)
        {
          if (!preg_match('/@[a-z]+/', $line))
          {
            $lines .= preg_replace("!^\s*/?\*+/?\s*!", "", $line)."\n";
          }
        }
        
        $lines = trim($lines, " \r\n");
        foreach (explode("\n", $lines) as $line)
        {
          echo "    {$line}\n";
        }
        
        echo "\n";
      }
    }
  }
  
  /**
   * Executes the action with logging on the console. You can access this
   * method by using --debug as the console controller action followed by the
   * normal action name (or none if default) and parameters.
   * 
   * @param array $params
   */
  public function __debugAction($params)
  {
    $this->setDebugEnabled(true);
    
    reset($params);
    $key = key($params);
    
    if (is_numeric($key))
    {
      $action = $params[$key];
      unset($params[$key]);
    }
    else 
    {
      $action = 'default';
    }
    
    $this->executeAction($action, $params);
  }
}