<?php


namespace Sintattica\Atk\Core\Menu;


use Exception;
use Sintattica\Atk\Core\Language;
use Sintattica\Atk\Core\Tools;

class ActionItem extends Item
{

    private $nodeUri;
    private $action;

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
     * @return ActionItem
     */
    public function setNodeUri(string $nodeUri): ActionItem
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
     * @return ActionItem
     */
    public function setAction(string $action): ActionItem
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
     * @return ActionItem
     */
    public function setActionUrlParams(array $urlParams): ActionItem
    {
        $this->urlParams = $urlParams;
        return $this;
    }

    /**
     * @return int|array|string
     */
    public function getEnable()
    {

        if ($this->nodeUri && $this->action) {
            return [$this->nodeUri, $this->action];
        }

        return $this->enable;
    }

    /**
     * @return string
     */
    public function getUrl(): string
    {
        $this->addUrlParam('atkmenu', $this->getIdentifier());
        return Tools::dispatch_url($this->nodeUri, $this->action, $this->urlParams);
    }

    public function addUrlParam(string $key, string $value)
    {
        $this->urlParams[$key] = $value;
    }

    /**
     * The method encodes the url params as a string with separators.
     * This is used to generate unique links for the menu items so the active menu can be displayed.
     * If no associative arrays have been provided the index of the array gets concatenated.
     * @return string|null
     * @throws Exception - If UrlParams contain arrays with more than 2 nested levels.
     *
     */
    protected function createIdentifierComponents(): ?string
    {

        $encodedUrlParams = "";
        $separator = "-";

        foreach ($this->urlParams as $key => $value) {
            if (is_array($value)) {
                foreach ($value as $subKey => $val) {
                    if (is_array($val)) {
                        throw new Exception("UrlParams on menu items must have less then 2 levels. More levels have been provided, ...");
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
