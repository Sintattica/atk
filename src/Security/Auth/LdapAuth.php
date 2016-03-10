<?php namespace Sintattica\Atk\Security\Auth;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Security\SecurityManager;

/**
 * Driver for authentication using an ldap server.
 *
 * Does not support authorization.
 *
 * @author Sandy Pleyte <sandy@achievo.org>
 * @package atk
 * @subpackage security
 *
 */
class LdapAuth extends AuthInterface
{

    /**
     * Authenticate a user.
     *
     * @param string $user The login of the user to authenticate.
     * @param string $passwd The password of the user. Note: if the canMd5
     *                       function of an implementation returns true,
     *                       $passwd will be passed as an md5 string.
     *
     * @return int SecurityManager::AUTH_SUCCESS - Authentication succesful
     *             SecurityManager::AUTH_MISMATCH - Authentication failed, wrong
     *                             user/password combination
     *             SecurityManager::AUTH_LOCKED - Account is locked, can not login
     *                           with current username.
     *             SecurityManager::AUTH_ERROR - Authentication failed due to some
     *                          error which cannot be solved by
     *                          just trying again. If you return
     *                          this value, you *must* also
     *                          fill the m_fatalError variable.
     */
    public function validateUser($user, $passwd)
    {
        if ($user == "") {
            return SecurityManager::AUTH_UNVERIFIED;
        } // can't verify if we have no userid

        if ($ldap = ldap_connect(Config::getGlobal("auth_ldap_host"))) {
            Tools::atkdebug("successful connection to ".Config::getGlobal("auth_ldap_host"));
            if (Config::getGlobal("auth_ldap_bind_tree")) {
                if ($bindID = @ldap_bind($ldap, Config::getGlobal("auth_ldap_bind_dn"), Config::getGlobal("auth_ldap_bind_pw"))) {
                    Tools::atkdebug("Succesfully bound to ".Config::getGlobal("auth_ldap_bind_dn")." with id: ".$bindID." conn_id ".$ldap);
                } else {
                    Tools::atkdebug("<b>Error binding to</b> ".Config::getGlobal("auth_ldap_bind_dn")." ".Config::getGlobal("auth_ldap_bind_pw"));

                    return SecurityManager::AUTH_ERROR;
                }
            }

            // find the dn for this uid, the uid is not always in the dn
            $filter = (Config::getGlobal("auth_ldap_search_filter") != "" ? Config::getGlobal("auth_ldap_search_filter") : "uid");
            $pattern = Config::getGlobal("auth_ldap_context");

            // Add support for searching in multiple DN's
            if (!is_array($pattern)) {
                $pattern = array($pattern);
            }

            foreach ($pattern as $searchPattern) {
                $filterCmd = $filter."=".$user;
                $sri = @ldap_search($ldap, $searchPattern, $filterCmd);

                if ($sri === false) {
                    Tools::atkdebug("Invalid searchpattern: ".$searchPattern);
                } else {
                    $allValues = ldap_get_entries($ldap, $sri);

                    if ($allValues["count"] > 0) {
                        // we only care about the first dn
                        $userDN = $allValues[0]["dn"];

                        // generate a bogus password to pass if the user doesn't give us one
                        // this gets around systems that are anonymous search enabled
                        if (empty($passwd)) {
                            $passwd = crypt(microtime());
                        }

                        // try to bind as the user with user suplied password
                        if (@ldap_bind($ldap, $userDN, $passwd)) {
                            return SecurityManager::AUTH_SUCCESS;
                        }
                    }
                }
            }
            Tools::atkdebug("LDAP did not successfully authenticate $user");

            // dn not found or password wrong TODO/FIXME: return -1 if dn not found
            return SecurityManager::AUTH_MISMATCH;
        } else {
            return SecurityManager::AUTH_ERROR;
        }
    }

    /**
     * Ldap can't handle passwords as md5
     *
     * @return boolean false
     */
    public function canMd5()
    {
        return false; // ?? Is this correct? can we store passwords as md5 in ldap?
    }
}
