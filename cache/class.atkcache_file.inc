<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Cache class for filebases caching based on file cache of the Solar framework.
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

class atkCache_file extends atkCache
{
  /**
   * Path where the cache files are saved
   *
   * @var string
   */
  protected $m_path;

  /**
   * Context that is used for fopen
   *
   * @var array
   */
  protected $m_context;


  /**
   * Constructor
   */
  public function __construct()
  {
    // create the context
    $context = $this->getCacheConfig('context');
    if (is_resource($context))
    {
      $this->m_context = $context;
    }
    elseif (is_array($context))
    {
      $this->m_context = stream_context_create($context);
    }
    else
    {
       $this->m_context = stream_context_create(array());
    }
    $this->m_context = stream_context_create(array());
    $this->m_path = $this->getCacheConfig('path',atkconfig('atktempdir')."cache/");
    atkFileAttribute::mkdir($this->m_path);

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
    // what file should we look for?
    $file = $this->getRealKey($key);

    // if the file does not exists or is unreadable, key is available
    if (! file_exists($file) || ! is_readable($file))
    {
      return $this->set($key, $data, $lifetime);
    }

    // if the file has expired, key is available

    if($lifetime === false) $lifetime = $this->m_lifetime;
    $expire_time = filemtime($file) + $lifetime;
    if (time() > $expire_time)
    {
      return $this->set($key, $data, $lifetime);
    }

    // key already exists
    return false;

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

    // serialize all non-scalar data
    if (! is_scalar($data))
    {
      $data = serialize($data);
      $serial = true;
    }
    else
    {
      $serial = false;
    }

    $file = $this->getRealKey($key);
    $fp = @fopen($file, 'wb', false, $this->m_context);

    if ($fp)
    {
      flock($fp, LOCK_EX);
      fwrite($fp, $data, strlen($data));

      // add a .serial file? (do this while the file is locked to avoid
      // race conditions)
      if ($serial)
      {
        // use this instead of touch() because it supports stream
        // contexts.
        file_put_contents($file . '.serial', null, LOCK_EX, $this->m_context);
      }
      else
      {
        // make sure no serial file is there from any previous entries
        // with the same name
        @unlink($file . '.serial', $this->m_context);
      }

      // create a .lifetime file to handle the lifetime of the cached item
      file_put_contents($file . '.lifetime',$lifetime, LOCK_EX, $this->m_context);

      // unlock and close, then done.
      flock($fp, LOCK_UN);
      fclose($fp);
      return true;
    }

    // could not open the file for writing.
    return false;
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

    // get the entry filename *before* validating;
    // this avoids race conditions.
    $file = $this->getRealKey($key);

    // make sure the file exists and is readable,
    if (file_exists($file) && is_readable($file))
    {
      // get the lifetime of the entry
      $lifetime = file_get_contents($file.".lifetime");

      // has the file expired?
      $expire_time = filemtime($file) + $lifetime;
      if (time() > $expire_time)
      {
         // expired, remove it
         $this->delete($key);
         return false;
      }
    }
    else
    {
      return false;
    }

    // file exists; open it for reading
    $fp = @fopen($file, 'rb', false, $this->m_context);

    // could it be opened?
    if ($fp)
    {

      // lock the file right away
      flock($fp, LOCK_SH);

      // get the cache entry data.
      // PHP caches file lengths; clear that out so we get
      // an accurate file length.
      clearstatcache();
      $len = filesize($file);
      $data = fread($fp, $len);

      // check for serializing while file is locked
      // to avoid race conditions
      if (file_exists($file . '.serial')) {
          $data = unserialize($data);
      }

      // unlock and close the file
      flock($fp, LOCK_UN);
      fclose($fp);

      // done!
      return $data;
    }

    // could not open file.
    return false;

  }

  /**
   * Deletes all cache entries
   * @return void
   */
  public function delete($key)
  {
    if (!$this->m_active)
    {
      return false;
    }
    $file = $this->getRealKey($key);

    @unlink($file, $this->m_context);
    @unlink($file . '.serial', $this->m_context);
    @unlink($file . '.lifetime', $this->m_context);

  }

  /**
   * Removes all cache entries.
   *
   * @return void
   */
  public function deleteAll()
  {
    if (!$this->m_active)
    {
      return false;
    }

    $list = (array) @scandir($this->m_path, null, $this->m_context);

    // delete each file
   foreach ($list as $file)
   {
     // Skip files that start with a dot
     if ($file[0]=='.') continue;
     @unlink($this->m_path . $file, $this->m_context);
   }

  }

  /**
   * Get realkey for the cache entry
   *
   * @param string $key Entry ID
   * @return string The real entry id
   */
  public function getRealKey($key)
  {
     return $this->m_path . md5($key);
  }

 /**
  * Get the current cache type
  *
  * @return string atkConfig type
  */
  public function getType()
  {
    return 'file';
  }


}

?>