<?php

namespace Sintattica\Atk\Security\Auth;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Security\SecurityManager;

/**
 * Driver for authentication and authorization using entries in the
 * configurationfile.
 *
 * See the methods in the atkConfig class for an explanation of how to add
 * users and privileges.
 *
 * Does not support authorization.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ConfigAuth extends AuthInterface
{
    /**
     * Authenticate a user.
     *
     * @param string $user   The login of the user to authenticate.
     * @param string $passwd The password of the user. Note: if the canMd5
     *                       function of an implementation returns true,
     *                       $passwd will be passed as an md5 string.
     *
     * @return int SecurityManager::AUTH_SUCCESS - Authentication succesful
     *             SecurityManager::AUTH_MISMATCH - Authentication failed, wrong
     *             user/password combination
     *             SecurityManager::AUTH_LOCKED - Account is locked, can not login
     *             with current username.
     *             SecurityManager::AUTH_ERROR - Authentication failed due to some
     *             error which cannot be solved by
     *             just trying again. If you return
     *             this value, you *must* also
     *             fill the m_fatalError variable.
     */
    public function validateUser($user, $passwd)
    {
        if ($user == '') {
            return SecurityManager::AUTH_UNVERIFIED;
        } // can't verify if we have no userid

        $configUser = Config::getGlobal('user');

        if ($user != '' && $passwd != '' && $configUser[$user]['password'] == $passwd) {
            return SecurityManager::AUTH_SUCCESS;
        } else {
            return SecurityManager::AUTH_MISMATCH;
        }
    }

    /**
     * Does the authentication method support md5 encoding of passwords?
     *
     * @return bool True if md5 is always used. false if md5 is not
     *              supported.
     *              Drivers that support both md5 and cleartext passwords
     *              can return Config::getGlobal("authentication_md5") to let the
     *              application decide whether to use md5.
     */
    public function canMd5()
    {
        return Config::getGlobal('authentication_md5');
    }

    /**
     * This function returns information about a user in an associative
     * array with the following elements:
     * "name" -> the userid (should normally be the same as the $user
     *           variable that gets passed to it.
     * "level" -> The level/group(s) to which this user belongs.
     * Specific implementations of the method may add more information if
     * necessary.
     *
     * @param string $user The login of the user to retrieve.
     *
     * @return array Information about a user.
     */
    public function getUser($user)
    {
        $configUser = Config::getGlobal('user');

        return array('name' => $user, 'level' => $configUser[$user]['level']);
    }

    /**
     * This function returns the level/group(s) that are allowed to perform
     * the given action on a node.
     *
     * @param string $node   The full nodename of the node for which to check
     *                       the privilege. (modulename.nodename)
     * @param string $action The privilege to check.
     *
     * @return mixed One (int) or more (array) entities that are allowed to
     *               perform the action.
     */
    public function getEntity($node, $action)
    {
        $access = Config::getGlobal('access');
        $rights = $access[$node];

        $result = array();

        for ($i = 0; $i < count($rights); ++$i) {
            if ($rights[$i][$action] != '') {
                $result[] = $rights[$i][$action];
            }
            if ($rights[$i]['*'] != '') {
                $result[] = $rights[$i]['*'];
            }
        }

        return $result;
    }

    /**
     * This function returns the level/group(s) that are allowed to
     * view/edit a certain attribute of a given node.
     *
     * @param string $node   The full nodename of the node for which to check
     *                       attribute access.
     * @param string $attrib The name of the attribute to check
     * @param string $mode   "view" or "edit"
     *
     * @return array
     */
    public function getAttribEntity($node, $attrib, $mode)
    {
        $attribrestrict = Config::getGlobal('attribrestrict');

        // $entity is an array of entities that may do $mode on $node.$attrib.
        $entity = $attribrestrict[$node][$attrib][$mode];

        return $entity;
    }

    /**
     * This function returns "get password" policy for current auth method.
     *
     * @return int const
     */
    public function getPasswordPolicy()
    {
        return self::PASSWORD_RETRIEVABLE;
    }

    /**
     * This function returns password or false, if password can't be retrieve/recreate.
     *
     * @param string $username User for which the password should be regenerated
     *
     * @return mixed string with password or false
     */
    public function getPassword($username)
    {
        $configUser = Config::getGlobal('user');
        if (isset($configUser[$username]['password'])) {
            return $configUser[$username]['password'];
        }

        return false;
    }
}
