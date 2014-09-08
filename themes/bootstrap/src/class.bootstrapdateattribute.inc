<?php

// https://github.com/Eonasdan/bootstrap-datetimepicker
useattrib("atkattribute");

class bootstrapDateAttribute extends atkAttribute
{
    function bootstrapDateAttribute($name, $flags = 0)
    {
        $this->atkAttribute($name, $flags);
    }

    function draw($record = "", $fieldprefix = "", $mode = "")
    {
        $id = $this->getHtmlId($fieldprefix);
        $pickerId = $id . '_picker';
        $fieldName = $fieldprefix . $this->fieldName();
        $value = $record[$this->fieldName()];

        $format = atktext('btkdate_format_edit');
        $dbFormat = 'YYYY-MM-DD';

        $params = array(
            'language' => atkConfig::getGlobal('language'),
            'pickTime' => false
        );

        $js = "";
        $js .= "jQuery(function ($) {";
        $js .= "  $('#" . $pickerId . "').datetimepicker(" . json_encode($params) . ");";
        $js .= "  $('#" . $pickerId . "').data('DateTimePicker').setDate(new moment('" . $value . "'));";
        $js .= "  $('#" . $pickerId . "').on('dp.change', function(e){";
        $js .= "     $('#" . $id . "').val($('#" . $pickerId . "').data('DateTimePicker').getDate().format('" . $dbFormat . "'));";
        $js .= "  });";
        $js .= "});";

        $page = atkinstance('atk.ui.atkpage');
        $srcPath = atkConfig::getGlobal('atkroot') . 'atk/themes/bootstrap/src/bootstrap-datetimepicker/build/';
        $page->register_script($srcPath . 'js/bootstrap-datetimepicker.min.js');
        $page->register_style($srcPath . 'css/bootstrap-datetimepicker.min.css');
        $page->register_scriptcode($js);

        $result = '<div class="row"><div class="col-md-3">';
        $result .= '  <div class="input-group date" id="' . $pickerId . '">';
        $result .= '    <input type="text" class="form-control" data-date-format="' . $format . '" />';
        $result .= '    <input type="hidden" id="' . $id . '" name="' . $fieldName . '" value="' . $value . '" />';
        $result .= '    <span class="input-group-addon"><span class="glyphicon glyphicon-calendar"></span>';
        $result .= '  </div>';
        $result .= '</div></div>';

        return $result;
    }

    function edit($record = "", $fieldprefix = "", $mode = "")
    {
        $dateEdit = $this->draw($record, $fieldprefix, $mode);
        return $dateEdit;
    }

    function display($record, $mode)
    {
        $val = $record[$this->fieldName()];
        $m = new \Moment\Moment($val);

        $result = $m->format(atktext('btkdate_format_view'), new \Moment\CustomFormats\MomentJs());

        return $result;
    }
}