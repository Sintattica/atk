<?php namespace Sintattica\Atk\Attributes;


use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Ui\Page;

class bootstrapColorPickerAttribute extends Attribute
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

    function __construct($name, $colorPickerOptions = array(), $flags = 0, $size = 0)
    {
        $this->colorPickerOptions = $colorPickerOptions;
        parent::__construct($name, $flags, $size);
    }


    public function registerScriptsAndStyles($fieldprefix)
    {
        $htmlId = $this->getHtmlId($fieldprefix) . '_group';

        $page = Page::getInstance();
        $base = Config::getGlobal('atkroot') . 'atk/themes/bootstrap/lib/bootstrap-colorpicker/dist/';

        $page->register_script($base . 'js/bootstrap-colorpicker.min.js');
        $page->register_style($base . 'css/bootstrap-colorpicker.min.css');

        $opts = json_encode($this->colorPickerOptions);
        $page->register_scriptcode("
            jQuery(function($){
                $('#$htmlId').colorpicker($opts);
            });");
    }

    function edit($record = "", $fieldprefix = "", $mode = "")
    {
        $this->registerScriptsAndStyles($fieldprefix);

        $id = $this->getHtmlId($fieldprefix);
        $this->registerKeyListener($id, KB_CTRLCURSOR | KB_UPDOWN);

        if (count($this->m_onchangecode)) {
            $onchange = 'onChange="' . $id . '_onChange(this);"';
            $this->_renderChangeHandler($fieldprefix);
        } else {
            $onchange = '';
        }

        $this->registerJavaScriptObservers($id);

        $size = $this->m_size;
        if ($mode == 'list' && $size > 20) {
            $size = 20;
        }

        $value = (isset($record[$this->fieldName()]) && !is_array($record[$this->fieldName()])
            ? htmlspecialchars($record[$this->fieldName()]) : "");

        $result = '<div class="input-group" id="' . $id . '_group">';
        $result .= '<input type="text" id="' . $id . '" name="' . $id . '" ' . $this->getCSSClassAttribute(array('form-control')) .
            ' value="' . $value . '"' .
            ($size > 0 ? ' size="' . $size . '"' : '') .
            ($this->m_maxsize > 0 ? ' maxlength="' . $this->m_maxsize . '"' : '') . ' ' . $onchange . ' />';

        $result .= '<span class="input-group-addon"><i></i></span>';
        $result .= '</div>';

        return $result;
    }
}