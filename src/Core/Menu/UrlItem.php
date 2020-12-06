<?php


namespace Sintattica\Atk\Core\Menu;


class UrlItem extends Item
{
    public const TARGET_BLANK = "_blank";
    public const TARGET_SELF = "_self";
    public const TARGET_PARENT = "_parent";
    public const TARGET_TOP = "_top";

    private string $url;
    protected string $target;

    /**
     * UrlItem constructor.
     * @param string $name
     * @param string $url
     * @param string $target
     */
    public function __construct(string $name, string $url, $target = self::TARGET_SELF)
    {
        parent::__construct();
        $this->name = $name;
        $this->url = $url;
        $this->target = $target;
    }


    /**
     * @return string
     */
    public function getUrl(): string
    {
        return $this->url;
    }

    /**
     * @param string $url
     * @return UrlItem
     */
    public function setUrl(string $url): UrlItem
    {
        $this->url = $url;
        return $this;
    }

    /**
     * @return string
     */
    public function getTarget(): string
    {
        return $this->target;
    }

    public function setTarget(string $target): Item
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


}
