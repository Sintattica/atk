<?php namespace Sintattica\Atk\Lock;

/**
 * This class is the abstract ATK lock base class (interface).
 *
 * All subclasses should implement
 * the same API. The (static) getInstance() method of this class can be
 * used to retrieve the one and only lock instance.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage lock
 * @abstract
 */
class Lock
{
    const EXCLUSIVE = 'exclusive';
    const SHARED = 'shared';

    var $m_id = null;

    /**
     * Returns the *only* lock instance, based on the settings in the
     * configuration file (if no settings found -> returns NULL!).
     *
     * @return reference to the *only* lock instance
     */
    public static function getInstance()
    {
        static $_instance = null;

        if ($_instance == null) {
            $class = "atk" . Config::getGlobal("lock_type") . "Lock";
            $file = Config::getGlobal("atkroot") . "atk/lock/class." . strtolower($class) . ".php";

            if (file_exists($file)) {
                include_once($file);
                if (class_exists($class)) {
                    Tools::atkdebug('Constructing a new lock - ' . strtolower($class));
                    $_instance = new $class();
                }
            }
        }

        return $_instance;
    }

    /**
     * Returns the unique lock ID.
     * @return the unique lock ID
     */
    function getId()
    {
        return $this->m_id;
    }

    /**
     * Locks the record with the given primary key / selector. If the
     * record is already locked the method will fail!
     *
     * @param string $selector the ATK primary key / selector
     * @param string $table the (unique) table name
     * @param string $mode mode of the lock (self::EXCLUSIVE or self::SHARED)
     *
     * @return success / failure of operation
     */
    function lock($selector, $table, $mode = self::EXCLUSIVE)
    {

    }

    /**
     * Tries to remove a lock of a certain record. Ofcourse this
     * method will fail if the lock isn't entirely ours. We also try
     * to remove any old expired locks.
     *
     * @param string $selector the ATK primary key / selector
     * @param string $table the (unique) table name
     */
    function unlock($selector, $table)
    {

    }

    /**
     * Extends the lock lease with the given ID. (This can mean multiple lock
     * leases will be extended, if there are multiple locks with the given ID!)
     *
     * @param int $identifier the unique lock ID
     *
     * @return success / failure of operation
     */
    function extend($identifier)
    {

    }

    /**
     * Checks if a certain item / record is locked or not. If so
     * we return an array with lock information. If not we return NULL.
     *
     * @param string $selector the ATK primary key / selector
     * @param string $table the (unique) table name
     *
     * @return lock information
     */
    function isLocked($selector, $table)
    {

    }
}
