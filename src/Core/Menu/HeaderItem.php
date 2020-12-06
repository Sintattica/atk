<?php


namespace Sintattica\Atk\Core\Menu;


class HeaderItem extends Item
{

    /**
     * HeaderItem constructor.
     * @param string $name
     */
    public function __construct(string $name)
    {
        parent::__construct();
        $this->name = $name;
    }



}
