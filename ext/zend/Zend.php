<?php
/**
 * Zend Framework
 *
 * LICENSE
 *
 * This source file is subject to the new BSD license that is bundled
 * with this package in the file LICENSE.txt.
 * It is also available through the world-wide-web at this URL:
 * http://framework.zend.com/license/new-bsd
 * If you did not receive a copy of the license and are unable to
 * obtain it through the world-wide-web, please send an email
 * to license@zend.com so we can send you a copy immediately.
 *
 * @category   Zend
 * @package    Zend
 * @copyright  Copyright (c) 2006 Zend Technologies USA Inc. (http://www.zend.com)
 * @license    http://framework.zend.com/license/new-bsd     New BSD License
 */


/**
 * Zend_Exception
 */
require_once 'Zend/Exception.php';


/**
 * Utility class for common functions.
 *
 * @category   Zend
 * @package    Zend
 * @copyright  Copyright (c) 2006 Zend Technologies USA Inc. (http://www.zend.com)
 * @license    http://framework.zend.com/license/new-bsd     New BSD License
 */
final class Zend
{
    /**
     * Object registry provides storage for shared objects
     * @var array
     */
    static private $_registry = array();


    /**
     * Singleton Pattern
     */
    private function __construct()
    {}


    /**
     * Loads a class from a PHP file.  The filename must be formatted
     * as "$class.php".
     *
     * If $dirs is a string or an array, it will search the directories
     * in the order supplied, and attempt to load the first matching file.
     *
     * If $dirs is null, it will split the class name at underscores to
     * generate a path hierarchy (e.g., "Zend_Example_Class" will map
     * to "Zend/Example/Class.php").
     *
     * If the file was not found in the $dirs, or if no $dirs were specified,
     * it will attempt to load it from PHP's include_path.
     *
     * @param string $class
     * @param string $dirs
     * @throws Zend_Exception
     * @return void
     */
    static public function loadClass($class, $dirs = null)
    {
        if (class_exists($class, false)) {
            return;
        }

        // autodiscover the path from the class name
        $path = str_replace('_', DIRECTORY_SEPARATOR, $class);
        if ($dirs === null && $path != $class) {
            // use the autodiscovered path
            $dirs = dirname($path);
            $file = basename($path) . '.php';
        } else {
            $file = $class . '.php';
        }

        self::loadFile($file, $dirs, true);

        if (!class_exists($class, false)) {
            throw new Zend_Exception("File \"$file\" was loaded "
                               . "but class \"$class\" was not found within.");
        }
    }


    /**
     * Loads an interface from a PHP file.  The filename must be formatted
     * as "$interface.php".
     *
     * If $dirs is a string or an array, it will search the directories
     * in the order supplied, and attempt to load the first matching file.
     *
     * If $dirs is null, it will split the interface name at underscores to
     * generate a path hierarchy (e.g., "Zend_Example_Interface" will map
     * to "Zend/Example/Interface.php").
     *
     * If the file was not found in the $dirs, or if no $dirs were specified,
     * it will attempt to load it from PHP's include_path.
     *
     * @param string $interface
     * @param string $dirs
     * @throws Zend_Exception
     * @return void
     */
    static public function loadInterface($interface, $dirs = null)
    {
        if (interface_exists($interface, false)) {
            return;
        }

        // autodiscover the path from the interface name
        $path = str_replace('_', DIRECTORY_SEPARATOR, $interface);
        if ($dirs === null && $path != $interface) {
            // use the autodiscovered path
            $dirs = dirname($path);
            $file = basename($path) . '.php';
        } else {
            $file = $interface . '.php';
        }

        self::loadFile($file, $dirs, true);

        if (!interface_exists($interface, false)) {
            throw new Zend_Exception("File \"$file\" was loaded "
                               . "but interface \"$interface\" was not found within.");
        }
    }


    /**
     * Loads a PHP file.  This is a wrapper for PHP's include() function.
     *
     * $filename must be the complete filename, including any
     * extension such as ".php".  Note that a security check is performed that
     * does not permit extended characters in the filename.  This method is
     * intended for loading Zend Framework files.
     *
     * If $dirs is a string or an array, it will search the directories
     * in the order supplied, and attempt to load the first matching file.
     *
     * If the file was not found in the $dirs, or if no $dirs were specified,
     * it will attempt to load it from PHP's include_path.
     *
     * If $once is TRUE, it will use include_once() instead of include().
     *
     * @param  string        $filename
     * @param  string|null   $directory
     * @param  boolean       $once
     * @throws Zend_Exception
     * @return void
     */
    static public function loadFile($filename, $dirs=null, $once=false)
    {
        // security check
        if (preg_match('/[^a-z0-9\-_.]/i', $filename)) {
            throw new Zend_Exception('Security check: Illegal character in filename');
        }

        /**
         * Determine if the file is readable, either within just the include_path
         * or within the $dirs search list.
         */
        $filespec = $filename;
        if ($dirs === null) {
            $found = self::isReadable($filespec);
        } else {
            foreach ((array)$dirs as $dir) {
                $filespec = rtrim($dir, '\\/') . DIRECTORY_SEPARATOR . $filename;
                $found = self::isReadable($filespec);
                if ($found) {
                    break;
                }
            }
        }

        /**
         * Throw an exception if the file could not be located
         */
        if (!$found) {
            throw new Zend_Exception("File \"$filespec\" was not found.");
        }

        /**
         * Attempt to include() the file.
         *
         * include() is not prefixed with the @ operator because if
         * the file is loaded and contains a parse error, execution
         * will halt silently and this is difficult to debug.
         *
         * Always set display_errors = Off on production servers!
         */
        if ($once) {
            include_once($filespec);
        } else {
            include($filespec);
        }
    }


