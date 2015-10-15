<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage lock
 *
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 6323 $
 * $Id$
 */

/**
 * Locking driver that used a database table to store the locks.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage lock
 */
class Atk_DbLock extends Atk_Lock
{

    /**
     * Initializes the lock. Checks the session if there aren't any
     * registered locks, and if so registers the script and retrieves
     * the unique lock ID.
     */
    public function __construct()
    {
        global $ATK_VARS;

        $atklockList = $this->loadLockList();
        if (!is_array($atklockList)) {
            $atklockList = array();
        }

        /* first time (session or stack)! */
        if (!is_array($atklockList[Atk_SessionManager::atkStackID()]) || count($atklockList[Atk_SessionManager::atkStackID()]["stack"]) == 0) {
            $atklockList[Atk_SessionManager::atkStackID()] = array("id" => 0, "stack" => array());
            $atklock = &$atklockList[Atk_SessionManager::atkStackID()];
        } /* check if some locks need to be removed */ else {
            $atklock = &$atklockList[Atk_SessionManager::atkStackID()];
            $this->m_id = (int)$atklock["id"];

            for ($i = 0, $_i = count($atklock["stack"]) - Atk_SessionManager::atkLevel(); $i < $_i; $i++) {
                $selectorList = array_pop($atklock["stack"]);
                if (is_array($selectorList)) {
                    foreach ($selectorList as $selector => $table) {
                        $this->unlock($selector, $table);
                    }
                }
            }

            if ($ATK_VARS["atklevel"] == -2) {
                $selectorList = array_pop($atklock["stack"]);
                if (is_array($selectorList)) {
                    foreach ($selectorList as $selector => $table) {
                        $this->unlock($selector, $table);
                    }
                }
            }

            $empty = true;
            for ($i = 0, $_i = count($atklock["stack"]); $i < $_i; $i++) {
                if (count($atklock["stack"][$i]) > 0) {
                    $empty = false;
                }
            }
            if ($empty) {
                $atklock["id"] = 0;
            }
        }

        for ($i = count($atklock["stack"]); $i < Atk_SessionManager::atkLevel(); $i++) {
            $atklock["stack"][] = array();
        }
        $this->m_id = (int)$atklock["id"];
        $this->storeLockList($atklockList);

        if (!isset($ATK_VARS['atkpartial']) && $this->m_id > 0) {
            $page = Atk_Tools::atkinstance("atk.ui.atkpage");
            $page->register_script(Atk_Config::getGlobal("atkroot") . "atk/javascript/xml.js");
            $page->register_script(Atk_Tools::session_url("include.php?file=atk/lock/lock.js.php&stack=" . Atk_SessionManager::atkStackID() . "&id=" . $this->m_id,
                SESSION_NEW));
        }
    }

    /**
     * Load lock data from session.
     */
    function loadLockList()
    {
        global $g_sessionManager;
        $atklockList = $g_sessionManager->getValue("atklock", "globals");
        return $atklockList;
    }

