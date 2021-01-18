<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Ui\Page;

class SwitchAttribute extends BoolAttribute
{
    /*
     * $switchOptions: see http://www.bootstrap-switch.org/options.html
     * eg: array('size' => 'large')
     */

    protected $switchOptions;

    private $page;

    public function __construct($name, $flags = 0, $switchOptions = [])
    {
        $this->page = Page::getInstance();

        $defaultOptions = array(
            'offText' => mb_strtoupper($this->text('no'), 'UTF-8'),
            'onText' => mb_strtoupper($this->text('yes'), 'UTF-8'),
            'size' => 'small'
        );

        $this->switchOptions = array_merge($defaultOptions, $switchOptions);

        parent::__construct($name, $flags);
    }

    public function edit($record, $fieldprefix, $mode): string
    {

        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);

        $onchange = '';
        if (Tools::count($this->m_onchangecode)) {
            $onchange = 'onClick="' . $id . '_onChange(this);" ';
            $this->_renderChangeHandler($fieldprefix);
        }

        $checked = isset($record[$this->fieldName()]) && $record[$this->fieldName()] > 0 ? 'checked' : '';

        $opts = json_encode($this->switchOptions);

        $this->page->register_loadscript("
            jQuery(function($){
                $('#$id').bootstrapSwitch($opts);
            });");

        $result = '<input type="checkbox" id="' . $id . '" name="' . $name . '" value="1" ' . $onchange . $checked . ' ' . $this->getCSSClassAttribute() . ' />';

        if ($this->hasFlag(self::AF_BOOL_INLINE_LABEL)) {
            $result .= '&nbsp;<label for="' . $id . '">' . $this->text($this->fieldName() . '_label') . '</label>';
        }

        return $result;
    }
}
