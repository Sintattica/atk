<?php namespace Sintattica\Atk\Utils;
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage utils
 *
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision:
 * $Id$
 */

/**
 * Utility for sending e-mails.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @package atk
 * @subpackage utils
 *
 * @deprecated
 * @see atkMailer
 */
class Atk_Mail
{

    /**
     * Wrapper for the PHP mail function which accepts the exact same parameters
     * as the normal mail function does but adds the ability to disable sending
     * e-mails using a configuration variable.
     *
     * @return bool mail succesfully sent?
     * @static
     *
     * @deprecated
     * @see atkMailer
     */
    function mail()
    {
        if (Atk_Config::getGlobal("mail_enabled", true)) {
            $args = func_get_args();
            return call_user_func_array("mail", $args);
        } else {
            return true;
        }
    }

}