    /**
     * Store lock data in session.
     *
     * @param array $list The lock list
     */
    function storeLockList($list)
    {
        global $g_sessionManager;
        $g_sessionManager->globalVar("atklock", $list, true);
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
        global $g_sessionManager;
        $success = false;

        /* first check if we haven't locked the item already */
        $atklockList = $this->loadLockList();
        if (is_array($atklockList)) {
            $atklock = $atklockList[Atk_SessionManager::atkStackID()];
            for ($i = Atk_SessionManager::atkLevel() - 1, $_i = count($atklock["stack"]); $i < $_i; $i++) {
                if (is_array($atklock["stack"][$i]) && in_array($selector, array_keys($atklock["stack"][$i]))) {
                    return true;
                }
            }
        }

        /* lock the lock table :) */
        $db = Atk_Tools::atkGetDb();
        $db->lock("db_lock");

        /* check if the item can be locked */
        if ($mode == self::EXCLUSIVE) {
            $query = &$db->createQuery();
            $query->addField("*");
            $query->addTable("db_lock");
            $query->addCondition("lock_table = '" . Atk_Tools::escapeSQL($table) . "'");
            $query->addCondition("lock_record = '" . Atk_Tools::escapeSQL($selector) . "'");
            $query->addCondition("lock_lease >= " . $db->func_now());
            $query->addCondition("session_id <> '" . Atk_Tools::escapeSQL(session_id()) . "'");
            $result = $db->getRows($query->buildCount());
        }

        /* lock item */
        if ($mode == self::SHARED || $result[0]["count"] == 0) {
            if ($this->m_id <= 0) {
                $this->m_id = $db->nextid("db_lock");
                $page = Atk_Page::getInstance();
                $page->register_script(Atk_Config::getGlobal("atkroot") . "atk/javascript/xml.js");
                $page->register_script(Atk_Tools::session_url("include.php?file=atk/lock/lock.js.php&stack=" . Atk_SessionManager::atkStackID() . "&id=" . $this->m_id,
                    SESSION_NEW));
            }

            $user = Atk_SecurityManager::atkGetUser();
            if (is_array($user)) {
                $user = $user['name'];
            }

            $query = &$db->createQuery();
            $query->addField("lock_id", $this->m_id);
            $query->addField("lock_table", Atk_Tools::escapeSQL($table));
            $query->addField("lock_record", Atk_Tools::escapeSQL($selector));
            $query->addField("user_id", Atk_Tools::escapeSQL($user));
            $query->addField("user_ip", Atk_Tools::escapeSQL(Atk_Tools::atkGetClientIp()));
            $query->addField("lock_stamp", $db->func_now(), "", "", false);
            $dbconfig = Atk_Config::getGlobal("db");
            if (substr($dbconfig["default"]["driver"], 0, 3) != 'oci') {
                $query->addField("lock_lease", $db->func_now() . " + INTERVAL 60 SECOND", "", "", false);
            } else {
                $query->addField("lock_lease", $db->func_now() . " +  " . (60 / 86400), "", "", false);
            }
            $query->addField("lock_lease_count", "0");
            $query->addField("session_id", Atk_Tools::escapeSQL(session_id()));
            $query->addTable("db_lock");
            $query->executeInsert();

            $atklockList = $this->loadLockList();
            $atklock = &$atklockList[Atk_SessionManager::atkStackID()];
            $atklock["id"] = (int)$this->m_id;
            $selectorList = array_pop($atklock["stack"]);
            $selectorList[$selector] = $table;
            $atklock["stack"][] = $selectorList;
            $this->storeLockList($atklockList);

            $db->commit();

            $success = true;
        }

        /* unlock the lock table */
        $db->unlock("db_lock");

        /* return */
        return $success;
    }

