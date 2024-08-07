<?php


namespace Sintattica\Atk\Core\Menu;


/**
 * Class SeparatorItem
 * @package Sintattica\Atk\Core\Menu
 */
class SeparatorItem extends Item
{
    private $color;

    public function __construct(string $color = "#c2c7d0", string $position = MenuBase::MENU_SIDEBAR)
    {
        parent::__construct();
        $this->position = $position;
        $this->color = $color;
        $this->name = uniqid();
    }

    public function getColor(): string
    {
        return $this->color;
    }

    public function setColor(string $color): self
    {
        $this->color = $color;
        return $this;
    }

    protected function createIdentifierComponents(): ?string
    {
        return '';
    }
}
