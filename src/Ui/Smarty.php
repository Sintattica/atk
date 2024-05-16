<?php

namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Config;
use Smarty\Smarty as SmartyBase;

class Smarty extends SmartyBase
{
    public $_file_perms;

    public function __construct()
    {
        parent::__construct();

        $this->_file_perms = Config::getGlobal('smarty_file_perms');
    }
}
