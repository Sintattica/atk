<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\AdminLte\UIStateColors;

class ButtonAttribute extends DummyAttribute
{
    const TYPE_SUBMIT = 'submit'; // default
    const TYPE_ACTION = 'action';

    private $currentState = UIStateColors::STATE_DEFAULT;

    public function __construct($name, $flags = 0)
    {
        $this->addCSSClasses(['btn', 'btn-sm', 'btn-default']);
        parent::__construct($name, $flags | self::AF_HIDE_LIST | self::AF_HIDE_VIEW | self::AF_HIDE_ADD | self::AF_READONLY, $name);
    }

    /**
     * @param string $uiState
     * @return ButtonAttribute
     */
    public function setUIState(string $uiState): self
    {
        if (in_array($uiState, UIStateColors::getAllUIStates())) {
            $this->removeUIState($this->currentState);
            $this->currentState = $uiState;
            $this->addCSSClass("btn-$uiState");
        }

        return $this;
    }

    /**
     * @param string $uiState
     * @return ButtonAttribute
     */
    public function removeUIState(string $uiState): self
    {
        if (in_array($uiState, UIStateColors::getAllUIStates())) {
            $this->currentState = UIStateColors::STATE_DEFAULT;
            $this->removeCSSClass("btn-$uiState");
        }

        return $this;
    }
}
