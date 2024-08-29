<?php

namespace Sintattica\Atk\Core\Menu;

use Exception;
use Sintattica\Atk\AdminLte\UIStateColors;
use Sintattica\Atk\Core\Language;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;

class ActionItem extends Item
{
    private $nodeUri;
    private $action;
    protected $urlParams = [];
    protected $badgeText = null;
    protected $badgeStatus = UIStateColors::STATE_INFO;

    public function __construct(string $name, string $nodeUri = '', string $action = '')
    {
        parent::__construct();

        // Default name is the translation of the node name
        if (!$name) {
            list($module, $nodeName) = explode('.', $nodeUri);
            $name = Language::text($nodeName, $module);
        }

        $this->name = $name;
        $this->nodeUri = $nodeUri;
        $this->action = $action;

        // in ActionItem we will check node permissions by default
        $this->enable = false;
    }

    public function getNodeUri(): string
    {
        return $this->nodeUri;
    }

    public function setNodeUri(string $nodeUri): self
    {
        $this->nodeUri = $nodeUri;
        return $this;
    }

    public function getAction(): ?string
    {
        return $this->action;
    }

    public function setAction(string $action): self
    {
        $this->action = $action;
        return $this;
    }

    public function getUrlParams(): array
    {
        return $this->urlParams;
    }

    /**
     * @deprecated
     */
    public function getActionUrlParams(): array
    {
        return $this->getUrlParams();
    }

    public function setUrlParams(array $urlParams): self
    {
        $this->urlParams = $urlParams;
        return $this;
    }

    /**
     * @deprecated
     */
    public function setActionUrlParams(array $urlParams): self
    {
        return $this->setUrlParams($urlParams);
    }

    public function addUrlParam(string $key, string $value): self
    {
        $this->urlParams[$key] = $value;
        return $this;
    }

    /**
     * @return bool|array
     */
    public function getEnable()
    {
        // parent actionitem or enable == true
        if ((!$this->nodeUri && !$this->action) || $this->enable === true) {
            return true;
        }

        if ($this->nodeUri && $this->action) {
            return [$this->nodeUri, $this->action];
        }

        return $this->enable;
    }

    public function getUrl(): string
    {
        $this->addUrlParam(Node::PARAM_ATKMENU, $this->getIdentifier());
        return Tools::dispatch_url($this->nodeUri, $this->action, $this->urlParams);
    }

    public function getBadgeText()
    {
        return $this->badgeText;
    }

    public function setBadgeText($badge): self
    {
        $this->badgeText = $badge;
        return $this;
    }

    public function getBadgeStatus(): string
    {
        return $this->badgeStatus;
    }

    public function setBadgeStatus(string $badgeStatus): self
    {
        $this->badgeStatus = $badgeStatus;
        return $this;
    }

    /**
     * The method encodes the url params as a string with separators.
     * This is used to generate unique links for the menu items so the active menu can be displayed.
     * If no associative arrays have been provided the index of the array gets concatenated.
     *
     * @throws Exception - If UrlParams contain arrays with more than 2 nested levels.
     */
    protected function createIdentifierComponents(): ?string
    {
        $encodedUrlParams = '';
        $separator = '-';

        foreach ($this->urlParams as $key => $value) {
            if (is_array($value)) {
                foreach ($value as $subKey => $val) {
                    if (is_array($val)) {
                        throw new Exception('UrlParams on menu items must have less then 2 levels. More levels have been provided, ...');
                    } else {
                        $encodedUrlParams .= $separator . $subKey . $separator . $val;
                    }
                }
            } else {
                $encodedUrlParams .= $separator . $key . $separator . $value;
            }
        }

        return $this->nodeUri . $this->action . $encodedUrlParams;
    }
}
