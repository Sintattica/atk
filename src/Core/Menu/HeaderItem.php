<?php


namespace Sintattica\Atk\Core\Menu;


class HeaderItem extends Item
{
    public function __construct(string $name)
    {
        parent::__construct();
        $this->name = $name;
    }

    protected function createIdentifierComponents(): ?string
    {
        return '';
    }
}
