<?php
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
 *
 * @copyright (c) 2006 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 * $Id$
 */
class atkJSON
{
  /**
   * Get JSON service object. This method should never
   * be called directly!
   *
   * @return Services_JSON JSON service object
   * 
   * @static 
   * @private
   */
  function &_getJSONService()
  {
    static $service = NULL;
    
    if ($service == NULL)
    {
      if (!class_exists("Services_JSON"))
      {
        include_once atkconfig('atkroot').'atk/ext/json/json.php';
      }
      
      $service = new Services_JSON();
    }
    
    return $service;
  }
  
  /**
   * Encode to JSON.
   *
   * @param mixed $var PHP variable
   * @return string JSON string
   */
  function encode($var)
  {
    if (function_exists('json_encode'))
      return json_encode($var);
    $service = atkJSON::_getJSONService();
    return $service->encode($var);
  }
  
  /**
   * Decode JSON string.
   *
   * @param string $string JSON string
   * @return mixed PHP value
   */
  function decode($string)
  {
    if (function_exists('json_decode'))
      return json_decode($string);      
    $service = atkJSON::_getJSONService();
    return $service->decode($string);
  }
}
?>