    /**
     * Tries to remove a lock from a certain record. Ofcourse this
     * method will fail if the lock isn't entirely ours. We also try
     * to remove any old expired locks.
     *
     * @param string $selector the ATK primary key / selector
     * @param string $table the (unique) table name
     */
    function unlock($selector, $table)
    {
        $db = Atk_Tools::atkGetDb();

        $user = Atk_SecurityManager::atkGetUser();
        if (is_array($user)) {
            $user = $user['name'];
        }

        /* lock the lock table :) */
        $db->lock("db_lock");

        /* extend lock lease */
        $query = &$db->createQuery();
        $query->addTable("db_lock");
        $query->addCondition
        (
            "(" .
            "lock_id = '" . (int)$this->m_id . "' AND " .
            "lock_table = '" . Atk_Tools::escapeSQL($table) . "' AND " .
            "lock_record = '" . Atk_Tools::escapeSQL($selector) . "' AND " .
            "user_id = '" . Atk_Tools::escapeSQL($user) . "' AND " .
            "user_ip = '" . Atk_Tools::escapeSQL(Atk_Tools::atkGetClientIp()) . "' AND " .
            "session_id = '" . Atk_Tools::escapeSQL(session_id()) . "'" .
            ") OR (" .
            "lock_lease <= " . $db->func_now() .
            ")"
        );
        $result = $db->query($query->buildDelete());

        /* unlock the lock table */
        $db->unlock("db_lock");
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
        global $ATK_VARS;

        if (!empty($ATK_VARS["stack"])) {
            $atkstackid = $ATK_VARS["stack"];
        } else {
            $atkstackid = Atk_SessionManager::atkStackID();
        }

        $user = Atk_SecurityManager::atkGetUser();
        if (is_array($user)) {
            $user = $user['name'];
        }

        /* lock the lock table :) */
        $db = Atk_Tools::atkGetDb();
        $db->lock("db_lock");

        /* extend lock lease */
        $query = &$db->createQuery();
        $dbconfig = Atk_Config::getGlobal("db");
        if (substr($dbconfig["default"]["driver"], 0, 3) != 'oci') {
            $query->addField("lock_lease", $db->func_now() . " + INTERVAL 60 SECOND", "", "", false);
        } else {
            $query->addField("lock_lease", $db->func_now() . " +  " . (60 / 86400), "", "", false);
        }
        $query->addField("lock_lease_count", "lock_lease_count + 1", "", "", false);
        $query->addTable("db_lock");
        $query->addCondition("lock_id = " . (int)$identifier);
        $query->addCondition("user_id = '" . Atk_Tools::escapeSQL($user) . "'");
        $query->addCondition("user_ip = '" . Atk_Tools::escapeSQL(Atk_Tools::atkGetClientIp()) . "'");
        $query->addCondition("session_id = '" . Atk_Tools::escapeSQL(session_id()) . "'");
        $query->addCondition("lock_lease >= " . $db->func_now());
        $query->executeUpdate();
        $result = $db->affected_rows();

        /* unlock the lock table */
        $db->unlock("db_lock");

        /* reset lock ID in session */
        if ($result <= 0) {
            $atklockList = $this->loadLockList();
            if (is_array($atklockList)) {
                unset($atklockList[$atkstackid]);
            }
            $this->storeLockList($atklockList);
        }

        /* return */
        return $result > 0;
    }

    /**
     * Checks if a certain item / record is locked or not. If so
     * we return an array with lock information. If not we return NULL.
     *
     * @param string $selector the ATK primary key / selector
     * @param string $table the (unique) table name
     * @param string $mode mode of the lock (self::EXCLUSIVE or self::SHARED)
     *
     * @return lock information
     */
    function isLocked($selector, $table, $mode = self::EXCLUSIVE)
    {
        static $_cache = array();

        /* first check if we haven't locked the item ourselves already */
        $atklockList = $this->loadLockList();
        if (is_array($atklockList)) {
            $atklock = $atklockList[Atk_SessionManager::atkStackID()];
            for ($i = Atk_SessionManager::atkLevel() - 1, $_i = count($atklock["stack"]); $i < $_i; $i++) {
                if (is_array($atklock["stack"][$i]) && in_array($selector, array_keys($atklock["stack"][$i]))) {
                    return null;
                }
            }
        }

        /* select all locks for the node table -> cache */
        if (!is_array($_cache[$table])) {
            $db = Atk_Tools::atkGetDb();
            $query = &$db->createQuery();
            $query->addField("lock_id");
            $query->addField("lock_record");
            $query->addField("user_id");
            $query->addField("user_ip");
            $query->addField("lock_stamp");
            $query->addField("lock_lease");
            $query->addTable("db_lock");
            $query->addCondition("lock_table = '" . Atk_Tools::escapeSQL($table) . "'");
            $query->addCondition("lock_lease >= " . $db->func_now());
            if ($mode == self::EXCLUSIVE) {
                $query->addCondition("session_id <> '" . Atk_Tools::escapeSQL(session_id()) . "'");
            }
            $_cache[$table] = $query->executeSelect();
        }

        /* search for lock */
        $locks = array();
        for ($i = 0, $_i = count($_cache[$table]); $i < $_i; $i++) {
            if ($_cache[$table][$i]["lock_record"] == $selector) {
                $locks[] = $_cache[$table][$i];
            }
        }

        return count($locks) > 0 ? $locks : false;
    }

}

