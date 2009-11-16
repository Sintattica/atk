<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage include
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6236 $
   * $Id$
   */

  /**
   * The autoloader, autoloads classes that PHP can't find.
   * PHP 5 only.
   *
   * @param string $classname The name of the class PHP can't find
   */
  function ATK_autoload($classname)
  {
    /**
     * We exempt some external libraries from autoloading because
     * they create a loop by checking for their own existance
     * in the following manner:
     *
     * if (!class_exists('Smarty'))
     * {
     *   class Smarty
     *   ....
     * }
     *
     * This fraks up the autoloader.
     * If anyone has a better way to prevent this, please be my guest...
     */
    if (!empty($classname) && !in_array($classname,array('Smarty','Services_JSON','pear','PEAR_Error')))
    {
      atkwarning("Autoload triggered by {$classname}");
      $classpath = atkClassLoader::findClass($classname);

      if ($classpath && atkimport($classpath))
      {
        atkdebug("Autoloaded: ".$classname);
      }
    }
  }

  spl_autoload_register('ATK_autoload');

?>