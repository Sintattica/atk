<?php

namespace Sintattica\Atk\Security;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;

class AccessLoggerServiceProvider
{
    public static function register(): void
    {
        $manager = SecurityManager::getInstance();

        // Logs only if logging is enabled
        if (Config::getGlobal('auth_accesslog_enabled')) {
            try {
                $logger = new AccessLogger();
                $manager->addListener($logger);
                Tools::atkdebug('AccessLogger registered successfully');
            } catch (\Exception $e) {
                Tools::atkdebug('Error registering AccessLogger: ' . $e->getMessage());
            }
        }
    }
}