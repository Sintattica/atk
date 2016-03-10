<?php

namespace Sintattica\Atk\Security\Auth;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Security\SecurityManager;

/**
 * Driver for authentication using an imap server.
 *
 * Does not support authorization.
 *
 * @author Sandy Pleyte <sandy@ibuildings.nl>
 */
class ImapAuth extends AuthInterface
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

// if it's a virtual mail server add @<domain> to the username
        if (Config::getGlobal('auth_mail_login_type') == 'vmailmgr') {
            $user = $user.'@'.Config::getGlobal('auth_mail_suffix');
        }

        if (Config::getGlobal('auth_mail_server') == '') {
            $this->m_fatalError = Tools::atktext('auth_no_server');

            return SecurityManager::AUTH_ERROR;
        }

        $mailauth = @imap_open('{'.Config::getGlobal('auth_mail_server')
            .':'.Config::getGlobal('auth_mail_port').'}', $user, $passwd);
        // TODO/FIXME: return SecurityManager::AUTH_ERROR when connection fails..
        if ($mailauth == 0) {
            return SecurityManager::AUTH_MISMATCH;
        } else {
            imap_close($mailauth);

            return SecurityManager::AUTH_SUCCESS;
        }
    }

    /**
     * Does this authentication method support md5 encoding of passwords?
     * Imap authentication cannot support md5 encoding of passwords.
     *
     * @return bool false
     */
    public function canMd5()
    {
        return false;
    }
}
