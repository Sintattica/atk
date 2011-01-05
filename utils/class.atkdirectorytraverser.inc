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
   * @copyright (c)2005 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6320 $
   * $Id$
   */

  /**
   * This class can be used to recursively traverse a directory structure.
   * For each dir or file encountered, it calls callback methods on
   * callbackobjects.
   * This is similar to the unix 'find' command with an execute option.
   * You pass a callbackobject that performs the actual action. The
   * full pathname of the file is passed as a parameter to the callback
   * method.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   *
   */
  class atkDirectoryTraverser
  {
    public $m_callbackObjects = array();
    public $m_excludes = array();



    /**
     * Constructor
     */
    function atkDirectoryTraverser()
    {
    }

    /**
     * Add a callback object.
     *
     * The object can be any object. If the object implements a method with
     * the following signature:
     * function visitDir($fullpath)
     * the method is called for each directory that is traversed.
     * It is also possible to implement a callback method with this signature:
     * function visitFile($fullpath)
     * This method will be called for each regular file.
     * Both methods are optional. If an object does not implement either of
     * these methods, it will simply be ignored.
     *
     * @param Object $callbackObject An object instance to be called back for
     *                               each file/dir.
     */
    function addCallbackObject(&$callbackObject)
    {
      $this->m_callbackObjects[] = &$callbackObject;
    }

    /**
     * Remove all callback objects.
     * @see addCallbackObject()
     */
    function clearCallbackObjects()
    {
      $this->m_callbackObjects = array();
    }

    /**
     * Traverse a path.
     *
     * This method recursively traverses the specified path. Make sure you
     * pass the path without trailing '/'.
     *
     * @param String $path The path to traverse
     * @return boolean true if succesful, false if the path does not exist or is
     *                 not readable.
     */
    function traverse($path)
    {
      if (@is_file($path))
      {
        $this->_callback("visitFile", $path);
        return true;
      } 
      elseif (@is_dir($path))
      {
        $this->_callback("visitDir", $path);
        $filenames = $this->getDirContents($path);
        foreach ($filenames as $file)
        {
          if ($file!=".."&&$file!=".")
          {
            if (!$this->traverse($path."/".$file)) return false;
          }
        }
        return true;
      }
      elseif (@is_link($path)) return true;

      return false;
    }

    /**
     * Read all the entries of a directory.
     * @param String $path The path to read the contents from.
     * @return array Array containing the contents of the directory.
     */
    function getDirContents($path)
    {
   	  if(!is_dir($path)) {
   	  	return array();
   	  }

      $result = array();
      $dir = @opendir($path);
      while (($file = @readdir($dir)) !== false)
      {
        if (!$this->isExcluded($file))
        {
          $result[] = $file;
        }
      }
      @closedir($dir);
      sort($result);
      return $result;
    }

    /**
     * Perform a callback on all callbackobjects.
     * @access private
     * @param String $method  The name of the method to call.
     * @param String $filename The filename that will be passed as parameter
     *                         to the callback.
     */
    function _callback($method, $filename)
    {
      for($i=0, $_i=count($this->m_callbackObjects); $i<$_i; $i++)
      {
        if (method_exists($this->m_callbackObjects[$i], $method))
        {
          $this->m_callbackObjects[$i]->$method($filename);
        }
      }
    }

    /**
     * Add a regex expression to prevent detection of certain files/directories
     *
     * @param string $regex
     */
    function addExclude($regex)
    {
      $this->m_excludes[] = $regex;
    }

    /**
     * Clear the excluding regex expressions
     *
     */
    function clearExcludes()
    {
      $this->m_excludes = array();
    }

    /**
     * Check whether a file is excluded or not
     *
     * @param string $file
     */
    function isExcluded($file)
    {
      $excluded = false;
      $i = 0;
      $count = count($this->m_excludes);
      while (!$excluded && $i < $count)
      {
        $excluded = (preg_match($this->m_excludes[$i], $file) > 0);
        $i++;
      }
      return $excluded;
    }
  }
