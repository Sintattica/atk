<?php


namespace Sintattica\Atk\Core\Menu;


class MenuItem
{
    public const TYPE_HEADER = 'header';
    public const TYPE_LINK = 'link';
    public const TYPE_SEPARATOR = 'separator';

    private $uuid;
    private $parent;

    /*
     * Todo: Da fare eventualmente. Possibilità di chiamare il costruttore dentro Menu.php e fare menu->appendItem();
     */

}
