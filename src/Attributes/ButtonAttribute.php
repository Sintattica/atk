<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

class ButtonAttribute extends DummyAttribute
{
    const TYPE_SUBMIT = 'submit'; // default

    protected $classes = ['btn', 'btn-default'];
    protected $node = '';
    protected $text = '';
    protected $sessionStatus = SessionManager::SESSION_NESTED;
    protected $saveForm = false;
    protected $onClickCallback = null;


    public function __construct($name, $flags = 0)
    {
        $this->text = $name;
        parent::__construct($name, $flags | self::AF_HIDE_LIST | self::AF_READONLY);
    }

    public function display($record, $mode)
    {
        $classes = implode(' ', $this->classes);
        $tranlatedText = $this->text($this->text);

        return '<button type="submit" class="' . $classes . '" name="' . $this->m_name . '" value="' . $tranlatedText . '">' . $tranlatedText . '</button>';

    }

    public function getText(): string
    {
        return $this->text;
    }

    public function setText($name): self
    {
        $this->text = $name;
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
        return self::TYPE_SUBMIT;
    }

    public function getOnClickCallback(): callable
    {
        return $this->onClickCallback;
    }

    public function onClick(callable $clickCallback): self
    {
        $this->onClickCallback = $clickCallback;
        return $this;
    }
}
