<?php

namespace App\Modules\Auth;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\Attribute as A;
use Sintattica\Atk\Attributes\ProfileAttribute;
use Sintattica\Atk\Attributes\TextAttribute;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Relations\ShuttleRelation;

class Groups extends Node
{
    function __construct($nodeUri)
    {
        parent::__construct($nodeUri, Node::NF_ADD_LINK | Node::NF_EDITAFTERADD);
        $this->setTable('auth_groups');
        $this->setOrder('name');
        $this->setDescriptorTemplate('[name]');

        $this->add(new Attribute('id', A::AF_AUTOKEY));
        $this->add(new Attribute('name', A::AF_OBLIGATORY | A::AF_UNIQUE | A::AF_SEARCHABLE));
        $this->add(new TextAttribute('description'));
        $this->add(new ShuttleRelation('users', A::AF_HIDE_LIST | A::AF_HIDE_ADD, 'auth.users_groups', 'auth.users', 'group_id', 'user_id'));
        $this->add(new ProfileAttribute('accessrights', A::AF_BLANKLABEL | A::AF_HIDE_ADD));
    }
}
