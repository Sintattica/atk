<?php

namespace App\Modules\App;

use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\TextAttribute;
use Sintattica\Atk\Security\SecurityManager;

class TestNode extends Node
{
    function __construct($nodeUri)
    {
        parent::__construct($nodeUri, Node::NF_ADD_LINK | Node::NF_EDITAFTERADD);
        $this->setTable('app_testnode');

        $this->add(new Attribute('id', Attribute::AF_AUTOKEY));
        $this->add(new Attribute('name', Attribute::AF_OBLIGATORY));
        $this->add(new TextAttribute('description'));

        $this->setDescriptorTemplate('[name]');

        //$user = SecurityManager::atkGetUser();
        //print_r($user);die;
    }
}
