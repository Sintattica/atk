<?php namespace Sintattica\Atk\Cache;

class MemcacheCache extends Cache
{
    public $m_memcache;

    /**
     * Constructor
     *
     * @todo Add support for Memcache pools
     */
    public function __construct()
    {
        if (!extension_loaded('memcache')) {
            throw new \Exception('The memcache extension is not loaded');
        }
        $this->m_memcache = new \Memcache;
        $result = @$this->m_memcache->connect($this->getCacheCOnfig('host', 'localhost'), $this->getCacheConfig('port', 11211),
            $this->getCacheConfig('timeout', 1));
        if (!$result) {
            throw new \Exception('Can\'t connect to the memcache server');
        }
    }

    /**
     * Inserts cache entry data, but only if the entry does not already exist.
     *
     * @param string $key The entry ID.
     * @param mixed $data The data to write into the entry.
     * @param int|bool $lifetime give a specific lifetime for this cache entry. When $lifetime is false the default lifetime is used.
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

        return $this->m_memcache->add($key, $data, null, $lifetime);
    }

    /**
     * Sets cache entry data.
     *
     * @param string $key The entry ID.
     * @param mixed $data The data to write into the entry.
     * @param int|bool $lifetime give a specific lifetime for this cache entry. When $lifetime is false the default lifetime is used.
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

        return $this->m_memcache->set($key, $data, null, $lifetime);
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

        return $this->m_memcache->get($key);
    }

    /**
     * Deletes a cache entry.
     *
     * @param string $key The entry ID.
     * @return boolean Success
     */
    public function delete($key)
    {
        if (!$this->m_active) {
            return false;
        }
        $this->m_memcache->delete($key);

        return true;
    }

    /**
     * Removes all cache entries.
     *
     * @return boolean Success
     */
    public function deleteAll()
    {
        if (!$this->m_active) {
            return false;
        }
        $this->m_memcache->flush();

        return true;
    }

    /**
     * Get the current cache type
     *
     * @return string Config type
     */
    public function getType()
    {
        return 'memcache';
    }
}
