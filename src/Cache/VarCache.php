<?php namespace Sintattica\Atk\Cache;


class VarCache extends Cache
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
        $this->setLifeTime($this->getCacheConfig('lifetime', 3600));
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
        $this->m_entry[$this->getRealKey($key)] = $data;
        $this->m_expires[$this->getRealKey($key)] = time() + $lifetime;
        return true;
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

        if (empty($this->m_entry[$this->getRealKey($key)])) {
            return $this->set($key, $data, $lifetime);
        } else {
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
        if (!$this->m_active) {
            return false;
        }

        if (!empty($this->m_entry[$this->getRealKey($key)]) && $this->m_expires[$this->getRealKey($key)] >= time()) {
            // exists, and is within its lifetime
            return $this->m_entry[$this->getRealKey($key)];
        } else {
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
     * @return boolean Success
     */
    public function delete($key)
    {
        if (!$this->m_active) {
            return false;
        }

        unset($this->m_entry[$this->getRealKey($key)]);
        unset($this->m_expires[$this->getRealKey($key)]);
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
        $this->m_entry = array();
        $this->m_expires = array();
        return true;
    }

    /**
     * Get the current cache type
     *
     * @return string Config type
     */
    public function getType()
    {
        return 'var';
    }

}
