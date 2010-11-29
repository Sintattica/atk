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
   * @copyright (c)2007 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6320 $
   * $Id$
   */

  /**
   * ATK JSON wrapper.
   *
   * Small wrapper around the PHP-JSON and JSON-PHP libraries. If you don't have
   * the PHP-JSON C library installed this class will automatically fallback to
   * the JSON-PHP PHP library. It's recommended to install the C library
   * because it's much faster.
   *
   * More information:
   * - http://pear.php.net/pepr/pepr-proposal-show.php?id=198
   * - http://www.aurore.net/projects/php-json/
   *
   * JSON (JavaScript Object Notation) is a lightweight data-interchange
   * format. It is easy for humans to read and write. It is easy for machines
   * to parse and generate. It is based on a subset of the JavaScript
   * Programming Language, Standard ECMA-262 3rd Edition - December 1999.
   * This feature can also be found in  Python. JSON is a text format that is
   * completely language independent but uses conventions that are familiar
   * to programmers of the C-family of languages, including C, C++, C#, Java,
   * JavaScript, Perl, TCL, and many others. These properties make JSON an
   * ideal data-interchange language.
   *
   * @package atk
   * @subpackage utils
   * @author Peter C. Verhage <peter@ibuildings.nl>
   */
  class atkJSON
  {
    /**
     * Maximum recursion depth for conversion of data for encoding to UTF-8
     */
    const UTF8_CONVERSION_RECURSION_LIMIT = 30;

    /**
     * Get JSON service object. This method should never
     * be called directly!
     *
     * @param bool $assoc return as associative array (instead of objects)
     * @return Services_JSON JSON service object
     *
     * @static
     * @private
     */
    private static function &_getJSONService($assoc=false)
    {
      static $services = array();

      if (!isset($services[$assoc]))
      {
        if (!class_exists("Services_JSON"))
        {
          include_once atkconfig('atkroot').'atk/ext/json/json.php';
        }

        $services[$assoc] = new Services_JSON($assoc ? SERVICES_JSON_LOOSE_TYPE : 0);
      }

      return $services[$assoc];
    }

    /**
     * Encode to JSON.
     *
     * @param mixed $var PHP variable
     * @return string JSON string
     */
    public static function encode($var)
    {
      if (function_exists('json_encode')) {
        $encoded = json_encode($var);
        if ($encoded!=='null' || $var===null) {
          return $encoded;
        }
        else {
          // Variable may contain non-utf-8 characters (like binary data)
          // format to UTF-8 and try again.
          return json_encode(self::_utf8json($var));
        }
      }
      $service = atkJSON::_getJSONService();
      return $service->encode($var);
    }

    /**
     * Convert a mixed type variable to UTF-8
     * @param mixed $data PHP variable
     */
    private function _utf8json($data, $depth = 0) {
      $depth++;
      if ($depth >= self::UTF8_CONVERSION_RECURSION_LIMIT) {
        throw new Exception('atkJSON recustion limit reached on: ' . $value);
      }

      if (is_string($data)) {
        return utf8_encode($data);
      }
      else if (is_numeric) {
        return $data;
      }
      else if (is_array($data)) {
        /* our return object */
        $newArray = array();

        foreach($data as $key => $val) {
          $newArray[$key] = self::_utf8json($val, $depth);
        }

        /* return utf8 encoded array */
        return $newArray;       
      }
      else {
        throw new Exception("Unrecognized datatype for UTF-8 conversion in atkJSON");
      }
    } 

    /**
     * Decode JSON string.
     *
     * @param string $string JSON string
     * @param boolean $assoc return as associative array (instead of objects)
     * @return mixed PHP value
     */
    public static function decode($string, $assoc=false)
    {
      if (function_exists('json_decode'))
        return json_decode($string, $assoc);
      $service = atkJSON::_getJSONService($assoc);
      return $service->decode($string);
    }
  }
