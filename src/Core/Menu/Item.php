<?php

namespace Sintattica\Atk\Core\Menu;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Language;
use Sintattica\Atk\Core\Tools;

/**
 * Class Item
 * Fluent Interface to create a new Item.
 * @package Sintattica\Atk\Core\Menu
 * @author N.Gjata
 */
abstract class Item
{

    public const TOOLTIP_PLACEMENT_TOP = "top";
    public const TOOLTIP_PLACEMENT_BOTTOM = "bottom";
    public const TOOLTIP_PLACEMENT_LEFT = "left";
    public const TOOLTIP_PLACEMENT_RIGHT = "right";

    public const ICON_FA = 'fa';
    public const ICON_IMAGE = 'image';
    public const ICON_CHARS = 'chars';

    protected const DEFAULT_PARENT = "main";
    protected const DEFAULT_ORDER = -1;

    protected $uuid;
    protected $name = "";
    protected $parent = self::DEFAULT_PARENT;
    protected $position;
    protected $enable = true;
    protected $order = self::DEFAULT_ORDER;
    protected $module = '';
    protected $raw = false;
    protected $tooltip = null;
    protected $tooltipPlacement = self::TOOLTIP_PLACEMENT_BOTTOM;
    protected $icon = null;
    protected $active = false;

    private $hideName = false;
    private $hideIcon = false;
    private $iconType = self::ICON_FA;

    public function __construct()
    {
        $this->position = Config::getGlobal('menu_default_item_position');
    }

    protected abstract function createIdentifierComponents(): ?string;

    public function getIdentifier(): ?string
    {
        if (!$this->uuid) {
            $this->uuid = self::generateHash($this->position . $this->parent . $this->createIdentifierComponents());
        }

        return $this->uuid;
    }

    protected static function generateHash($string, $hashLength = 6)
    {
        $fullHash = md5($string);
        return $hashLength ? substr($fullHash, 0, $hashLength) : $fullHash;
    }

    /**
     * Called only by children as this class is abstract
     */
    public function getType(): string
    {
        try {
            return Tools::getClassName($this);
        } catch (\Exception $e) {
            //NOOP -- this cannot happen!
        }

        return "";
    }

    public function getParent(): string
    {
        return $this->parent;
    }

    public function setParent(string $parent): self
    {
        $this->parent = $parent;
        return $this;
    }

    public function getName(): string
    {
        return $this->name;
    }

    public function setName(string $name): self
    {
        $this->name = $name;
        return $this;
    }

    public function getPosition(): string
    {
        return $this->position;
    }

    public function setPosition(string $position): self
    {
        $this->position = $position;
        return $this;
    }

    /**
     * @return bool|array
     */
    public function getEnable()
    {
        return $this->enable;
    }

    public function setEnable(bool $enable): self
    {
        $this->enable = $enable;
        return $this;
    }

    public function getOrder(): int
    {
        return $this->order;
    }

    public function setOrder(int $order): self
    {
        $this->order = $order;
        return $this;
    }

    public function getModule(): string
    {
        return $this->module;
    }

    public function setModule(string $module): self
    {
        $this->module = $module;
        return $this;
    }

    public function isRaw(): bool
    {
        return $this->raw;
    }

    public function setRaw(bool $raw): self
    {
        $this->raw = $raw;
        return $this;
    }

    public function getIcon(): ?string
    {
        return $this->icon;
    }

    public function setIcon(?string $icon, ?string $iconType = self::ICON_FA): self
    {
        switch ($iconType) {
            case self::ICON_IMAGE:
                $this->iconType = self::ICON_IMAGE;
                $this->icon = $icon;
                break;
            case self::ICON_CHARS:
                $this->iconType = self::ICON_CHARS;
                $this->icon = substr(Tools::atktext($icon, '', '', Language::getLanguage()), 0, 2);
                break;
            default:
                $this->iconType = self::ICON_FA;
                $this->icon = $icon;
        }

        return $this;
    }

    public function isActive(): bool
    {
        return $this->active;
    }

    public function setActive(bool $active): self
    {
        $this->active = $active;
        return $this;
    }

    public function getIconType(): string
    {
        return $this->iconType;
    }

    public function isNameHidden(): bool
    {
        return $this->hideName;
    }

    public function hideName(bool $hideItemName = true): self
    {
        $this->hideName = $hideItemName;
        return $this;
    }

    public function isIconHidden(): bool
    {
        return $this->hideIcon;
    }

    public function hideIcon(bool $hideItemIcon = true): self
    {
        $this->hideIcon = $hideItemIcon;
        return $this;
    }

    public function getTooltip(): ?string
    {
        return $this->tooltip;
    }

    public function setTooltip(string $tooltip): self
    {
        $this->tooltip = $tooltip;
        return $this;
    }

    public function getTooltipPlacement(): string
    {
        return $this->tooltipPlacement;
    }

    public function setTooltipPlacement(string $tooltipPlacement): self
    {
        $this->tooltipPlacement = $tooltipPlacement;
        return $this;
    }
}
