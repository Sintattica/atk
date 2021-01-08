<?php


namespace Sintattica\Atk\Core\Menu;

/**
 * Class SeparatorItem
 * @package Sintattica\Atk\Core\Menu
 */
class SeparatorItem extends Item
{

    private string $color;

    public function __construct(string $color = "#c2c7d0", string $position = MenuBase::MENU_SIDEBAR)
    {
        parent::__construct();
        $this->position = $position;
        $this->color = $color;
    }

    /**
     * @return string
     */
    public function getColor(): string
    {
        return $this->color;
    }

    /**
     * @param string $color
     * @return SeparatorItem
     */
    public function setColor(string $color): SeparatorItem
    {
        $this->color = $color;
        return $this;
    }
    

}
