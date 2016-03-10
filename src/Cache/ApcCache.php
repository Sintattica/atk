<?php

namespace Sintattica\Atk\Cache;

use Sintattica\Atk\Core\Config;
use Exception;

class ApcCache extends Cache
{
    /**
     * constructor.
     */
    public function __construct()
    {
        // make sure we have apc available
        if (!(extension_loaded('apc') && ini_get('apc.enabled'))) {
            throw new Exception('The APC extension is not loaded or disabled');
        }
        $this->m_namespace = Config::getGlobal('cache_namespace', 'default');
    }

    /**
     * Inserts cache entry data, but only if the entry does not already exist.
     *
     * @param string   $key      The entry ID.
     * @param mixed    $data     The data to write into the entry.
     * @param int|bool $lifetime give a specific lifetime for this cache entry. When $lifetime is false the default lifetime is used.
     *
     * @return bool True on success, false on failure.
     */
    public function add($key, $data, $lifetime = false)
    {
        if (!$this->m_active) {
            return false;
        }

        if ($lifetime === false) {
            $lifetime = $this->m_lifetime;
        }

        return apc_add($this->getRealKey($key), serialize($data), $lifetime);
    }

    /**
     * Sets cache entry data.
     *
     * @param string   $key      The entry ID.
     * @param mixed    $data     The data to write into the entry.
     * @param int|bool $lifetime give a specific lifetime for this cache entry. When $lifetime is false the default lifetime is used.
     *
     * @return bool True on success, false on failure.
     */
    public function set($key, $data, $lifetime = false)
    {
        if (!$this->m_active) {
            return false;
        }

        if ($lifetime === false) {
            $lifetime = $this->m_lifetime;
        }

        return apc_store($this->getRealKey($key), serialize($data), $lifetime);
    }

    /**
     * Gets cache entry data.
     * Note: cache can be older then the lifetime that is set. It seems this is a bug
     * in the APC cache that is ignoring the lifetime param from the add / store function.
     *
     * @param string $key The entry ID.
     *
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
     *
     * @return bool Success
     */
    public function delete($key)
    {
        if (!$this->m_active) {
            return false;
        }

        return apc_delete($this->getRealKey($key));
    }

    /**
     * Removes all cache entries.
     *
     * @return bool Success
     */
    public function deleteAll()
    {
        if (!$this->m_active) {
            return false;
        }

        apc_clear_cache('user');

        return true;
    }

    /**
     * Get the current cache type.
     *
     * @return string atkConfig type
     */
    public function getType()
    {
        return 'apc';
    }
}
