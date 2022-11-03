<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

class ActionButtonAttribute extends ButtonAttribute
{
    private $node = '';
    private $action = '';
    private $params = [];
    private $target = null;
    private $sessionStatus = SessionManager::SESSION_NESTED;
    private $saveForm = false;

    public function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags | self::AF_HIDE_LIST | self::AF_READONLY);
    }

    public function display($record, $mode)
    {
        $classes = implode(' ', $this->m_cssclasses);
        $tranlatedText = $this->text($this->m_text);

        $targetNode = Atk::getInstance()->atkGetNode($this->node);
        $action = $this->action;
        if ($action === 'edit' and !$targetNode->allowed('edit')) {
            // action edit but user has not access rights: goes to view page
            $action = 'view';
        }
        if (empty($this->params)) {
            // default params
            $this->params['atkselector'] = $targetNode->getPrimaryKey($record);
        }
        $url = Tools::dispatch_url($this->node, $action, $this->params);
        $extraProps = [];
        if ($classes) {
            $extraProps[] = 'class="' . $classes . '"';
        }
        if ($this->target) {
            $extraProps[] = 'target="' . $this->target . '"';
        }
        return Tools::href($url, $tranlatedText, $this->sessionStatus, $this->saveForm, implode(' ', $extraProps));
    }

    public function addParam($key, $value): self
    {
        $this->params[$key] = $value;
        return $this;
    }

    public function addParams($params = []): self
    {
        $this->params = array_merge($this->params, $params);
        return $this;
    }

    public function setParams($params = []): self
    {
        $this->params = $params;
        return $this;
    }

    public function getParams(): array
    {
        return $this->params;
    }

    public function getNode(): string
    {
        return $this->node;
    }

    public function setNode(string $node): self
    {
        $this->node = $node;
        return $this;
    }

    public function getAction(): string
    {
        return $this->action;
    }

    public function setAction($action): self
    {
        $this->action = $action;
        return $this;
    }

    public function getTarget(): string
    {
        return $this->target;
    }

    public function setTarget($target): self
    {
        $this->target = $target;
        return $this;
    }

    public function getType(): string
    {
        return self::TYPE_ACTION;
    }

    public function getSessionStatus(): int
    {
        return $this->sessionStatus;
    }

    public function setSessionStatus(int $sessionStatus): self
    {
        $this->sessionStatus = $sessionStatus;
        return $this;
    }

    public function getSaveForm(): bool
    {
        return $this->saveForm;
    }

    public function setSaveForm(bool $saveForm): self
    {
        $this->saveForm = $saveForm;
        return $this;
    }
}
