<?php namespace Sintattica\Atk\Security\Auth;

/**
 * Dummy driver for non-authentication. When using 'none' as authentication
 * method, any loginname and any password will be accepted.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage security
 *
 */
class NoneAuth extends AuthInterface
{

    /**
     * Authenticate a user.
     *
     * @param String $user The login of the user to authenticate.
     * @param String $passwd The password of the user. Note: if the canMd5
     *                       function of an implementation returns true,
     *                       $passwd will be passed as an md5 string.
     *
     * @return int AUTH_SUCCESS - Authentication succesful
     *             AUTH_MISMATCH - Authentication failed, wrong
     *                             user/password combination
     *             AUTH_LOCKED - Account is locked, can not login
     *                           with current username.
     *             AUTH_ERROR - Authentication failed due to some
     *                          error which cannot be solved by
     *                          just trying again. If you return
     *                          this value, you *must* also
     *                          fill the m_fatalError variable.
     */
    function validateUser($user, $passwd)
    {
        if ($user == "") {
            return AUTH_SUCCESS;
        } else {
            return AUTH_MISMATCH;
        }
    }

    /**
     * This authentication method does not support md5 storage of passwords
     * since this method is not using passwords
     *
     * @return boolean false
     */
    function canMd5()
    {
        return false;
    }

}