    /**
     * Returns TRUE if the $filename is readable, or FALSE otherwise.  This
     * function uses the PHP include_path, where PHP's is_readable() does not.
     *
     * @param string $filename
     * @return boolean
     */
    static public function isReadable($filename)
    {
        $f = @fopen($filename, 'r', true);
        $readable = is_resource($f);
        if ($readable) {
            fclose($f);
        }
        return $readable;
    }


    /**
     * Debug helper function.  This is a wrapper for var_dump() that adds
     * the <pre /> tags, cleans up newlines and indents, and runs
     * htmlentities() before output.
     *
     * @param  mixed  $var The variable to dump.
     * @param  string $label An optional label.
     * @return string
     */
    static public function dump($var, $label=null, $echo=true)
    {
        // format the label
        $label = ($label===null) ? '' : rtrim($label) . ' ';

        // var_dump the variable into a buffer and keep the output
        ob_start();
        var_dump($var);
        $output = ob_get_clean();

        // neaten the newlines and indents
        $output = preg_replace("/\]\=\>\n(\s+)/m", "] => ", $output);
        if (PHP_SAPI == 'cli') {
            $output = PHP_EOL . $label
                    . PHP_EOL . $output 
                    . PHP_EOL;
        } else {
            $output = '<pre>'
                    . $label
                    . htmlentities($output, ENT_QUOTES)
                    . '</pre>';
        }

        if ($echo) {
            echo($output);
        }
        return $output;
    }


    /**
     * Registers a shared object.
     *
     * @todo use SplObjectStorage if ZF minimum PHP requirement moves up to at least PHP 5.1.0
     *
     * @param   string      $name The name for the object.
     * @param   object      $obj  The object to register.
     * @throws  Zend_Exception
     * @return  void
     */
    static public function register($name, $obj)
    {
        if (!is_string($name)) {
            throw new Zend_Exception('First argument $name must be a string.');
        }

        // don't register the same name twice
        if (array_key_exists($name, self::$_registry)) {
           throw new Zend_Exception("Object named '$name' already registered.  Did you mean to call registry()?");
        }

        // only objects may be stored in the registry
        if (!is_object($obj)) {
           throw new Zend_Exception("Only objects may be stored in the registry.");
        }

        $e = '';
        // an object can only be stored in the registry once
        foreach (self::$_registry as $dup=>$registeredObject) {
            if ($obj === $registeredObject) {
                $e = "Duplicate object handle already exists in the registry as \"$dup\".";
                break;
            }
        }

        /**
         * @todo throwing exceptions inside foreach could cause leaks, use a workaround
         *       like this until a fix is available
         *
         * @link http://bugs.php.net/bug.php?id=34065
         */
        if ($e) {
            throw new Zend_Exception($e);
        }

        self::$_registry[$name] = $obj;
    }


    /**
     * Retrieves a registered shared object, where $name is the
     * registered name of the object to retrieve.
     *
     * If the $name argument is NULL, an array will be returned where 
	 * the keys to the array are the names of the objects in the registry 
	 * and the values are the class names of those objects.
     *
     * @see     register()
     * @param   string      $name The name for the object.
     * @throws  Zend_Exception
     * @return  object      The registered object.
     */
    static public function registry($name=null)
    {
        if ($name === null) {
            $registry = array();
            foreach (self::$_registry as $name=>$obj) {
                $registry[$name] = get_class($obj);
            }
            return $registry;
        }

        if (!is_string($name)) {
            throw new Zend_Exception('First argument $name must be a string, or null to list registry.');
        }

        if (!array_key_exists($name, self::$_registry)) {
           throw new Zend_Exception("No object named \"$name\" is registered.");
        }

        return self::$_registry[$name];
    }

    
    /**
     * Returns TRUE if the $name is a named object in the
     * registry, or FALSE if $name was not found in the registry.
     *
     * @param  string $name
     * @return boolean
     */
    static public function isRegistered($name)
    {
        return isset(self::$_registry[$name]);
    }
}
