<?php

namespace Sintattica\Atk\Core;

use ReflectionException;
use Sintattica\Atk\Core\Menu\MenuBase;

/**
 * The Module abstract base class.
 *
 * All modules in an ATK application should derive from this class
 */
abstract class Module
{
    public static string $module;

    private Atk $atk;

    private MenuBase $menu;

    public function __construct(Atk $atk, MenuBase $menu)
    {
        $this->atk = $atk;
        $this->menu = $menu;
    }

    protected function getMenu(): MenuBase
    {
        return $this->menu;
    }

    protected function getAtk(): Atk
    {
        return $this->atk;
    }

    abstract public function register();

    public function boot()
    {
        //noop
    }

    public function registerNode($nodeName, $nodeClass, $actions = null): void
    {
        $this->atk->registerNode(static::$module.'.'.$nodeName, $nodeClass, $actions);
    }

    public function unregisterNode(string $nodeName): void
    {
        $this->atk->unregisterNode(static::$module.'.'.$nodeName);
    }

    /**
     * @throws ReflectionException
     */
    public function addNodeToMenu($menuName, $nodeName, $action, $parent = 'main', $enable = null, $order = 0, $position = MenuBase::MENU_SIDEBAR): void
    {
        if ($enable === null) {
            $enable = [static::$module.'.'.$nodeName, $action];
        }
        $this->menu->addMenuItem($menuName, Tools::dispatch_url(static::$module.'.'.$nodeName, $action), $parent, $enable, $order, static::$module, '', $position);
    }

    /**
     * @throws ReflectionException
     */
    public function addMenuItem($name = '', $url = '', $parent = 'main', $enable = 1): void
    {
        $this->menu->addMenuItem($name, $url, $parent, $enable, 0, static::$module);
    }
}
