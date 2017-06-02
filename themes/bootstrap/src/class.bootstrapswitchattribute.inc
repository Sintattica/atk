<?php

useattrib("atkboolattribute");

class bootstrapSwitchAttribute extends atkBoolAttribute
{
    /*
     * remember to import in your node:
     * atkimport('atk.themes.bootstrap.src.bootstrapSwitchAttribute');
     *
     * $switchOptions: see http://www.bootstrap-switch.org/options.html
     * eg: array('size' => 'large')
     */

    protected $switchOptions;

    function bootstrapSwitchAttribute($name, $switchOptions = array(), $flags = 0)
    {
        $defaultOptions = array(
            'offText' => mb_strtoupper($this->text("no"), 'UTF-8'),
            'onText' => mb_strtoupper($this->text("yes"), 'UTF-8'),
        );
        $this->switchOptions = array_merge($defaultOptions, $switchOptions);
        atkBoolAttribute::__construct($name, $flags);
    }

    public function registerScriptsAndStyles($fieldprefix)
    {
        $htmlId = $this->getHtmlId($fieldprefix);

        $page = atkinstance('atk.ui.atkpage');
        //$theme = atkinstance("atk.ui.atktheme");
        $base = atkconfig('atkroot') . 'atk/themes/bootstrap/lib/bootstrap-switch/';

        $page->register_script($base . 'js/bootstrap-switch.min.js');
        $page->register_style($base . 'css/bootstrap3/bootstrap-switch.min.css');

        $opts = json_encode($this->switchOptions);
        $page->register_loadscript("
            jQuery(function($){
                $('#$htmlId').bootstrapSwitch($opts);
            });");
    }

    function edit($record = "", $fieldprefix = "", $mode = "")
    {
        $this->registerScriptsAndStyles($fieldprefix);

        $id = $this->getHtmlId($fieldprefix);
        $onchange = '';
        if (count($this->m_onchangecode)) {
            $onchange = 'onClick="' . $id . '_onChange(this);" ';
            $this->_renderChangeHandler($fieldprefix);
        }
        $checked = "";
        if (isset($record[$this->fieldName()]) && $record[$this->fieldName()] > 0)
            $checked = "checked";
        $this->registerKeyListener($id, KB_CTRLCURSOR | KB_CURSOR);

        $result = '<input type="checkbox" id="' . $id . '" name="' . $id . '" value="1" ' . $onchange . $checked . ' ' . $this->getCSSClassAttribute("atkcheckbox") . ' />';

        if ($this->hasFlag(AF_BOOL_INLINE_LABEL)) {
            $result .= '&nbsp;<label for="' . $id . '">' . $this->text(array(
                    $this->fieldName() . '_label', parent::label($record))) . '</label>';
        }

        $result .= $this->getSpinner();

        return $result;
    }

    // AGX: this is broken, 2nd parameter should be $mode
    function display($record, $fieldprefix = "")
    {
        $this->registerScriptsAndStyles($fieldprefix);

        $id = $this->getHtmlId($fieldprefix);

        if ($this->hasFlag(AF_BOOL_DISPLAY_CHECKBOX)) {
            return '
    		  <div align="left">
    		    <input type="checkbox" id="' . $id . '" disabled="disabled" ' . ($record[$this->fieldName()]
                ? 'checked="checked"' : '') . ' />
    		  </div>
    		';
        } else {
            return $this->text($record[$this->fieldName()] ? "yes" : "no");
        }
    }

}

?>
