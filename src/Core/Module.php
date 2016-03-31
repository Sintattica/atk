<?php

namespace Sintattica\Atk\Core;

/**
 * The Module abstract base class.
 *
 * All modules in an ATK application should derive from this class
 */
abstract class Module
{
    public static $module;

    /** @var Atk $atk */
    private $atk;

    /** @var Menu $menu */
    private $menu;

    public function __construct(Atk $atk, Menu $menu)
    {
        $this->atk = $atk;
        $this->menu = $menu;
    }

    protected function getMenu(){
        return $this->menu;
    }

    protected function getAtk(){
        return $this->atk;
    }

    abstract public function boot();

    public function registerNode($nodeName, $nodeClass, $actions = null)
    {
        $this->atk->registerNode(static::$module.'.'.$nodeName, $nodeClass, $actions);
    }

    public function addNodeToMenu($menuName, $nodeName, $action, $parent = 'main')
    {
        $this->menu->addMenuItem($menuName, Tools::dispatch_url(static::$module.'.'.$nodeName, $action), $parent, [static::$module.'.'.$nodeName, $action], 0,
            static::$module);
    }

    public function addMenuItem($name = '', $url = '', $parent = 'main', $enable = 1)
    {
        $this->menu->addMenuItem($name, $url, $parent, $enable, 0, static::$module);
    }
}
