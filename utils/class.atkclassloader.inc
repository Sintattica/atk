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
  }
?>