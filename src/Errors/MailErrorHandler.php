<?php

namespace Sintattica\Atk\Errors;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Security\SecurityManager;

/**
 * Handles errors by sending them to a specified email address.
 *
 * Params used:
 * - mailto: The email address the errors will be mailed to
 */
class MailErrorHandler extends ErrorHandlerBase
{
    protected function _wordwrap($line)
    {
        return wordwrap($line, 100, "\n", 1);
    }

    /**
     * Handle the error.
     *
     * @param string $errorMessage
     * @param string $debugMessage
     */
    public function handle($errorMessage, $debugMessage)
    {
        $sessionManager = SessionManager::getInstance();

        $sessionData = &SessionManager::getSession();

        $txt_app_title = Tools::atktext('app_title');

        if ($this->params['mailto'] != '') { // only if enabled..

            $atk = Atk::getInstance();

            $subject = '['.$_SERVER['SERVER_NAME']."] $txt_app_title error";

            $defaultfrom = sprintf('%s <%s@%s>', $txt_app_title, Config::getGlobal('identifier', 'atk'), $_SERVER['SERVER_NAME']);
            $from = Config::getGlobal('mail_sender', $defaultfrom);

            $body = "Hello,\n\nAn error seems to have occurred in the atk application named '$txt_app_title'.\n";
            $body .= "\nThe errormessage was:\n\n".implode("\n", is_array($errorMessage) ? $errorMessage : array())."\n";
            $body .= "\nA detailed report follows:\n";
            $body .= "\nPHP Version: ".phpversion()."\n\n";

            $body .= "\nDEBUGMESSAGES\n".str_repeat('-', 70)."\n";

            $lines = [];
            for ($i = 0, $_ = Tools::count($debugMessage); $i < $_; ++$i) {
                $lines[] = $this->_wordwrap(Tools::atk_html_entity_decode(preg_replace('(\[<a.*</a>\])', '', $debugMessage[$i])));
            }
            $body .= implode("\n", $lines);

            if (is_array($_GET)) {
                $body .= "\n\n_GET\n".str_repeat('-', 70)."\n";
                foreach ($_GET as $key => $value) {
                    $body .= $this->_wordwrap($key.str_repeat(' ', max(1, 20 - strlen($key))).' = '.var_export($value, 1))."\n";
                }
            }

            if (function_exists('getallheaders')) {
                $request = getallheaders();
                if (Tools::count($request) > 0) {
                    $body .= "\n\nREQUEST INFORMATION\n".str_repeat('-', 70)."\n";
                    foreach ($request as $key => $value) {
                        $body .= $this->_wordwrap($key.str_repeat(' ', max(1, 30 - strlen($key))).' = '.var_export($value, 1))."\n";
                    }
                }
            }

            if (is_array($_POST)) {
                $body .= "\n\n_POST\n".str_repeat('-', 70)."\n";
                foreach ($_POST as $key => $value) {
                    $body .= $this->_wordwrap($key.str_repeat(' ', max(1, 20 - strlen($key))).' = '.var_export($value, 1))."\n";
                }
            }

            if (is_array($_COOKIE)) {
                $body .= "\n\n_COOKIE\n".str_repeat('-', 70)."\n";
                foreach ($_COOKIE as $key => $value) {
                    $body .= $this->_wordwrap($key.str_repeat(' ', max(1, 20 - strlen($key))).' = '.var_export($value, 1))."\n";
                }
            }

            $body .= "\n\nATK CONFIGURATION\n".str_repeat('-', 70)."\n";
            foreach ($GLOBALS as $key => $value) {
                if (substr($key, 0, 7) == 'config_') {
                    $body .= $this->_wordwrap($key.str_repeat(' ', max(1, 30 - strlen($key))).' = '.var_export($value, 1))."\n";
                }
            }

            $body .= "\n\nMODULE CONFIGURATION\n".str_repeat('-', 70)."\n";
            foreach ($atk->g_modules as $modname => $modpath) {
                $modexists = file_exists($modpath) ? ' (path exists)' : ' (PATH DOES NOT EXIST!)';
                $body .= $this->_wordwrap($modname.':'.str_repeat(' ', max(1, 20 - strlen($modname))).var_export($modpath, 1).$modexists)."\n";
            }

            $body .= "\n\nCurrent User:\n".str_repeat('-', 70)."\n";
            $user = SecurityManager::atkGetUser();
            if (is_array($user) && Tools::count($user)) {
                foreach ($user as $key => $value) {
                    $body .= $this->_wordwrap($key.str_repeat(' ', max(1, 30 - strlen($key))).' = '.var_export($value, 1))."\n";
                }
            } else {
                $body .= "Not known\n";
            }

            if (is_object($sessionManager)) {
                $body .= "\n\nATK SESSION\n".str_repeat('-', 70);
                if (isset($sessionData['stack'])) {
                    $stack = $sessionData['stack'];
                    for ($i = 0; $i < Tools::count($stack); ++$i) {
                        $body .= "\nStack level $i:\n";
                        $item = isset($stack[$i]) ? $stack[$i] : null;
                        if (is_array($item)) {
                            foreach ($item as $key => $value) {
                                $body .= $this->_wordwrap($key.str_repeat(' ', max(1, 30 - strlen($key))).' = '.var_export($value, 1))."\n";
                            }
                        }
                    }
                }
                if (isset($sessionData['globals'])) {
                    $ns_globals = $sessionData['globals'];
                    if (Tools::count($ns_globals) > 0) {
                        $body .= "\nNamespace globals:\n";
                        foreach ($ns_globals as $key => $value) {
                            $body .= $this->_wordwrap($key.str_repeat(' ', max(1, 30 - strlen($key))).' = '.var_export($value, 1))."\n";
                        }
                    }
                }
            }

            $body .= "\n\nSERVER INFORMATION\n".str_repeat('-', 70)."\n";

            foreach ($_SERVER as $key => $value) {
                $body .= $this->_wordwrap($key.str_repeat(' ', max(1, 20 - strlen($key))).' = '.var_export($value, 1))."\n";
            }

            //TODO: replace with some mailer object
            mail($this->params['mailto'], $subject, $body, "From: $from");
        }
    }
}
