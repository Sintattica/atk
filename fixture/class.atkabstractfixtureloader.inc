<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage fixture
   *
   * @copyright (c)2007 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Abstract fixture loader. All fixture loaders should extend
   * this class and implement the load method.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage fixture
   * @abstract
   */
  class atkAbstractFixtureLoader
  {
    /**
     * Parses the given string using PHP. Parsed results will be returned.
     * PHP code must be surrounded by PHP open and close tags. Script code
     * has full access to all loaded ATK files.
     *
     * @param string $string string to parse
     * @return string parse result
     *
     * @access protected
     */
    function parse($string)
    {
    	ob_start();
      eval(str_replace(array("?>\r\n", "?>\n"), array("?> \r\n", "?> \n"), "?>".$string));
      $string = ob_get_contents();
      ob_end_clean();
      return $string;
    }

    /**
     * Loads and returns the fixture data from the given file.
     *
     * @param string $path fixture file path
     * @return array fixture data
     *
     * @abstract
     */
    function load($path)
    {
      // should be implemented in subclass
      return false;
    }
  }
?>