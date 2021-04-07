<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

class ButtonAttribute extends DummyAttribute
{
    const TYPE_SUBMIT = 'submit'; // default
    const TYPE_ACTION = 'action';

    private $name;
    private $classes = ['btn', 'btn-default'];
    private $node = '';
    private $action = '';
    private $params = [];
    private $target = null;
    private $sessionStatus = SessionManager::SESSION_NESTED;
    private $saveForm = false;
    private $type = self::TYPE_SUBMIT;
    private $callback = null;

    public function __construct($name, $flags = 0)
    {
        $this->name = $name;

        parent::__construct($name, $flags | self::AF_HIDE_LIST | self::AF_READONLY);
    }

    public function display($record, $mode)
    {
        $classes = implode(' ', $this->classes);

        switch ($this->type) {
            case self::TYPE_ACTION:
                /** @var Node $ownerInstance */
                $ownerInstance = Atk::getInstance()->atkGetNode($this->node);
                $action = $this->action;
                if ($action === 'edit' and !$ownerInstance->allowed('edit')) {
                    // action edit ma l'utente non ha i permessi: mando in view
                    $action = 'view';
                }
                $this->params['atkselector'] = $ownerInstance->getPrimaryKey($record);
                $url = Tools::dispatch_url($this->node, $action, $this->params);
                $extraProps = [];
                if ($classes) {
                    $extraProps[] = 'class="' . $classes . '"';
                }
                if ($this->target) {
                    $extraProps[] = 'target="' . $this->target . '"';
                }
                return Tools::href($url, Tools::atktext($this->name, $ownerInstance->getModule()), $this->sessionStatus, $this->saveForm, implode(' ', $extraProps));

            case self::TYPE_SUBMIT:
            default:
                $txt = $this->text($this->name);
                return '<button type="submit" class="' . $classes . '" name="' . $this->name . '" value="' . $txt . '">' . $txt . '</button>';
        }
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

    public function getName()
    {
        return $this->name;
    }

    public function setName($name): self
    {
        $this->name = $name;
        return $this;
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

    public function getClasses(): array
    {
        return $this->classes;
    }

    public function setClasses(array $cssClasses): self
    {
        $this->classes = $cssClasses;
        return $this;
    }

    public function addClass(string $class): self
    {
        $this->classes[] = $class;
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

    public function getType(): string
    {
        return $this->type;
    }

    public function setType(string $type): self
    {
        $this->type = $type;
        return $this;
    }

    public function getCallback(): callable
    {
        return $this->callback;
    }

    public function setCallback(callable $callback): self
    {
        $this->callback = $callback;
        return $this;
    }
}