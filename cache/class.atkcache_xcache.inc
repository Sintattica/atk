<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Cache class for xcache (http://xcache.lighttpd.net/)
 *
 * @package atk
 * @subpackage cache
 *
 * @copyright (c)2008 Sandy Pleyte
 * @author Sandy Pleyte <sandy@achievo.org>
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */
atkimport("atk.cache.atkcache");

class atkCache_xcache extends atkCache
{
  /**
   * constructor
   */
  public function __construct()
  {
    if(!(extension_loaded('xcache') && ini_get('xcache.cacher')))
    {
      throw new Exception('The xCache extension is not loaded');
    }
  }

  /**
   * Inserts cache entry data, but only if the entry does not already exist.
   *
   * @param string $key The entry ID.
   * @param mixed $data The data to write into the entry.
   * @param int $lifetime give a specific lifetime for this cache entry. When $lifetime is false the default lifetime is used.
   * @return bool True on success, false on failure.
   */
  public function add($key,$data,$lifetime=false)
  {
    if (!$this->m_active)
    {
      return false;
    }

    if (xcache_isset($this->getRealKey($key)))
    {
      return false;
    }

    if($lifetime === false) $lifetime = $this->m_lifetime;
    return xcache_set($this->getRealKey($key), $data, $lifetime);
  }

  /**
   * Sets cache entry data.
   *
   * @param string $key The entry ID.
   * @param mixed $data The data to write into the entry.
   * @param int $lifetime give a specific lifetime for this cache entry. When $lifetime is false the default lifetime is used.
   * @return bool True on success, false on failure.
   */
  public function set($key,$data,$lifetime=false)
  {
    if (!$this->m_active)
    {
      return false;
    }

    if($lifetime === false) $lifetime = $this->m_lifetime;
    return xcache_set($this->getRealKey($key), $data, $lifetime);
  }

  /**
   * Gets cache entry data.
   *
   * @param string $key The entry ID.
   * @return mixed Boolean false on failure, cache data on success.
   */
  public function get($key)
  {
    if (!$this->m_active)
    {
      return false;
    }
     return xcache_get($this->getRealKey($key));
  }

  /**
   * Deletes a cache entry.
   *
   * @param string $key The entry ID.
   * @return void
   */
  public function delete($key)
  {
    if (!$this->m_active)
    {
      return false;
    }

    return xcache_unset($this->getRealKey($key));

  }

  /**
   * Removes all cache entries.
   *
   * @todo Test this feature, on my server this didn't work
   *       for some reason. Don't forget to return 'true' after
   *       fixing this.
   *
   * @return boolean Succes
   */
  public function deleteAll()
  {
    if (!$this->m_active)
    {
      return false;
    }
    /*

    $olduser = null;
    $oldpass = null;

    if (isset($_SERVER['PHP_AUTH_USER'])) {
        $olduser = $_SERVER['PHP_AUTH_USER'];
    }

    if (isset($_SERVER['PHP_AUTH_PW'])) {
        $oldpass = $_SERVER['PHP_AUTH_PW'];
    }

    // force credentials to the configured values
    $_SERVER['PHP_AUTH_USER'] = $this->getCacheConfig('user');
    $_SERVER['PHP_AUTH_PW'] = $this->getCacheConfig('passwd');

    try
    {
      // clear user cache
      $vcnt = xcache_count(XC_TYPE_VAR);
      for ($i = 0; $i < $vcnt; $i++) {
          if (! xcache_clear_cache(XC_TYPE_VAR, $i)) {
              return false;
          }
      }
    }
    catch(exception $e)
    {
      // Error
    }

    // Restore creds to prior state
    if ($olduser !== null) {
        $_SERVER['PHP_AUTH_USER'] = $olduser;
    } else {
        $_SERVER['PHP_AUTH_USER'] = null;
    }

    if ($oldpass !== null) {
        $_SERVER['PHP_AUTH_PW'] = $oldpass;
    } else {
        $_SERVER['PHP_AUTH_PW'] = null;
    }
    */
    //TODO Change this to true 
    return false;
  }


  /**
   * Get the current cache type
   *
   * @return string atkConfig type
   */
  public function getType()
  {
    return 'xcache';
  }

}

?>