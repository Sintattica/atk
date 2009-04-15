<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6320 $
   * $Id$
   */

  /**
   * Utility for importing and loading classes.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @package atk
   * @subpackage utils
   */
  class atkClassLoader
  {
    /**
     * Class path mounts.
     *
     * @var array
     */
    static $s_classPaths = array();
    
    /**
     * Imports a file
     * @param string $fullclassname Name of class in atkformat (map1.map2.classfile)
     * @param bool   $failsafe      If $failsafe is true (default), the class is required.  Otherwise, the
     *                                class is included.
     * @param bool   $path          Whether or not it is NOT an ATK classname
     *                                 ("map.class"), if true it will interpret classname
     *                                 as: "map/class.classname.inc", default false.
     * @return bool whether the file we want to import was actually imported or not
     */
    static function import($fullclassname, $failsafe=true, $path = false)
    {
      $filename = ($path === false) ? self::getClassPath($fullclassname) : $fullclassname;
  
      static $alreadyHad = array();
  
      if (array_key_exists($filename, $alreadyHad) === true)
      {
        return $alreadyHad[$filename];
      }
      else 
      {
        if (is_readable($filename) === true)
        {
          $alreadyHad[$filename] = true;
          
          if ($failsafe === true)
          {
            require_once($filename);
          }
          else
          {
            include_once($filename);
          }
          return true;
        }
        else 
        {
          $alreadyHad[$filename] = false;
        }
      }
  
      return false;
    }    
    
    /**
     * Clean-up the given path.
     *
     * @param string $path
     * @return cleaned-up path
     *
     * @see http://nl2.php.net/manual/en/function.realpath.php (comment of 21st of September 2005)
     */
    static function cleanPath($path)
    {
      $result = array();
  
      $pathArray = explode('/', $path);
      if (empty($pathArray[0]))
        $result[] = '';
  
      foreach ($pathArray AS $key => $dir)
      {
        if ($dir == '..')
        {
          if (end($result) == '..' || !array_pop($result))
          {
            $result[] = '..';
          }
        }
        elseif ($dir && $dir != '.')
        {
          $result[] = $dir;
        }
      }
  
      if (!end($pathArray))
        $result[] = '';
  
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
     * atkClassLoader::mountClassPath('frontend', atkconfig('atkroot').'../frontend/');
     * atkClassLoader::newInstance('frontend.helloworld');
     *
     * @param string $prefix prefix
     * @param string $path   class path
     */
    static function mountClassPath($prefix, $path)
    {
      self::$s_classPaths[$prefix] = $path;
    }
  
    /**
     * Converts an ATK classname ("map1.map2.classname")
     * to a pathname ("/map1/map2/class.classname.inc")
     * @param string $fullclassname  ATK classname to be converted
     * @param bool $class            is the file a class? defaults to true
     * @return string converted filename
     */
    static function getClassPath($fullclassname, $class = true)
    {
      $elems = explode(".", strtolower($fullclassname));
      if($elems[0] == "module")
      {
        array_shift($elems);
        $prefix = moduleDir(array_shift($elems));
      }
      else if (isset(self::$s_classPaths[$elems[0]]))
      {
        $prefix = self::$s_classPaths[$elems[0]];
        array_shift($elems);                
      }
      else
      {
       $prefix = atkconfig("atkroot");
      }
  
      $last = &$elems[count($elems)-1];
      if ($class) {$last = "class.".$last.".inc";}
  
      $filename = $prefix.implode("/", $elems);
  
      return $filename;
    }
  
    /**
     * Converts a pathname ("/map1/map2/class.classname.inc")
     * to an ATK classname ("map1.map2.classname")
     * @param string $classpath pathname to be converted
     * @param bool $class       is the file a class? defaults to true
     * @return string converted filename
     */
    static function getClassName($classpath, $class=true)
    {
      $classpath = self::cleanPath($classpath);
      $elems = explode("/", strtolower($classpath));
      for($counter=0; $counter<=count($elems);$counter++)
      {
        if(isset($elems[$counter]) && $elems[$counter] == "../")
        {
          array_shift($elems);
        }
      }
  
      if($class)
      {
        $last = &$elems[count($elems)-1];
        $last = substr($last, 6, -4);
      }
      else
      {
        $last = &$elems[count($elems)-2].$elems[count($elems)-1];
      }
  
      $classname = implode(".", $elems);
  
      return $classname;
    }
  
    /**
     * Returns a new instance of a class
     * 
     * @param string $fullclassname the ATK classname of the class ("map1.map2.classname")
     * @param mixed  ...            all arguments after the class name will be passed to the
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
     * @param array  $args          arguments for the new instance
     * 
     * @return object instance of the class
     */
    static function newInstanceArgs($fullclassname, $args=array())
    {
      $fullclassname = self::resolveClass($fullclassname);
      self::import($fullclassname, true);
  
      $elems = explode(".", strtolower($fullclassname));
      $classname = $elems[count($elems)-1];
  
      if (class_exists($classname))
      {
        if (count($args) == 0)
        {
          return new $classname();
        }
        else 
        {
          $class = new ReflectionClass($classname);      
          return $class->newInstanceArgs($args);
        }
      }
      else
      {
        atkerror("Class $fullclassname not found.");
        return null;
      }
    }    
  
    /**
     * Return a singleton instance of the specified class.
     *
     * This works for all singletons that implement the getInstance() method.
     *
     * @param string $fullclassname the ATK classname of the class ("map1.map2.classname")
     * @param bool $reset Force resetting of the instance
     * @return obj instance of the class
     * */
    static function getSingletonInstance($fullclassname, $reset=false)
    {
      static $s_instances = array();
      $fullclassname = self::resolveClass($fullclassname);
      if (!isset($s_instances[$fullclassname]) || $reset)
      {
        self::import($fullclassname);
        $elems = explode(".", strtolower($fullclassname));
        $classname = $elems[count($elems)-1];
        atkdebug("Getting singleton instance $fullclassname");
        eval("\$s_instances[\$fullclassname] = &".$classname."::getInstance($reset);");
      }
      return $s_instances[$fullclassname];
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
      if (isset($g_overloaders[$class])) return $g_overloaders[$class];
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
    function addOverloader($original, $overload, $overwrite=true)
    {
      global $g_overloaders;
      if (!array_key_exists($original, $g_overloaders) || $overwrite)
      {
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
      if (array_key_exists($original, $g_overloaders))
      {
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
      if (array_key_exists($original, $g_overloaders))
      {
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
     * @param array  $params Any params to be passed to the invoked method.
     *
     * @return boolean false if the call failed. In all other cases, it
     *                 returns the output of the invoked method. (be
     *                 careful with methods that return false).
     */
    function invokeFromString($str, $params=array())
    {
      if (strpos($str, "#")===false) return false;

      list($class, $method) = explode("#", $str);
      if ($class!="" && $method!="")
      {
        $handler = &atknew($class);
        if (is_object($handler))
        {
          return $handler->$method($params);
        }
        return false;
      }
      else
      {
        return false;
      }
    }

    /**
     * Finds a class in the current application.
     *
     * @param string $classname The classname to find.
     * @return string|bool The classpath (atkimport statement) of the class
     *                     if found, else false
     */
    function findClass($classname)
    {
      static $s_forced;
      $classname = strtolower($classname);
      $classloader = new atkClassLoader();
      $classes = $classloader->getAllClasses();
      if (!in_array($classname,array_keys($classes)))
      {
        if (!$s_forced)
        {
          $s_forced=1;
          $classes = $classloader->getAllClasses(true);
          if (in_array($classname,array_keys($classes)))
          {
            return $classes[$classname];
          }
        }
        return false;
      }
      return $classes[$classname];
    }

    /**
     * Gets an array with all the the classes
     *
     * @param bool $force Force reloading of classes, instead of using cache.
     * @return Array An array with the classes as keys and the path as value.
     */
    function getAllClasses($force=false)
    {
      static $s_classes=array();

      if (empty($s_classes) || $force)
      {
        atkimport('atk.utils.atktmpfile');
        $cache = new atkTmpFile('classes.inc.php');
        $classes = array();
        @include($cache->getPath());

        if (empty($classes) || $force)
        {
          $classes = $this->findAllClasses();
          $cache->writeAsPhp('classes',$classes);
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
      $traverser = atknew('atk.utils.atkdirectorytraverser');
      $classfinder = new atkClassFinder();
      $traverser->addCallbackObject($classfinder);
      $traverser->traverse(atkconfig('atkroot').'atk/');
      $classes = $classfinder->getClasses();
      atkdebug("atkClassLoader::findAllClasses(): Found ".count($classes).' classes');
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
  class atkClassFinder
  {
    var $m_classes=array();

    function visitFile($file)
    {
      $filename = basename($file);
      if (substr($filename,0,6)==='class.' && substr($filename, -4)==='.inc')
      {
        $this->m_classes[substr($filename, 6,-4)] = getClassName($file);
      }
    }

    /**
     * Returns all the found classes as keys with their classpath (atkimport statement)
     * as value.
     *
     * @return array The found classes with classpatsh
     */
    function getClasses()
    {
      return $this->m_classes;
    }
  }
?>