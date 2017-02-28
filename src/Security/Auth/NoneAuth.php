<?php

namespace Sintattica\Atk\Security\Auth;

use Sintattica\Atk\Security\SecurityManager;

/**
 * Dummy driver for non-authentication. When using 'none' as authentication
 * method, any loginname and any password will be accepted.
 */
class NoneAuth extends AuthInterface
{
    public function validateUser($user, $passwd)
    {
        return SecurityManager::AUTH_SUCCESS;
    }

    public function isValidUser($user)
    {
        return true;
    }

    public function getUser($user)
    {
        $sm = SecurityManager::getInstance();
        return $sm->getSystemUser('administrator');
    }
}
