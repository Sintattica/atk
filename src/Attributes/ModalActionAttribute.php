<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\AdminLte\UIStateColors;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Session\SessionManager;

class ModalActionAttribute extends DummyAttribute
{

    private $uiState;
    private $okLabel = "Save Changes";
    private $cancelLabel = "Close";
    private $modalTitle = "";
    private $body = "";
    private $params = [];
    private $hiddenParams = [];
    private $atkActionName = "";

    /**
     * Constructor.
     *
     * @param string $name The name of the attribute
     * @param int $flags The flags for this attribute
     * @param string $text The text to display
     */
    public function __construct($name, $flags = 0, $text = '', ?string $uiState = UIStateColors::COLOR_DEFAULT)
    {

        $this->uiState = $uiState;

        parent::__construct($name, $flags); // base class constructor
        $this->m_text = $text;
    }

    /**
     * @return string|null
     */
    public function getUiState(): ?string
    {
        return $this->uiState;
    }

    /**
     * @param string|null $uiState
     * @return ModalActionAttribute
     */
    public function setUiState(?string $uiState): ModalActionAttribute
    {
        $this->uiState = $uiState;
        return $this;
    }

    /**
     * @return string
     */
    public function getOkLabel(): string
    {
        return $this->okLabel;
    }

    /**
     * @param string $okLabel
     * @return ModalActionAttribute
     */
    public function setOkLabel(string $okLabel): ModalActionAttribute
    {
        $this->okLabel = $okLabel;
        return $this;
    }

    /**
     * @return string
     */
    public function getCancelLabel(): string
    {
        return $this->cancelLabel;
    }

    /**
     * @param string $cancelLabel
     * @return ModalActionAttribute
     */
    public function setCancelLabel(string $cancelLabel): ModalActionAttribute
    {
        $this->cancelLabel = $cancelLabel;
        return $this;
    }

    /**
     * @return string
     */
    public function getModalTitle(): string
    {
        return $this->modalTitle;
    }

    /**
     * @param string $modalTitle
     * @return ModalActionAttribute
     */
    public function setModalTitle(string $modalTitle): ModalActionAttribute
    {
        $this->modalTitle = $modalTitle;
        return $this;
    }

    /**
     * @return string
     */
    public function getBody(): string
    {
        return $this->body;
    }

    /**
     * @param string $body
     * @return ModalActionAttribute
     */
    public function setBody(string $body): ModalActionAttribute
    {
        $this->body = $body;
        return $this;
    }

    /**
     * @return array
     */
    public function getParams(): array
    {
        return $this->params;
    }

    /**
     * @param array $params - nomi dei parametri da passare
     * @return ModalActionAttribute
     */
    public function setParamKeys(array $params): ModalActionAttribute
    {
        foreach ($params as $param) {
            $this->params[$param] = "";
        }

        return $this;
    }


    public function addParam(string $key, ?string $value = ""): ModalActionAttribute
    {
        $this->params[$key] = $value;
        return $this;
    }


    public function addHiddenParam(string $key, ?string $value = ""): ModalActionAttribute
    {
        $this->hiddenParams[$key] = $value;
        return $this;
    }

    /**
     * @return string
     */
    public function getAtkActionName(): string
    {
        return $this->atkActionName;
    }

    /**
     * @param string $atkActionName
     * @return ModalActionAttribute
     */
    public function setAtkActionName(string $atkActionName): ModalActionAttribute
    {
        $this->atkActionName = $atkActionName;
        return $this;
    }



    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * Here it will only return the text, no edit box.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string A piece of htmlcode for editing this attribute
     */
    public function edit($record, $fieldprefix, $mode): string
    {
        $strClasses = implode(' ', $this->getCSSClasses());
        $result = '<a class="btn bg-' . $this->uiState . ' ' . $strClasses . ' " id="' . $this->getHtmlId($fieldprefix) . '" data-toggle="modal" data-target="#' . $this->getHtmlId($fieldprefix) . '-modal"';

        $style = '';
        foreach ($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }

        if ($style != '') {
            $result .= ' style="' . $style . '"';
        }

        $result .= '>';

        if (in_array($mode, ['csv', 'plain', 'list'])) {
            return $this->m_text;
        }

        $result .= '<span>' . $this->m_text . '</span>';

        $result .= '</a>';

        $result .= $this->createModalWindow($record, $fieldprefix);

        return $result;
    }

    private function createModalWindow(array $record, string $fieldPrefix): string
    {

        $nodeInstance = $this->getOwnerInstance();

        $htmlFieldPrefix = $this->getHtmlId($fieldPrefix);
        $htmlInputIds = [];

        $result = '<div class="modal fade" id="' . $htmlFieldPrefix . '-modal' . '" tabindex="-1" role="dialog" aria-labelledby="exampleModalCenterTitle" aria-hidden="true">
                   <div class="modal-dialog modal-dialog-centered" role="document">
                     <div class="modal-content">
                       <div class="modal-header">
                         <h5 class="modal-title">' . $this->modalTitle . '</h5>
                         <button type="button" class="close" data-dismiss="modal" aria-label="Close"><span aria-hidden="true">&times;</span></button>
                       </div> 
                       <div class="modal-body">
                        <form method="post" action="' . Config::getGlobal('dispatcher') . '">';


        $sm = SessionManager::getInstance();

        $result .= '<input type="hidden" name="atkaction" value="' . $this->atkActionName . '">';
        $result .= '<input type="hidden" name="atkmenu" value="' . $sm->globalStackVar('atkmenu') . '">';
        $result .= '<input type="hidden" name="atknodeuri" value="' . $nodeInstance->atkNodeUri() . '">';
        $result .= '<input type="hidden" name="atkselector" value="' . $nodeInstance->getPrimaryKey($record) . '">';

        foreach ($this->hiddenParams as $key => $val) {
            $result .= '<input type="hidden" name="' . $key . '" value="' . $val . '">';
        }

        foreach ($this->params as $key => $val) {
            $inputId = $htmlFieldPrefix . '_input_' . $key;

            $result .= '<div class="input-group input-group-sm mb-3">
                             <div class="input-group-prepend">
                               <span class="input-group-text">' . $this->text($key) . '</span>
                             </div>
                             <input type="text" class="form-control" aria-label="Small" aria-describedby="inputGroup-sizing-sm" id="' . $inputId . '" name=" ' . $key . '">
                        </div>';
        }


        $result .= '    </form>
                        </div>
                          <div class="modal-footer">
                            <a class="btn btn-secondary" data-dismiss="modal">' . $this->cancelLabel . '</a>
                            <button type="submit" class="btn bg-' . $this->uiState . '"> ' . $this->okLabel . '</button>
                          </div>
                        </div>
                      </div>
                    </div>';

        return $result;
    }

}
