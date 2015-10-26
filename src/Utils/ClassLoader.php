<?php namespace Sintattica\Atk\Utils;

Use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Module;
use \ReflectionClass;

/**
 * Utility for importing and loading classes.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 *
 * @package atk
 * @subpackage utils
 */
class ClassLoader
{
    /**
     * Class path mounts.
     *
     * @var array
     */
    static $s_classPaths = array();

    /**
     * Is re-indexed during this request?
     *
     * @var boolean
     */
    static $s_isReindexed = false;

    /**
     * Imports a file
     * @param string $fullclassname Name of class in atkformat (map1.map2.classfile)
     * @param bool $failsafe If $failsafe is true (default), the class is required.  Otherwise, the
     *                                class is included.
     * @param bool $path Whether or not it is NOT an ATK classname
     *                                 ("map.class"), if true it will interpret classname
     *                                 as: "map/classname.php", default false.
     * @return bool whether the file we want to import was actually imported or not
     */
    static function import($fullclassname, $failsafe = true, $path = false)
    {
        $filename = ($path === false) ? self::getClassPath($fullclassname) : $fullclassname;

        static $alreadyHad = array();

        if (array_key_exists($filename, $alreadyHad) === true) {
            return $alreadyHad[$filename];
        } else {
            if (is_readable($filename) === true) {
                $alreadyHad[$filename] = true;

                if ($failsafe === true) {
                    require_once($filename);
                } else {
                    include_once($filename);
                }
                return true;
            } else {
                $alreadyHad[$filename] = false;
            }
        }

        return false;
    }

    /**
     * Clean-up the given path.
     *
     * @param string $path
     * @return string cleaned-up path
     *
     * @see http://nl2.php.net/manual/en/function.realpath.php (comment of 21st of September 2005)
     */
    static function cleanPath($path)
    {
        $result = array();

        $pathArray = explode('/', $path);
        if (empty($pathArray[0])) {
            $result[] = '';
        }

        foreach ($pathArray AS $key => $dir) {
            if ($dir == '..') {
                if (end($result) == '..' || !array_pop($result)) {
                    $result[] = '..';
                }
            } elseif ($dir && $dir != '.') {
                $result[] = $dir;
            }
        }

        if (!end($pathArray)) {
            $result[] = '';
        }

        return implode('/', $result);
    }

    /**
     * Mount a certain (class) path on a certain prefix. After this you can
     * simply load the classes on the given path using the prefix. This makes
     * it for example possible to load classes outside your atkroot.
     *
     * Note: at the moment only single element prefixes are allowed!
     *
     * Example:
     * ClassLoader::mountClassPath('frontend', Config::getGlobal('atkroot').'../frontend/');
     * ClassLoader::newInstance('frontend.helloworld');
     *
     * @param string $prefix prefix
     * @param string $path class path
     */
    static function mountClassPath($prefix, $path)
    {
        self::$s_classPaths[$prefix] = $path;
    }

    /**
     * Converts an ATK classname ("map1.map2.classname")
     * to a pathname ("/map1/map2/class.classname.php")
     * @param string $fullclassname ATK classname to be converted
     * @param bool $class is the file a class? defaults to true
     * @return string converted filename
     */
    static function getClassPath($fullclassname, $class = true)
    {
        $elems = explode(".", strtolower($fullclassname));
        if ($elems[0] == "module") {
            array_shift($elems);
            $prefix = Module::moduleDir(array_shift($elems));
        } else {
            if (isset(self::$s_classPaths[$elems[0]])) {
                $prefix = self::$s_classPaths[$elems[0]];
                array_shift($elems);
            } else {
                $prefix = Config::getGlobal("atkroot");
            }
        }

        $last = &$elems[count($elems) - 1];
        if ($class) {
            $last = "class." . $last . ".php";
        }

        $filename = $prefix . implode("/", $elems);

        return $filename;
    }

    /**
     * Converts a pathname ("/map1/map2/class.classname.php")
     * to an ATK classname ("map1.map2.classname")
     * @param string $classpath pathname to be converted
     * @param bool $class is the file a class? defaults to true
     * @return string converted filename
     */
    static function getClassName($classpath, $class = true)
    {
        $classpath = self::cleanPath($classpath);
        $elems = explode("/", strtolower($classpath));
        for ($counter = 0; $counter <= count($elems); $counter++) {
            if (isset($elems[$counter]) && $elems[$counter] === "../") {
                array_shift($elems);
            }
        }

        if ($class) {
            $last = &$elems[count($elems) - 1];
            $last = substr($last, 6, -4);
        } else {
            $last = &$elems[count($elems) - 2] . $elems[count($elems) - 1];
        }

        $classname = implode(".", $elems);

        return $classname;
    }

    /**
     * Returns a new instance of a class
     *
     * @param string $fullclassname the ATK classname of the class ("map1.map2.classname")
     * @param mixed ...            all arguments after the class name will be passed to the
     *                              class constructor
     *
     * @return object instance of the class
     */
    static function newInstance($fullclassname)
    {
        $args = func_get_args();
        array_shift($args);
        $args = array_values($args);
        return self::newInstanceArgs($fullclassname, $args);
    }

    /**
     * Returns a new instance of a class
     *
     * @param string $fullclassname the ATK classname of the class ("map1.map2.classname")
     * @param array $args arguments for the new instance
     *
     * @return object instance of the class
     */
    static function newInstanceArgs($fullclassname, $args = array())
    {
        $fullclassname = self::resolveClass($fullclassname);
        self::import($fullclassname, true);

        $elems = explode(".", strtolower($fullclassname));
        $classname = $elems[count($elems) - 1];

        if (strpos($classname, 'atk') === 0) {
            $classname = "" . substr($classname, 3);
        }

        if (class_exists($classname)) {
            if (count($args) === 0) {
                return new $classname();
            } else {
                $class = new ReflectionClass($classname);
                return $class->newInstanceArgs($args);
            }
        } else {
            Tools::atkerror("Class $fullclassname not found.");
            return null;
        }
    }

