<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Cache class for variable (in-memory) 
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
class atkCache_var extends atkCache
{
  /**
   * Expiration timestamps for each cache entry.
   * @var array
   */
  protected $m_expires = array();

  /**
   * Cache entries.
   * @var array
   */
  protected $m_entry = array();
    

  /**
   * constructor
   */
  public function __construct() 
  {
    $this->setLifeTime($this->getCacheConfig('lifetime',3600));
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
      return;
    }
    
    if($lifetime === false) $lifetime = $this->m_lifetime;
    $this->m_entry[$this->getRealKey($key)] = $data;
    $this->m_expires[$this->getRealKey($key)] = time() + $lifetime;
    return true;
  }
    
  /**
   * Inserts cache entry data, but only if the entry does not already exist.
   * 
   * @param string $key The entry ID.
   * @param mixed $data The data to write into the entry.
   * @return bool True on success, false on failure.
   */
  public function add($key,$data,$lifetime=false)
  {
    if (!$this->m_active) 
    {
      return;
    }
        
    if (empty($this->m_entry[$this->getRealKey($key)])) 
    {
      return $this->set($key,$data,$lifetime);
    } 
    else 
    {
      return false;
    }
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
      return;
    }
    
    if (!empty($this->m_entry[$this->getRealKey($key)]) && $this->m_expires[$this->getRealKey($key)] >= time()) 
    {
      // exists, and is within its lifetime
      return $this->m_entry[$this->getRealKey($key)];
    } 
    else 
    {
      // clear the entry
      unset($this->m_entry[$this->getRealKey($key)]);
      unset($this->m_expires[$this->getRealKey($key)]);
      return false;
    }
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
      return;
    }
      
    unset($this->m_entry[$this->getRealKey($key)]);
    unset($this->m_expires[$this->getRealKey($key)]);
  }
  
  /**
   * Removes all cache entries.
   * 
   * @return void
   */
  public function deleteAll()
  {
    if (! $this->m_active) 
    {
      return;
    }
    $this->m_entry = array();
    $this->m_expires = array();
  }
  
  /**
   * Get the current cache type
   *
   * @return string atkConfig type
   */
  public function getType()
  {
    return 'var';
  }
}