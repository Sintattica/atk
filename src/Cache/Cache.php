<?php

namespace Sintattica\Atk\Cache;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use ArrayAccess;
use Exception;

/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Base class for all caching systems that atk supports
 *
 *
 * @copyright (c)2008 Sandy Pleyte
 * @author Sandy Pleyte <sandy@achievo.org>
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 6309 $
 * $Id$
 */
abstract class Cache implements ArrayAccess
{
    /**
     * All cache instances.
     *
     * @var Cache[]
     */
    private static $m_instances = array();

    /**
     * Is the cache still active.
     *
     * @var bool
     */
    protected $m_active = true;

    /**
     * Lifetime of each cache entry in seconds.
     *
     * @var int
     */
    protected $m_lifetime = 3600;

    /**
     * Namespace so atkCache can also be used on shared hosts.
     *
     * @var string
     */
    protected $m_namespace = 'default';

    /**
     * Private Constructor so we can only have
     * once instance of each cache.
     */
    private function __construct()
    {
    }

    /**
     * Get Cache instance, default when no type
     * is configured it will use var cache.
     *
     * @param string $types    Cache type
     * @param bool   $fallback fallback to var cache if all types fail?
     * @param bool   $force    force new instance
     *
     * @return Cache object of the request type
     *
     * @throws Exception if $fallback is false and Cache type(s) not found
     */
    public static function getInstance($types = '', $fallback = true, $force = false)
    {
        if ($types == '') {
            $types = Config::getGlobal('cache_method', array());
        }
        if (!is_array($types)) {
            $types = array($types);
        }

        foreach ($types as $type) {
            try {
                if (!$force && array_key_exists($type, self::$m_instances) && is_object(self::$m_instances[$type])) {
                    Tools::atkdebug("cache::getInstance -> Using cached instance of $type");

                    return self::$m_instances[$type];
                } else {
                    self::$m_instances[$type] = new $type();
                    self::$m_instances[$type]->setNamespace(Config::getGlobal('cache_namespace', 'default'));
                    self::$m_instances[$type]->setLifetime(self::$m_instances[$type]->getCacheConfig('lifetime', 3600));
                    self::$m_instances[$type]->setActive(Config::getGlobal('cache_active', true));
                    Tools::atkdebug("cache::getInstance() -> Using $type cache");

                    return self::$m_instances[$type];
                }
            } catch (Exception $e) {
                Tools::atknotice("Can't instantatie atkCache class $type: ".$e->getMessage());
            }
        }

        if (!$fallback) {
            throw new Exception('Cannot instantiate Cache class of the following type(s): '.implode(', ', $types));
        }

        // Default return var cache
        Tools::atkdebug('cache::getInstance() -> Using var cache');

        return self::getInstance('var', false, $force);
    }

    /**
     * Get config values from the cache config.
     *
     * @param string $key     Key
     * @param mixed  $default Default value
     *
     * @return mixed
     */
    public function getCacheConfig($key, $default = '')
    {
        $cacheConfig = Config::getGlobal('cache', array());
        $type = $this->getType();

        if (array_key_exists($type, $cacheConfig) &&
            array_key_exists($key, $cacheConfig[$type])
        ) {
            return $cacheConfig[$type][$key];
        } else {
            return $default;
        }
    }

    /**
     * Get Classname.
     *
     * @param string $type Cache type
     *
     * @return string Classname of the cache type
     */
    private function getClassname($type)
    {
        if (strpos($type, '.') === false) {
            return "atk.cache.atkcache_$type";
        } else {
            return $type;
        }
    }

    /**
     * Turn cache on/off.
     *
     * @param bool $flag Set cache active or not active
     */
    public function setActive($flag)
    {
        $this->m_active = (bool) $flag;
    }

    /**
     * is cache active.
     *
     * @return bool
     */
    public function isActive()
    {
        return $this->m_active;
    }

    /**
     * Set the namespace for the current cache.
     *
     * @param string $namespace
     */
    public function setNamespace($namespace)
    {
        $this->m_namespace = $namespace;
    }

    /**
     * Return current namespace that the cache is using.
     *
     * @return string
     */
    public function getNamespace()
    {
        return $this->m_namespace;
    }

    /**
     * Set the lifetime in seconds for the cache.
     *
     * @param int $lifetime Set the lifetime in seconds
     */
    public function setLifetime($lifetime)
    {
        $this->m_lifetime = (int) $lifetime;
    }

    /**
     * Get lifetime of the ache.
     *
     * @return int The cache lifetime
     */
    public function getLifetime()
    {
        return $this->m_lifetime;
    }

    /**
     * Add cache entry if it not exists
     * allready.
     *
     * @param string $key      Entry Id
     * @param mixed  $data     The data we want to add
     * @param int    $lifetime give a specific lifetime for this cache entry. When $lifetime is false the default lifetime is used.
     *
     * @return bool True on success, false on failure.
     */
    abstract public function add($key, $data, $lifetime = false);

    /**
     * Set cache entry, if it not exists then
     * add it to the cache.
     *
     * @param string $key      Entry ID
     * @param mixed  $data     The data we want to set
     * @param int    $lifetime give a specific lifetime for this cache entry. When $lifetime is false the default lifetime is used.
     *
     * @return true on success, false on failure.
     */
    abstract public function set($key, $data, $lifetime = false);

    /**
     * get cache entry by key.
     *
     * @param string $key Entry id
     *
     * @return mixed Boolean false on failure, cache data on success.
     */
    abstract public function get($key);

    /**
     * delete cache entry.
     *
     * @param string $key Entry ID
     */
    abstract public function delete($key);

    /**
     * Deletes all cache entries.
     */
    abstract public function deleteAll();

    /**
     * Get realkey for the cache entry.
     *
     * @param string $key Entry ID
     *
     * @return string The real entry id
     */
    public function getRealKey($key)
    {
        return $this->m_namespace.'::'.$key;
    }

    /**
     * Get Current cache type.
     *
     * @return string Current cache
     */
    public function getType()
    {
        return 'base';
    }

    // ************************
    // * ArrayAcces functions *
    // ************************   

    /**
     * Whether the offset exists.
     *
     * @param string $offset Key to check
     *
     * @return bool
     */
    public function offsetExists($offset)
    {
        return $this->get($offset) !== false;
    }

    /**
     * Value at given offset.
     *
     * @param string $offset Key to get
     *
     * @return mixed
     */
    public function offsetGet($offset)
    {
        return $this->get($offset);
    }

    /**
     * Set value for given offset.
     *
     * @param string $offset Key to set
     * @param mixed  $value  Value for key
     *
     * @return bool
     */
    public function offsetSet($offset, $value)
    {
        return $this->set($offset, $value);
    }

    /**
     * Unset value for given offset.
     *
     * @param string $offset Key to unset
     */
    public function offsetUnset($offset)
    {
        return $this->delete($offset);
    }
}