    /**
     * Resolve a classname to its final classname.
     *
     * An application can overload a class with a custom version. This
     * method resolves the initial classname to its overloaded version
     * (if any).
     *
     * @static
     * @param String $class The name of the class to resolve
     * @return String The resolved classname
     */
    static function resolveClass($class)
    {
        global $g_overloaders;
        if (isset($g_overloaders[$class])) {
            return $g_overloaders[$class];
        }

        return $class;
    }

    /**
     * Add a class overloader
     *
     * @static
     * @param String $original
     * @param String $overload
     * @param bool $overwrite
     * @return bool Wether or not we added the overloader
     */
    function addOverloader($original, $overload, $overwrite = true)
    {
        global $g_overloaders;
        if (!array_key_exists($original, $g_overloaders) || $overwrite) {
            $g_overloaders[$original] = $overload;
            return true;
        }
        return false;
    }

    /**
     * Remove a class overloader for a class
     *
     * @static
     * @param String $original
     * @return bool Wether or not we removed an overloader
     */
    function removeOverloader($original)
    {
        global $g_overloaders;
        if (array_key_exists($original, $g_overloaders)) {
            unset($g_overloaders[$original]);
            return true;
        }
        return false;
    }

    /**
     * Checks wether or not a class has an overloader defined
     *
     * @static
     * @param String $original The class to check for
     * @return bool Wether or not the class has an overloader
     */
    function hasOverloader($original)
    {
        global $g_overloaders;
        if (array_key_exists($original, $g_overloaders)) {
            return true;
        }
        return false;
    }

    /**
     * Invoke a method on a class based on a string definition.
     * The string must be in the format
     * "packagename.subpackage.classname#methodname"
     *
     * @static
     *
     * @param String $str The "classname#method" to invoke.
     * @param array $params Any params to be passed to the invoked method.
     *
     * @return boolean false if the call failed. In all other cases, it
     *                 returns the output of the invoked method. (be
     *                 careful with methods that return false).
     */
    public static function invokeFromString($str, $params = array())
    {
        if (strpos($str, "#") === false) {
            return false;
        }

        list($class, $method) = explode("#", $str);
        if ($class != "" && $method != "") {
            $handler = new $class();
            if (is_object($handler)) {
                return call_user_func_array(array($handler, $method), $params);
            }
            return false;
        } else {
            return false;
        }
    }

    /**
     * Finds a class in the current application.
     *
     * @param string $class The classname to find.
     *
     * @return string|bool   The classpath (Tools::atkimport( statement) of the class
     *                       if found, else false
     */
    function findClass($class)
    {
        if (strpos($class, "") === 0) {
            $class = "atk" . substr($class, 4);
        }

        $classloader = new ClassLoader();
        $classes = $classloader->getAllClasses();
        $class = strtolower($class);


        if (!in_array($class, array_keys($classes))) {
            if (Config::getGlobal('autoload_reindex_on_missing_class', false) && !self::$s_isReindexed) {
                $classes = $classloader->getAllClasses(true);
                self::$s_isReindexed = true;

                if (in_array($class, array_keys($classes))) {
                    return $classes[$class];
                }
            }

            return false;
        }

        return $classes[$class];
    }

    /**
     * Gets an array with all the the classes
     *
     * @param bool $force Force reloading of classes, instead of using cache.
     * @return Array An array with the classes as keys and the path as value.
     */
    function getAllClasses($force = false)
    {
        static $s_classes = array();

        if (empty($s_classes) || $force) {
            $cache = new TmpFile('classes.inc.php');
            $classes = array();

            if (is_readable($cache->getPath())) {
                include($cache->getPath());
            }

            if (empty($classes) || $force) {
                $classes = $this->findAllClasses();
                $cache->writeAsPhp('classes', $classes);
            }
            $s_classes = $classes;
        }
        return $s_classes;
    }

    /**
     * Find all classes in ATK.
     *
     * @todo Make it search and support modules too.
     *
     * @return Array An array with the classes as keys and the path as value.
     */
    function findAllClasses()
    {
        $traverser = new DirectoryTraverser();
        $classfinder = new ClassFinder();
        $traverser->addCallbackObject($classfinder);
        $cwd = getcwd();
        chdir(Config::getGlobal('atkroot'));
        $traverser->traverse('atk');
        chdir($cwd);
        $classes = $classfinder->getClasses();
        Tools::atkdebug("ClassLoader::findAllClasses(): Found " . count($classes) . ' classes');
        return $classes;
    }

}

/**
 * Find all files that might be classes.
 *
 * Made for use with the directory traverser.
 *
 * @author Boy Baukema <boy@achievo.org>
 *
 * @package atk
 * @subpackage utils
 */
class ClassFinder
{
    var $m_classes = array();

    function visitFile($file)
    {
        $filename = basename($file);
        if (substr($filename, 0, 6) === 'class.' && substr($filename, -4) === '.php') {
            $name = substr($filename, 6, -4);
            $this->m_classes[$name] = Tools::getClassName($file);
        }
    }

    /**
     * Returns all the found classes as keys with their classpath (Tools::atkimport( statement)
     * as value.
     *
     * @return array The found classes with classpatsh
     */
    function getClasses()
    {
        return $this->m_classes;
    }

}
