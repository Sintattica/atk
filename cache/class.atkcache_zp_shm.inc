<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Cache class for Zend Platform 3.6 and higher (http://www.zend.com/en/products/platform/)
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

class atkCache_zp_shm extends atkCache
{
  protected $m_namespace;

  /**
   * constructor
   */
  public function __construct()
  {
    if (!function_exists('accelerator_license_info'))
    {
      throw new Exception('The Zend Platform extension is not loaded');
    }
    if (!function_exists('accelerator_get_configuration'))
    {
      $licenseInfo = accelerator_license_info();
      throw new Exception('The Zend Platform extension is not loaded correctly: '.$licenseInfo['failure_reason']);
    }
    if(!function_exists('zend_shm_cache_store'))
    {
      throw new Exception('The Zend Platform doesn\'t support the shared memory cache (version 3.6 and higher only)');
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
     if (! $this->m_active)
     {
       return false;
     }

     if (zend_shm_cache_fetch($this->getRealKey($key)) !== null)
     {
       return false;
     }

     if($lifetime === false) $lifetime = $this->m_lifetime;
     zend_shm_cache_store($this->getRealKey($key), serialize($data),$lifetime);
     return true;
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
     if (! $this->m_active)
     {
       return false;
     }
     if($lifetime === false) $lifetime = $this->m_lifetime;
     zend_shm_cache_store($this->getRealKey($key), serialize($data),$lifetime);
     return true;
  }

  /**
   * Gets cache entry data.
   *
   * @param string $key The entry ID.
   * @return mixed Boolean false on failure, cache data on success.
   */
  public function get($key)
  {
     if (! $this->m_active)
     {
       return false;
     }

     $rawCacheValue =zend_shm_cache_fetch($this->getRealKey($key));
     $cacheValue = is_string($rawCacheValue) ? unserialize($rawCacheValue) : $rawCacheValue;
     return $cacheValue;
  }

  /**
   * Deletes a cache entry.
   *
   * @param string $key The entry ID.
   * @return boolean Succes
   */
  public function delete($key)
  {
    if (! $this->m_active)
    {
      return false;
    }
    return zend_shm_cache_delete($this->getRealKey($key));
  }


  /**
   * Removes all cache entries.
   *
   * @return boolean Succes
   */
  public function deleteAll()
  {
    if (! $this->m_active)
    {
      return false;
    }
    return zend_shm_cache_clear($this->m_namespace);
  }

  /**
   * Get the current cache type
   *
   * @return string atkConfig type
   */
  public function getType()
  {
    return 'zp_shm';
  }


}

?>