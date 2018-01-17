<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Core\Tools;

class ColorPickerAttribute extends Attribute
{
    /*
     * remember to import in your node:
     *
     *
     *
     * $colorPickerOptions: see http://mjolnic.com/bootstrap-colorpicker/
     * eg: array('format' => 'hex')
     */

    protected $colorPickerOptions;

    public function __construct($name, $flags = 0, $colorPickerOptions = [])
    {
        $this->colorPickerOptions = $colorPickerOptions;
        parent::__construct($name, $flags);
    }

    public function registerScriptsAndStyles($fieldprefix)
    {
        $htmlId = $this->getHtmlId($fieldprefix).'_group';

        $page = Page::getInstance();
        $base = Config::getGlobal('assets_url').'lib/bootstrap-colorpicker/dist/';

        $page->register_script($base.'js/bootstrap-colorpicker.min.js');
        $page->register_style($base.'css/bootstrap-colorpicker.min.css');

        $opts = json_encode($this->colorPickerOptions);
        $page->register_scriptcode("
            jQuery(function($){
                $('#$htmlId').colorpicker($opts);
            });");
    }

    public function edit($record, $fieldprefix, $mode)
    {
        $this->registerScriptsAndStyles($fieldprefix);

        $id = $this->getHtmlId($fieldprefix);

        if (Tools::count($this->m_onchangecode)) {
            $onchange = 'onChange="'.$id.'_onChange(this);"';
            $this->_renderChangeHandler($fieldprefix);
        } else {
            $onchange = '';
        }

        $style = '';
        foreach($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }

        $value = (isset($record[$this->fieldName()]) && !is_array($record[$this->fieldName()]) ? htmlspecialchars($record[$this->fieldName()]) : '');

        $result = '<div class="input-group ColorPickerAttribute_group" id="'.$id.'_group">';
        $result .= '<input type="text" id="'.$id.'"';
        $result .= ' name="'.$this->getHtmlName($fieldprefix).'"';
        $result .= ' '.$this->getCSSClassAttribute(array('form-control'));
        $result .= ' value="'.$value.'"';
        if($this->m_size > 0){
            $result .= ' size="'.$this->m_size.'"';
        }
        if($this->m_maxsize > 0){
            $result .= ' maxlength="'.$this->m_maxsize.'"';
        }
        if($onchange){
            $result .= ' '.$onchange;
        }
        if($placeholder = $this->getPlaceholder()){
            $result .= ' placeholder="'.htmlspecialchars($placeholder).'"';
        }
        if($style != ''){
            $result .= ' style="'.$style.'"';
        }
        $result .= ' />';


        $result .= '<span class="input-group-addon"><i></i></span>';
        $result .= '</div>';

        return $result;
    }
}
