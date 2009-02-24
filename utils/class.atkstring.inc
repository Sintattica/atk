<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * String class for multibyte (utf-8) character support
 * 
 * @package atk
 * @subpackage utils
 *
 * @copyright (c)2009 Ibuildings
 * @author Sandy Pleyte <sandy@achievo.org>
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */
class atkString
{
	/**
	 * Do we have multibyte support 
	 * @var boolean
	 */
	static $s_hasMultiByteSupport = null;

  /**
   * Accepted charsets for htmlentities and html_entity_decode
   * @var array
   */	
	static $s_acceptedCharsets = array('iso-8859-1', 'iso-8859-15', 'utf-8', 'cp866', 'ibm866', '866',
                   				            'cp1251', 'windows-1251', 'win-1251', '1251', 'cp1252', 'windows-1252', '1252',
                                      'koi8-r', 'koi8-ru', 'koi8r', 'big5', '950', 'gb2312', '936', 'big5-hkscs',
                                      'shift_jis', 'sjis', '932', 'euc-jp', 'eucjp');

	
	/**
	 * Check if the system has multibyte support
	 * @return boolean
	 */
	protected static function hasMultiByteSupport()
	{
		if(self::$s_hasMultiByteSupport === null)
		{
  		if (function_exists('mb_strlen') && atkconfig('use_mbstring',true)) 
      {
        mb_internal_encoding(atkGetCharset());
				self::$s_hasMultiByteSupport = true;
      }
			else
			{
				self::$s_hasMultiByteSupport = false;
			}
		}
		return self::$s_hasMultiByteSupport;
 	}
	
	/**
	 * Get string length
	 * @param string $str The string being checked for length
	 * @return int
	 */
	public static function strlen($str)
	{
		if(self::hasMultiByteSupport())
		{
			return mb_strlen($str);
		}
		elseif(strtolower(atkGetCharset())=='utf-8')
		{
  		preg_match_all("/./su", $str, $matches);
      $chars = $matches[0];
      return count($chars);
		}
		else
		{
			return strlen($str);
		}
	}
	
	/**
	 * Get part of string
	 * @param string $str The string being checked. 
	 * @param int $start The first position used in $str 
	 * @param int $length[optional] The maximum length of the returned string
	 * @return string
	 */
	public static function substr($str,$start,$length='')
	{
		if(self::hasMultiByteSupport())
		{
			return mb_substr($str,$start,$length);
		}
		else
		{
			return substr($str,$start,$length);
		}
	}
	
	/**
	 * Return char on given position
	 * @param string $str The string being checked
	 * @param int $pos The position of the char
	 * @return string
	 */
	public static function charAt($str,$pos)
	{
		return self::substr($str,$pos,1);
	}
	
	/**
	 *  Find position of first occurrence of string in a string
	 * @param object $haystack The string being checked. 
	 * @param object $needle The position counted from the beginning of haystack . 
	 * @param object $offset[optional] The search offset. If it is not specified, 0 is used. 
	 * @return int|boolean
	 */
	public static function strpos($haystack,$needle,$offset=0)
	{
		if(self::hasMultiByteSupport())
		{
			return mb_strpos($haystack,$needle,$offset);
		}
		else
		{
			return substr($haystack,$needle,$offset);
		}
	} 
	
	/**
	 * Make a string lowercase
	 * @param string $str The string being lowercased. 
	 * @return string 
	 */
	public static function strtolower($str)
	{
		if(self::hasMultiByteSupport())
		{
			return mb_strtolower($str);
		}
		else
		{
			return strtolower($str);
		}
		
	}
	
	/**
	 * Make a string uppercase
	 * @param string $str The string being uppercased. 
	 * @return string 
	 */
	public static function strtoupper($str)
	{
		if(self::hasMultiByteSupport())
		{
			return mb_strtoupper($str);
		}
		else
		{
			return strtoupper($str);
		}
	}	
	
	
	
	/**
   * ATK version of the PHP html_entity_decode function. Works just like PHP's
   * html_entity_decode function, but falls back to the in the language file
   * configured charset instead of PHP's default charset, if no
   * charset is given.
   *
   * @param String $str    string to convert
   * @param int $quote_style  quote style (defaults to ENT_COMPAT)
   * @param String $charset   character set to use (default to atktext('charset', 'atk'))
   *
   * @return String encoded string
   */  
	function html_entity_decode($str, $quote_style=ENT_COMPAT, $charset=null)
  {
    if ($charset===null) $charset = atkGetCharset();
		
    // check if charset is allowed, else use default charset for this function
    if(!in_array(strtolower($charset),self::$s_acceptedCharsets)) $charset = 'iso-8859-1';
    
    return html_entity_decode($str, $quote_style, $charset);
  }
	
	/**
   * ATK version of the PHP htmlentities function. Works just like PHP's
   * htmlentities function, but falls back to atkGetCharset() instead of
   * PHP's default charset, if no charset is given.
   *
   * @param String $string    string to convert
   * @param int $quote_style  quote style (defaults to ENT_COMPAT)
   * @param String $charset   character set to use (default to atkGetCharset())
   *
   * @return String encoded string
   */
  function htmlentities($str, $quote_style=ENT_COMPAT, $charset=null)
  {
    if ($charset===null) $charset = atkGetCharset();
		
    // check if charset is allowed, else use default charset for this function
    if(!in_array(strtolower($charset),self::$s_acceptedCharsets)) $charset = 'iso-8859-1';

    return htmlentities($str, $quote_style, $charset);
  }
	
	/**
   * ATK version of the PHP html_entity_decode function. Works just like PHP's
   * html_entity_decode function, but falls back to atkGetCharset() instead of 
   * PHP's default charset, if no charset is given.
   *
   * @param String $string    string to convert
   * @param int $quote_style  quote style (defaults to ENT_COMPAT)
   * @param String $charset   character set to use (default to atkGetCharset())
   *
   * @return String encoded string
   */ 
  function iconv($in_charset, $out_charset, $str)
  {
    if(function_exists("iconv"))
    {
      $str = iconv($in_charset,$out_charset,$str);
    }
    else
    {
      atkwarning(atktext("error_iconv_not_install"));
    }
    return $str;
  }
	
	
}