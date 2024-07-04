<?php


namespace Sintattica\Atk\Core\Menu;


class UrlItem extends Item
{
    public const TARGET_BLANK = "_blank";
    public const TARGET_SELF = "_self";
    public const TARGET_PARENT = "_parent";
    public const TARGET_TOP = "_top";

    private $url;
    protected $target;

    public function __construct(string $name, string $url, string $target = self::TARGET_SELF)
    {
        parent::__construct();
        $this->name = $name;
        $this->url = $url;
        $this->target = $target;
    }

    public function getUrl(): string
    {
        return $this->url;
    }

    public function setUrl(string $url): self
    {
        $this->url = $url;
        return $this;
    }

    public function getTarget(): string
    {
        return $this->target;
    }

    public function setTarget(string $target): self
    {
        switch ($target) {
            case $target == self::TARGET_BLANK:
                $this->target = self::TARGET_BLANK;
                break;
            case $target == self::TARGET_PARENT:
                $this->target = self::TARGET_PARENT;
                break;
            case $target == self::TARGET_SELF:
                $this->target = self::TARGET_SELF;
                break;
            case $target == self::TARGET_TOP:
                $this->target = self::TARGET_TOP;
                break;
            default:
                $this->target = $target;
        }

        return $this;
    }

    protected function createIdentifierComponents(): ?string
    {
        return $this->url;
    }
}
