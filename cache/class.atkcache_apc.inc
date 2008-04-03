<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Cache class for apc (http://pecl.php.net/package/APC)
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

class atkCache_apc extends atkCache
{
  /**
   * constructor
   */
   public function __construct()
   {
     // make sure we have apc available
     if (! ( extension_loaded('apc') && ini_get('apc.enabled') ) ) {
        throw new Exception('The APC extension is not loaded or disabled');
     }
     $this->m_namespace=atkconfig('cache_namespace',"default");
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

    if($lifetime === false) $lifetime = $this->m_lifetime;
    return apc_add($this->getRealKey($key), serialize($data), $lifetime);
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
    return apc_store($this->getRealKey($key), serialize($data), $lifetime);
  }

  /**
   * Gets cache entry data.
   * Note: cache can be older then the lifetime that is set. It seems this is a bug
   * in the APC cache that is ignoring the lifetime param from the add / store function.
   *
   * @param string $key The entry ID.
   * @return mixed Boolean false on failure, cache data on success.
   */
  public function get($key)
  {
    if (!$this->m_active) {
      return false;
    }

    $rawCacheValue = apc_fetch($this->getRealKey($key));
    $cacheValue = is_string($rawCacheValue) ? unserialize($rawCacheValue) : $rawCacheValue;
    return $cacheValue;
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

    return apc_delete($this->getRealKey($key));
  }

  /**
   * Removes all cache entries.
   *
   * @return void
   */
  public function deleteAll()
  {
    if (! $this->m_active) {
      return false;
    }

    apc_clear_cache('user');
  }

  /**
   * Get the current cache type
   *
   * @return string atkConfig type
   */
  public function getType()
  {
    return 'apc';
  }

}

?>