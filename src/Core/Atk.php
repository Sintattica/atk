<?php

namespace Sintattica\Atk\Core;


use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\IndexPage;

class Atk
{

    public function __construct()
    {
        Bootstrap::run();
    }

    public function run()
    {

        $sessionManager = SessionManager::getInstance();
        $sessionManager->start();

        $securityManager = SecurityManager::getInstance();
        if ($securityManager->authenticate()) {
            $indexPage = new IndexPage();
            $indexPage->generate();
        }

    }

    public function registerModule($moduleClass)
    {
        Module::module($moduleClass);

    }
}
