<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

class ButtonAttribute extends DummyAttribute
{
    private $nodeUri;
    private $action;
    private $params;
    private $name;
    private $class;
    private $target;
    private $sessionStatus;
    private $saveForm;

    public function __construct($name, $flags = 0, $owner, $action, $options = [])
    {
        $this->name = $name;
        $this->nodeUri = $owner;
        $this->action = $action;
        // TODO: remove?
        $this->params = isset($options['params']) ? $options['params'] : [];
        $this->class = 'btn btn-default' . (isset($options['class']) ? ' ' . $options['class'] : '');
        $this->target = isset($options['target']) ? $options['target'] : null;
        $this->sessionStatus = isset($options['sessionStatus']) ? $options['sessionStatus'] : SessionManager::SESSION_NESTED;
        $this->saveForm = isset($options['saveForm']) ? $options['saveForm'] : false;

        parent::__construct($name, $flags | self::AF_HIDE_LIST | self::AF_READONLY);
    }

    public function display($record, $mode)
    {
        $ownerInstance = Atk::getInstance()->atkGetNode($this->nodeUri);
        $action = $this->action;
        if ($action == 'edit') {
            // check if the user has the permission, otherwise change in view action
            $action = $ownerInstance->allowed('edit') ? 'edit' : 'view';
        }
        $this->params['atkselector'] = $ownerInstance->getPrimaryKey($record);
        $url = Tools::dispatch_url($this->nodeUri, $action, $this->params);
        $extraProps = 'class="' . $this->class . '"';
        if ($this->target) {
            $extraProps .= ' target="' . $this->target . '"';
        }
        return Tools::href($url, Tools::atktext($this->name, $ownerInstance->getModule()), $this->sessionStatus, $this->saveForm, $extraProps);
    }

    public function addParam(string $key, string $value): self
    {
        $this->params[$key] = $value;
        return $this;
    }

    public function addParams(array $params = []): self
    {
        $this->params = array_merge($this->params, $params);
        return $this;
    }

    public function setParams($params = []): self
    {
        $this->params = $params;
        return $this;
    }

    public function setClass(string $class): self
    {
        $this->class = $class;
        return $this;
    }

    public function setTarget(string $target): self
    {
        $this->target = $target;
        return $this;
    }

    public function setSessionStatus(int $sessionStatus): self
    {
        $this->sessionStatus = $sessionStatus;
        return $this;
    }

    public function setSaveForm(bool $saveForm): self
    {
        $this->saveForm = $saveForm;
        return $this;
    }

}
