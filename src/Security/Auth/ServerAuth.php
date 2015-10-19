<?php namespace Sintattica\Atk\Security\Auth;


use Sintattica\Atk\Security\SecurityManager;


/**
 * Driver for external authentication, such as Apache .htaccess files.
 *
 * With this driver, the webserver is supposed to handle the authentication.
 * Use with care. ATK will not validate anything so if the server
 * authentication is not set-up properly, this may be a security risk
 * The only check ATK makes is whether the webserver has put a valid
 * username in $_SERVER['PHP_SecurityManager::AUTH_USER'].
 *
 * @author Ivo Jansch        <ivo@achievo.org>
 * @author Gabriele Gallacci <infouser@gallacci.com>
 * @package atk
 * @subpackage security
 *
 */
class ServerAuth extends AuthInterface
{

    /**
     * Authenticate a user.
     *
     * @param String $user The login of the user to authenticate.
     * @param String $passwd The password of the user. Note: if the canMd5
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
    function validateUser($user, $passwd)
    {
        if ($_SERVER['PHP_AUTH_USER']) {
            return SecurityManager::AUTH_SUCCESS;
        } else {
            return SecurityManager::AUTH_MISMATCH;
        }
    }

    /**
     * Does this authentication method support md5 encoding of passwords?
     *
     * @return boolean false
     */
    function canMd5()
    {
        return false;
    }

}

