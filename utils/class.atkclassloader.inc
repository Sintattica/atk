<?php

  /**
   * Utility for importing and loading classes.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @version $Revision$
   *
   * $Id$
   */
  class atkClassLoader
  {
    /**
     * invoke a method on a class based on a string definition.
     * The string must be in the format 
     * "packagename.subpackage.classname#methodname"
     *
     * @param String $str The "classname#method" to invoke.
     * @param Array  $params Any params to be passed to the invoked method.
     *
     * @return false if the call failed. In all other cases, it returns the
     *               output of the invoked method. (be careful with methods
     *               that return false).
     */
    function invokeFromString($str, $params)
    {
      list($class, $method) = explode("#", $str);
      if ($class!="" && $method!="")
      {
        $handler = &atknew($class);
        return $handler->$method($params);
      }
      else
      {
        return false;
      }
    }
  }
?>