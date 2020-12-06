<?php

namespace App\Modules\Auth;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\Attribute as A;
use Sintattica\Atk\Attributes\BoolAttribute;
use Sintattica\Atk\Attributes\EmailAttribute;
use Sintattica\Atk\Attributes\PasswordAttribute;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Relations\OneToManyRelation as O2M;
use Sintattica\Atk\Relations\ShuttleRelation;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;

class Users extends Node
{
    function __construct($nodeUri)
    {
        parent::__construct($nodeUri, Node::NF_ADD_LINK);

        $this->setTable('auth_users');
        $this->setOrder('[table].lastname, [table].firstname');
        $this->setDescriptorTemplate('[username]');

        $this->add(new Attribute('id', A::AF_AUTOKEY));
        $this->add(new Attribute('firstname', A::AF_FORCE_LOAD | A::AF_OBLIGATORY | A::AF_SEARCHABLE));
        $this->add(new Attribute('lastname', A::AF_FORCE_LOAD | A::AF_OBLIGATORY | A::AF_SEARCHABLE));
        $this->add(new Attribute('username', A::AF_FORCE_LOAD | A::AF_OBLIGATORY | A::AF_SEARCHABLE | A::AF_UNIQUE));

        $pwdFlags = A::AF_OBLIGATORY | A::AF_HIDE_LIST | PasswordAttribute::AF_PASSWORD_NO_VALIDATE;
        $this->add(new PasswordAttribute('passwd', $pwdFlags, true, ['minalphabeticchars' => 6, 'minnumbers' => 2]));
        $this->add(new EmailAttribute('email'));
        $this->add(new BoolAttribute('isDisabled', A::AF_SEARCHABLE | A::AF_FORCE_LOAD));
        $this->add(new BoolAttribute('isAdmin', A::AF_SEARCHABLE | A::AF_FORCE_LOAD));
        $this->add(new BoolAttribute('isU2FEnabled', A::AF_HIDE_LIST));

        $this->add(new ShuttleRelation('groups', A::AF_SEARCHABLE | A::AF_CASCADE_DELETE, 'auth.users_groups', 'auth.groups', 'user_id', 'group_id'));

        $this->add(new O2M('u2f_keys', O2M::AF_HIDE_LIST | O2M::AF_CASCADE_DELETE, 'auth.u2f', 'user_id'));
    }

    function rowColor($record)
    {
        if ($record['disabled']) {
            return '#CCCCCC';
        }
    }

    public function recordActions($record, &$actions, &$mraactions)
    {
        if ($record && is_array($record) && $this->allowed('impersonate', $record)) {

            $actionbase = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkselector=[pk]';
            $actions['impersonate'] = $actionbase.'&atkaction=impersonate';
        }
    }

    public function action_impersonate()
    {

        $isCli = php_sapi_name() === 'cli';
        if($isCli){
            die('cli not allowed');
        }

        $selector = Tools::atkArrayNvl($this->m_postvars, 'atkselector', '');
        if($selector == ''){
            die('wrong selector');
        }
        $record = $this->select($selector)->mode('view')->getFirstRow();
        if(!$record){
            die('not allowed');
        }


        if ($this->allowed('impersonate', $record)) {
            $secMan = SecurityManager::getInstance();
            $user = $secMan->m_authorization->getUser($record[Config::getGlobal('auth_userfield')]);
            if(!$user){
                die('wrong user');
            }

            $user['AUTH'] = 'impersonate';
            $sm = SessionManager::getInstance();
            $sm->globalVar('authentication', ['authenticated' => 1, 'user' => $user], true);
            $session = $sm::getSession();
            $session['login'] = 1;

            header('User: '.$user['name']);
            header('Location: '.$_SERVER['PHP_SELF']);
        }

        exit;
    }
}
