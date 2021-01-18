<?php


namespace Sintattica\Atk\Core\Menu;


use Sintattica\Atk\Core\Tools;

/**
 * Class Item
 * Fluent Interface to create a new Item.
 * @package Sintattica\Atk\Core\Menu
 * @author N.Gjata
 */
abstract class Item
{


    protected const DEFAULT_PARENT = "main";
    protected const DEFAULT_ORDER = -1;

    protected $uuid;
    protected $name = "";

    protected $parent = self::DEFAULT_PARENT;
    protected $position = MenuBase::MENU_SIDEBAR;
    protected $enable = 1;
    protected $order = self::DEFAULT_ORDER;
    protected $module = '';
    protected $raw = false;
    protected $urlParams = [];

    //Todo: Pensare come fare per i link esterni!
    // private string $url;
    protected $icon = null;
    protected $active = false;


    //Todo: Creare gli ItemType -> HeaderMenuItem, SeparatorMenuItem, LinkMenuItem che estendono Item
    // Campi base: name, parent, position, enabled, order, module, itemType (autocompilato)
    protected function __construct()
    {
        $this->uuid = uniqid("", true);
    }

    /**
     * Called only by children as this class is abstract
     * @return string
     */
    public function getType(): string
    {
        try {
            return Tools::getClassName($this);
        } catch (\Exception $e){
            //NOOP -- this cannot happen!
        }

        return "";
    }


    public function getIdentifier(): string
    {
        return $this->uuid;
    }

    /**
     * @return string
     */
    public function getUuid(): string
    {
        return $this->uuid;
    }

    /**
     * @return string
     */
    public function getParent(): string
    {
        return $this->parent;
    }

    /**
     * @param string $parent
     * @return Item
     */
    public function setParent(string $parent): Item
    {
        $this->parent = $parent;
        return $this;
    }

    /**
     * @return string
     */
    public function getName(): string
    {
        return $this->name;
    }

    /**
     * @param string $name
     * @return Item
     */
    public function setName(string $name): Item
    {
        $this->name = $name;
        return $this;
    }

    /**
     * @return string
     */
    public function getPosition(): string
    {
        return $this->position;
    }

    /**
     * @param string $position
     * @return Item
     */
    public function setPosition(string $position): Item
    {
        $this->position = $position;
        return $this;
    }

    /**
     * @return int
     */
    public function getEnable(): int
    {
        return $this->enable;
    }

    /**
     * @param int $enable
     * @return Item
     */
    public function setEnable(int $enable): Item
    {
        $this->enable = $enable;
        return $this;
    }

    /**
     * @return int
     */
    public function getOrder(): int
    {
        return $this->order;
    }

    /**
     * @param int $order
     * @return Item
     */
    public function setOrder(int $order): Item
    {
        $this->order = $order;
        return $this;
    }

    /**
     * @return string
     */
    public function getModule(): string
    {
        return $this->module;
    }

    /**
     * @param string $module
     * @return Item
     */
    public function setModule(string $module): Item
    {
        $this->module = $module;
        return $this;
    }

    /**
     * @return bool
     */
    public function isRaw(): bool
    {
        return $this->raw;
    }

    /**
     * @param bool $raw
     * @return Item
     */
    public function setRaw(bool $raw): Item
    {
        $this->raw = $raw;
        return $this;
    }

    /**
     * @return null|string
     */
    public function getIcon(): ?string
    {
        return $this->icon;
    }

    /**
     * @param string|null $icon
     * @return Item
     */
    public function setIcon(?string $icon): Item
    {
        $this->icon = $icon;
        return $this;
    }

    /**
     * @return bool
     */
    public function isActive(): bool
    {
        return $this->active;
    }

    /**
     * @param bool $active
     * @return Item
     */
    public function setActive(bool $active): Item
    {
        $this->active = $active;
        return $this;
    }



}
