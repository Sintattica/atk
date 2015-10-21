<?php namespace Sintattica\Atk\Security;

/**
 * The atkMockSecurityManager class is an atkSecurityManager mock
 * object for testing purposes
 *
 * The most important feature of the atkMockSecurityManager is the
 * ability to influence the result of each function call.
 *
 * @todo mock every function call. This can't be done nicely until
 * we feature PHP5. For now, we add mock methods on a per-need basis
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage security
 */
class MockSecurityManager extends SecurityManager
{
    /**
     * Set the nodepriviledges
     *
     * @var array
     */
    var $m_resultallowed = array();

    /**
     * Set which privileges are allowed
     *
     * @param bool $result
     * @param string $nodeprivilege
     */
    function setAllowed($result, $nodeprivilege = "all")
    {
        $this->m_resultallowed[$nodeprivilege] = $result;
    }

    /**
     * Check if the currently logged-in user has a certain privilege on a
     * node.
     * @param String $node The full nodename of the node for which to check
     *                     access privileges. (modulename.nodename notation).
     * @param String $privilege The privilege to check (atkaction).
     * @return boolean True if the user has the privilege, false if not.
     */
    function allowed($node, $privilege)
    {
        if (isset($this->m_resultallowed["all"])) {
            return $this->m_resultallowed["all"];
        }
        if (isset($this->m_resultallowed[$node . "." . $privilege])) {
            return $this->m_resultallowed[$node . "." . $privilege];
        }
        return parent::allowed($node, $privilege);
    }

}

