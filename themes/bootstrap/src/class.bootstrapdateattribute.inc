<?php

// https://github.com/Eonasdan/bootstrap-datetimepicker
useattrib("atkattribute");

class bootstrapDateAttribute extends atkAttribute
{

    var $m_date_format_edit;
    var $m_date_format_view;


    function bootstrapDateAttribute($name, $flags = 0)
    {
        $this->m_date_format_edit = atktext('btkdate_format_edit');
        $this->m_date_format_view = atktext('btkdate_format_view');

        $this->atkAttribute($name, $flags);
    }

    function draw($record = "", $fieldprefix = "", $mode = "")
    {

        $id = $this->getHtmlId($fieldprefix);
        $pickerId = $id . '_picker';
        $fieldName = $fieldprefix . $this->fieldName();
        $value = $this->arrayToValue($record[$this->fieldName()]);

        $dbFormat = 'YYYY-MM-DD hh:mm:ss';

        $params = array(
            'language' => atkConfig::getGlobal('language'),
            'pickTime' => true
        );

        $js = "";
        $js .= "jQuery(function ($) {";
        $js .= "  $('#" . $pickerId . "').datetimepicker(" . json_encode($params) . ");";
        $js .= "  $('#" . $pickerId . "').on('dp.change', function(e){";
        $js .= "     $('#" . $id . "').val($('#" . $pickerId . "').data('DateTimePicker').getDate().format('" . $dbFormat . "'));";
        $js .= "  });";

        if ($value) {
            $js .= "  $('#" . $pickerId . "').data('DateTimePicker').setDate(new moment('" . $value . "'));";
        }

        $js .= "});";

        $page = atkinstance('atk.ui.atkpage');
        $srcPath = atkConfig::getGlobal('atkroot') . 'atk/themes/bootstrap/src/bootstrap-datetimepicker/build/';
        $page->register_script($srcPath . 'js/bootstrap-datetimepicker.min.js');
        $page->register_style($srcPath . 'css/bootstrap-datetimepicker.min.css');
        $page->register_scriptcode($js);

        $result = '<div class="row"><div class="col-md-3">';
        $result .= '  <div class="input-group date" id="' . $pickerId . '">';
        $result .= '    <input type="text" class="form-control" data-date-format="' . $this->m_date_format_edit . '" />';
        $result .= '    <input type="hidden" id="' . $id . '" name="' . $fieldName . '" />';
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

        $value = $this->arrayToValue($record[$this->fieldName()]);
        if (!$value) return null;

        $m = new \Moment\Moment($value);

        $result = $m->format($this->m_date_format_view, atkMomentphpProvider::getFormatInstance());

        return $result;
    }

    /**
     * Convert database value to date array
     * @param array $rec database record with date field
     * @return array with 6 fields (year, month, day, hours, minutes, seconds)
     */
    function db2value($rec)
    {
        if (!isset($rec[$this->fieldName()]) || strlen($rec[$this->fieldName()]) == 0) {
            return null;
        }

        try {
            $m = new \Moment\Moment($rec[$this->fieldName()]);
            $format = atkMomentphpProvider::getFormatInstance();
            $result = array(
                'year' => $m->format('YYYY', $format),
                'month' => $m->format('M', $format),
                'day' => $m->format('D', $format),
                'hours' => $m->format('H', $format),
                'minutes' => $m->format('m', $format),
                'seconds' => $m->format('s', $format)
            );
            return $result;
        } catch (\Moment\MomentException $e) {

        }
        return null;
    }

    function value2db($rec)
    {
        return $rec[$this->fieldName()];
    }

    function arrayToValue($a)
    {
        if (empty($a['year']) || empty($a['month']) || empty($a['day']))
            return null;
        if ($a['year'] == '' || $a['month'] == 0 || $a['day'] == 0)
            return null;

        $date = sprintf('%04d-%02d-%02d', $a['year'], $a['month'], $a['day']);
        $time = sprintf('%02d:%02d:%02d', $a['hours'], $a['minutes'], $a['seconds']);

        $result = $date . ' ' . $time;
        return $result;
    }

    /**
     * Set the format for the boxes in view mode.
     *
     * @param String $format_view The format (see format for momentJs function)
     */
    function setFormatView($format_view)
    {
        $txt_date_format_view = atktext("btkdate_format_view", "atk", "", "", "", true);

        if (!empty($format_view))
            $this->m_date_format_view = $format_view;
        elseif (!empty($txt_date_format_view))
            $this->m_date_format_view = $txt_date_format_view;
        else
            $this->m_date_format_view = "YYYY-MM-DD HH:mm:ss";
    }

    /**
     * Set the format for the boxes in edit mode.
     *
     * @param String $format_view The format (see format for momentJs function)
     */
    function setFormatEdit($format_edit)
    {
        $txt_date_format_edit = atktext("btkdate_format_edit", "atk", "", "", "", true);

        if (!empty($format_edit))
            $this->m_date_format_edit = $format_edit;
        elseif (!empty($txt_date_format_edit))
            $this->m_date_format_edit = $txt_date_format_edit;
        else
            $this->m_date_format_edit = "YYYY-MM-DD hh:mm:ss";
    }
}