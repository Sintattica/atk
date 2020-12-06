<?php

namespace App\Modules\Auth;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\HiddenAttribute;
use Sintattica\Atk\Attributes\NumberAttribute;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Handlers\ActionHandler;
use Sintattica\Atk\Relations\ManyToOneRelation as M2O;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;

class U2F extends Node
{
    public function __construct($nodeUri)
    {
        parent::__construct($nodeUri);
        $this->setTable(Config::getGlobal('auth_u2f_dbtable'));
        $this->setDescriptorTemplate('name');

        $this->add(new NumberAttribute('id', NumberAttribute::AF_AUTOKEY));
        $this->add(new M2O(Config::getGlobal('auth_userfk'), M2O::AF_HIDE_LIST | M2O::AF_READONLY_EDIT, Config::getGlobal('auth_usernode')));
        $this->add(new Attribute('name', Attribute::AF_SEARCHABLE | Attribute::AF_OBLIGATORY))->setInitialValue('mykey');
        $this->add(new U2FRegisterAttribute('u2f_register'));
    }

    public function getFormButtons($mode, $record = [])
    {
        if ($mode != 'add') {
            return parent::getFormButtons($mode, $record);
        }

        $sm = SessionManager::getInstance();
        $page = $this->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/tools.js');
        $result = [];

        $result[] = '<button type="button" id="u2f_register_button" class="btn btn-primary">'.$this->text('u2f_register').'</button>';

        if ($sm->atkLevel() > 0 || Tools::hasFlag(Tools::atkArrayNvl($this->m_feedback, 'save', 0), ActionHandler::ACTION_CANCELLED)) {
            $result[] = $this->getButton('cancel');
        }

        return $result;
    }

    public function postAdd($record, $mode = 'add')
    {
        $u2f_regReq = isset($_SESSION['u2f_regReq']) ? $_SESSION['u2f_regReq'] : null;
        unset($_SESSION['u2f_regReq']);
        if (!$u2f_regReq) {
            return false;
        }

        try {
            $securityManager = SecurityManager::getInstance();
            $reg = $securityManager->getU2F()->doRegister(json_decode($u2f_regReq), json_decode($record['u2f_register']));
            $db = $this->getDb();
            $tbl = $this->getTable();
            $stmt = $db->prepare("UPDATE `$tbl` SET keyHandle=?, publicKey=?, certificate=?, counter=? WHERE id = ?");
            $stmt->execute([$reg->keyHandle, $reg->publicKey, $reg->certificate, $reg->counter, $record['id']]);

            return true;

        } catch (\Exception $e) {
            return false;
        }
    }
}

class U2FRegisterAttribute extends HiddenAttribute
{
    public function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags, '');
        $this->setLoadType(self::NOLOAD)->setStorageType(self::NOSTORE);
    }

    public function hide($record, $fieldprefix, $mode)
    {
        if ($mode == 'add') {
            $page = $this->getOwnerInstance()->getPage();
            $page->register_script(Config::getGlobal('assets_url').'javascript/u2f-api.js');
            $htmlId = $this->getHtmlId($fieldprefix);
            $user_id = $record[Config::getGlobal('auth_userfk')]['id'];

            $securityManager = SecurityManager::getInstance();
            $registrations = $securityManager->u2fGetRegistrations($user_id);
            $data = $securityManager->getU2F()->getRegisterData($registrations);

            list($req, $sigs) = $data;
            $_SESSION['u2f_regReq'] = json_encode($req);

            $req = json_encode($req);
            $sigs = json_encode($sigs);

            $script = <<<EOF
jQuery('#u2f_register_button').click(function(el){
    el.preventDefault();
    var req = $req;
    var sigs = $sigs;
    u2f.register([req], sigs, function(data) {
        var form = document.getElementById('entryform');
        var reg = document.getElementById('$htmlId');
        var atksubmitaction = jQuery(form).find('input[type="hidden"].atksubmitaction');
        atksubmitaction.attr('name', 'atksaveandclose').val('atksaveandclose');
        
        if(data.errorCode && data.errorCode != 0) {
            alert("registration failed with error code: " + data.errorCode);
            return;
        }
        reg.value = JSON.stringify(data);
        form.submit();
    });
});
EOF;
            $page->register_loadscript($script);
        }

        return parent::hide($record, $fieldprefix, $mode);
    }
}
