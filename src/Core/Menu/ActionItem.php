<?php


namespace Sintattica\Atk\Core\Menu;


use Sintattica\Atk\Core\Language;
use Sintattica\Atk\Core\Tools;

class ActionItem extends Item
{

    private string $nodeUri;
    private string $action;

    /**
     * ActionItem constructor.
     * @param string $name
     * @param string $nodeUri
     * @param string $action
     */
    public function __construct(string $name, string $nodeUri = "", string $action = "")
    {

        parent::__construct();

        //Default name is the translation of the node name
        if (!$name) {
            list($modulo, $nodo) = explode('.', $nodeUri);
            $name = Language::text($nodo, $modulo);
        }

        $this->name = $name;
        $this->nodeUri = $nodeUri;
        $this->action = $action;

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
     * @return Item
     */
    public function setNodeUri(string $nodeUri): Item
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
     * @return Item
     */
    public function setAction(string $action): Item
    {
        $this->action = $action;
        return $this;
    }

    /**
     * @return array
     */
    public function getActionUrlParams(): array
    {
        return $this->urlParams;
    }

    /**
     * @param array $urlParams
     * @return Item
     */
    public function setActionUrlParams(array $urlParams): Item
    {
        $this->urlParams = $urlParams;
        return $this;
    }

    /**
     * @return string
     */
    public function getUrl(): string
    {
        return Tools::dispatch_url($this->nodeUri, $this->action, $this->urlParams);
    }


}
