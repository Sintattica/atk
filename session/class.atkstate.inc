<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage session
 *
 * @copyright (c)2000-2007 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * The atkstate manager.
 *
 * This class is used to save and retrieve the state of the sections, but
 * in the future probably also for other stuff (I want to move the tab state
 * also to this class later on). Now state can save in cookie (default) and in
 * global! session.
 *
 * @author Yury Golovnya <ygolovnya@gmain.com>
 * @package atk
 * @subpackage session
 *
 */

class atkState
{

  /**
   * This method retrieves a state value. This value can either be retrieved
   * from a cookie which is saved across sessions or from the current session, 
   * depending where the original value was saved. So the method first looks 
   * in the state cookie if the key exists, if not it looks in the session and
   * if it exists returns it's value. If not a value of NULL is returned.
   * 
   * @param Mixed $key     The key
   * @param Mixed $default default value fallback is the retrieved value === null
   * 
   * @return mixed The retrieved value.
   */
  public static function get($key, $default=null)
  {
    $key = self::getKey($key);

    $value = self::_getFromCookie($key);
    if ($value === null)
    {
      $value = self::_getFromSession($key);
    }
    
    if ($value === null)
    {
      $value = $default;
    }
    
    return $value;
  }
  
  /**
   * Get value from cookie
   *
   * @param mixed $key The keyname
   * @return string The value
   */
  protected static function _getFromCookie($key)
  {
    if(isset($_COOKIE[$key]))
    {
      return $_COOKIE[$key];
    }
    else
    {
      return null;
    }
  }
  
  /**
   * Get value from session
   *
   * @param mixed $key The keyname
   * @return string The value
   */
  protected static function _getFromSession($key)
  {
    if(isset($_SESSION[$key]))
    {
      return $_SESSION[$key];
    }
    else 
    {
      return null;
    }
  }
  /**
   * This method sets a state value. A state value can either be saved in the
   * current user's session (in the global session, so not in a specific stack!)
   * or in a cookie (by default in a cookie), the atk state cookie. Whatever
   * mechanism is used depends on the $type parameter. The key doesn't necessarily
   * have to be a string (this is also true for the get method). This means that
   * if you get an array as key you probably have to flatten the key to something
   * useful because if you want to save a value in the session or in a cookie
   * the key needs to be a simple string. You could use, for example 
   * print_r($key, true) to get a nice string representation. For cookies it might
   * be even more safe to md5 this string so that they key doesn't say anything to
   * the user and doesn't get too big.
   * @param mixed $key The key name
   * @param String $value The value of key
   * @param String $type The namespace from which to retrieve the value
   * @return mixed The storage method type.
   */
  public static function set($key, $value, $type='cookie')
  {
    $key = atkState::getKey($key);

    switch($type)
    {
      case 'cookie':
        self::_set_using_cookie($key,$value);
        break;
      case 'session':
        self::_set_using_session($key,$value);
        break;
      default:
        atkerror("set method don't exists");
    }
  }
  
  /**
   * Set value in cookie
   *
   * @param mixed $key
   * @param string $value
   */
  protected static function _set_using_cookie($key,$value)
  {
    setcookie($key, $value, time()+60*(atkconfig("state_cookie_expire")));
  }

  /**
   * Set value in session
   *
   * @param mixed $key
   * @param string $value
   */
  protected static function _set_using_session($key,$value)
  {
    $_SESSION[$key] = $value;
  }
  
  /**
   * Get the key
   *
   * @param string $key
   * @return string An md5 hash of the key
   */
  protected static function getKey($key)
  {
    return md5(print_r($key,true));
  }
}
?>