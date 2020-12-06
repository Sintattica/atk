<?php

namespace App\Modules\Auth;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Relations\ManyToOneRelation;

class Users_Groups extends Node
{
    function __construct($nodeUri)
    {
        parent::__construct($nodeUri);
        $this->setTable('auth_users_groups');

        $this->add(new ManyToOneRelation('user_id', Attribute::AF_PRIMARY, 'auth.users'));
        $this->add(new ManyToOneRelation('group_id', Attribute::AF_PRIMARY, 'auth.groups'));
    }
}
