<?php

namespace Sintattica\Atk\Core;

use League\Container\Container;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\IndexPage;

class Atk
{

    private $container;


    public function __construct()
    {
        Bootstrap::run();
        /*
        $this->container = $c = new Container();
        //  $this->container->share('sessionManager', 'Sintattica\Atk\Session\SessionManager');

        $c->share('Menu', 'Sintattica\Atk\Core\Menu');

        $c->share('IndexPage', 'Sintattica\Atk\Ui\IndexPage')
            ->withArgument('Menu');

        $c->share('ModuleManager', 'Sintattica\Atk\Core\ModuleManager');
        */
    }

    public function run()
    {
        if (Config::getGlobal('session_init', true)) {
            $sessionManager = SessionManager::getInstance();
            $sessionManager->start();
        }

        SecurityManager::atksecure();
        $indexpage = new IndexPage();
        $indexpage->m_page->register_style('styles/app.css');
        $indexpage->generate();
    }

    public function registerModule($moduleClass)
    {
        Module::module($moduleClass);

    }
}
