<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Cache class for eaccelerator (http://eaccelerator.net/)
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
class atkCache_eaccelerator extends atkCache
{
  /**
   * constructor
   */
  public function __construct()
  {
    if(!(extension_loaded('eaccelerator') && ini_get('eaccelerator.enable')))
    {
      throw new Exception('The eAccelerator extension is not loaded or disabled');
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
    
    if (eaccelerator_get($this->getRealKey($key)) !== null) 
    {
      return false;
    }
    
    if($lifetime === false) $lifetime = $this->m_lifetime;
    return eaccelerator_put($this->getRealKey($key), serialize($data), $lifetime);
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
    return eaccelerator_put($this->getRealKey($key), serialize($data), $lifetime);
  }

  /**
   * Gets cache entry data.
   * 
   * @param string $key The entry ID.
   * @return mixed Boolean false on failure, cache data on success.
   */
  public function get($key)
  {
    if (!$this->m_active) {
      return false;
    }
    
    $rawCacheValue = eaccelerator_get($this->getRealKey($key));
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
        
    return eaccelerator_rm($this->getRealKey($key));
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
    
    return eaccelerator_clean();
  }
  
  public function getType()
  {
    return 'eaccelerator';
  }
  
}

?>