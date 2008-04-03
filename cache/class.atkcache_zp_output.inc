<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Cache class for Zend Platform 3.6 and lower (http://www.zend.com/en/products/platform/)
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

class atkCache_zp_output extends atkCache
{

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
    $acceleratorConfig = accelerator_get_configuration();
    if (@!$acceleratorConfig['output_cache_licensed'])
    {
      throw new Exception('The Zend Platform extension does not have the proper license to use content caching features.');
    }
    if (@!$acceleratorConfig['output_cache_enabled'])
    {
      throw new Exception('The Zend Platform content caching feature must be enabled for using this backend, set the \'zend_accelerator.output_cache_enabled\' directive to On !');
    }
    if (!is_writable($acceleratorConfig['output_cache_dir']))
    {
      throw new Exception('The cache copies directory \''. ini_get('zend_accelerator.output_cache_dir') .'\' must be writable !');
    }
    if(!function_exists('output_cache_put'))
    {
      throw new Exception('The Zend Platform doesn\'t support the ouput cache anymore (version 3.0 and lower only)');
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

     if($lifetime !== false) atkwarning("Lifetime setting is not supported in this type of cache :".$this->getType().". Instead default lifetime is used");

     if (output_cache_get($key,$this->m_lifetime) !== false)
     {
       return false;
     }

     output_cache_put($key, serialize($data));
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

     if($lifetime !== false) atkwarning("Lifetime setting is not supported in this type of cache :".$this->getType().". Instead default lifetime is used");

     output_cache_put($key, serialize($data));
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

     $rawCacheValue = output_cache_get($key, $this->m_lifetime);
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
    if (! $this->m_active)
    {
      return false;
    }
    return output_cache_remove_key($key);
  }

  /**
   * Removes all cache entries.
   * Not supported for ZendPlatform output cache
   *
   * @return void
   */
  public function deleteAll() {}

  /**
   * Get the current cache type
   *
   * @return string atkConfig type
   */
  public function getType()
  {
    return 'zp_output';
  }


}

?>