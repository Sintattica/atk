<?php

namespace App\Modules\App;

class Module extends \Sintattica\Atk\Core\Module
{
    static $module = 'app';

    public function register()
    {
        $this->registerNode('testNode', TestNode::class, ['admin', 'add', 'edit', 'delete']);
    }

    public function boot()
    {
        $this->addNodeToMenu('testNode', 'testNode', 'admin');
    }
}
