<?php


namespace Sintattica\Atk\Core\Menu;


use Sintattica\Atk\Core\Language;
use Sintattica\Atk\Core\Tools;

/**
 * Class MenuItem
 * Fluent Interface to create a new MenuItem.
 * @package Sintattica\Atk\Core\Menu
 * @author N.Gjata
 */
class MenuItem
{
    public const TYPE_HEADER = 'header';
    public const TYPE_LINK = 'link';
    public const TYPE_SEPARATOR = 'separator';

    private const DEFAULT_ORDER = -1;

    private string $uuid;
    private string $name;
    private string $parent;
    private string $nodeUri;
    private string $action;
    private string $position;
    private int $enable;
    private int $order;
    private string $module;
    private string $target;
    private bool $raw;
    private string $type;
    private array $urlParams;
    private string $url;
    private ?string $icon;
    private bool $active = false;


    //Todo: Creare gli ItemType -> HeaderMenuItem, SeparatorMenuItem, LinkMenuItem che estendono MenuItem
    // Campi base: name, parent, position, enabled, order, module, itemType (autocompilato)
    public function __construct(
        string $name = '',
        string $parent = 'main',
        string $nodeUri = '',
        string $action = '',
        string $position = MenuBase::MENU_SIDEBAR,
        int $enable = 1,
        int $order = self::DEFAULT_ORDER,
        string $module = '',
        string $target = '',
        bool $raw = false,
        string $itemType = MenuItem::TYPE_LINK,
        array $urlParams = [],
        string $icon = null
    )
    {

        $this->uuid = uniqid("", true);


        //Default name is the translation of the node name
        if (!$name) {
            list($modulo, $nodo) = explode('.', $nodeUri);
            $this->name = Language::text($nodo, $modulo);
        }

        $this->name = $name;
        $this->parent = $parent;
        $this->nodeUri = $nodeUri;
        $this->action = $action;
        $this->position = $position;
        $this->enable = $enable;
        $this->order = $order;
        $this->module = $module;
        $this->target = $target;
        $this->raw = $raw;
        $this->type = $itemType;
        $this->urlParams = $urlParams;
        $this->url = Tools::dispatch_url($nodeUri, $action, $urlParams);
        $this->icon = $icon;

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
     * @return MenuItem
     */
    public function setParent(string $parent): MenuItem
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
     * @return MenuItem
     */
    public function setName(string $name): MenuItem
    {
        $this->name = $name;
        return $this;
    }

    /**
     * @return string
     */
    public function getNodeUri(): string
    {
        return $this->nodeUri;
    }

    /**
     * @param string $nodeUri
     * @return MenuItem
     */
    public function setNodeUri(string $nodeUri): MenuItem
    {
        $this->nodeUri = $nodeUri;
        return $this;
    }

    /**
     * @return string
     */
    public function getAction(): string
    {
        return $this->action;
    }

    /**
     * @param string $action
     * @return MenuItem
     */
    public function setAction(string $action): MenuItem
    {
        $this->action = $action;
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
     * @return MenuItem
     */
    public function setPosition(string $position): MenuItem
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
     * @return MenuItem
     */
    public function setEnable(int $enable): MenuItem
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
     * @return MenuItem
     */
    public function setOrder(int $order): MenuItem
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
     * @return MenuItem
     */
    public function setModule(string $module): MenuItem
    {
        $this->module = $module;
        return $this;
    }

    /**
     * @return string
     */
    public function getTarget(): string
    {
        return $this->target;
    }

    /**
     * @param string $target
     * @return MenuItem
     */
    public function setTarget(string $target): MenuItem
    {
        $this->target = $target;
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
     * @return MenuItem
     */
    public function setRaw(bool $raw): MenuItem
    {
        $this->raw = $raw;
        return $this;
    }

    /**
     * @return string
     */
    public function getType(): string
    {
        return $this->type;
    }

    /**
     * @param string $type
     * @return MenuItem
     */
    public function setType(string $type): MenuItem
    {
        $this->type = $type;
        return $this;
    }

    /**
     * @return array
     */
    public function getUrlParams(): array
    {
        return $this->urlParams;
    }

    /**
     * @param array $urlParams
     * @return MenuItem
     */
    public function setUrlParams(array $urlParams): MenuItem
    {
        $this->urlParams = $urlParams;
        return $this;
    }

    /**
     * @return string
     */
    public function getUrl(): string
    {
        return $this->url;
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
     * @return MenuItem
     */
    public function setIcon(?string $icon): MenuItem
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
     * @return MenuItem
     */
    public function setActive(bool $active): MenuItem
    {
        $this->active = $active;
        return $this;
    }

}
