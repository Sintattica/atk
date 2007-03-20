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
   * @version $Revision$
   * $Id$
   */

  /**
   * Utility for importing and loading classes.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @package atk
   * @subpackage utils
   *
   * @todo atknew() and atkimport() function should be moved to this
   *       class.
   *
   */
  class atkClassLoader
  {
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
    function resolveClass($class)
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
          if (!in_array($classname,array_keys($classes)))
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
      atkdebug("atkClassLoader::findAllClasses($force): Found ".count($classes).' classes');
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