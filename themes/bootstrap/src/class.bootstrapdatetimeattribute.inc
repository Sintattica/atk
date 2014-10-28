<?php

// https://github.com/Eonasdan/bootstrap-datetimepicker
useattrib("atkattribute");

atkimport('atk.utils.atkmomentphpprovider');

class bootstrapDateTimeAttribute extends atkAttribute
{
    var $m_bootstrapdatetime_format_edit;
    var $m_bootstrapdatetime_format_view;
    var $m_db_format; //MomentJS Format
    var $m_type = 'datetime';
    var $m_pickerparams = array();
    var $m_disabledDates = array();
    var $m_minDate;
    var $m_maxDate;
    var $m_sideBySide = true;

    function bootstrapDateTimeAttribute($name, $flags = 0)
    {
        $this->atkAttribute($name, $flags);
    }

    function postInit()
    {
        if ($this->m_type == 'datetime') {
            if (!$this->m_bootstrapdatetime_format_edit) $this->m_bootstrapdatetime_format_edit = atktext('btkdatetime_format_edit');
            if (!$this->m_bootstrapdatetime_format_view) $this->m_bootstrapdatetime_format_view = atktext('btkdatetime_format_view');
            $this->m_db_format = 'YYYY-MM-DD HH:mm:ss';
        } else if ($this->m_type == 'date') {
            if (!$this->m_bootstrapdatetime_format_edit) $this->m_bootstrapdatetime_format_edit = atktext('btkdate_format_edit');
            if (!$this->m_bootstrapdatetime_format_view) $this->m_bootstrapdatetime_format_view = atktext('btkdate_format_view');
            $this->m_db_format = 'YYYY-MM-DD';
        } else if ($this->m_type == 'time') {
            if (!$this->m_bootstrapdatetime_format_edit) $this->m_bootstrapdatetime_format_edit = atktext('btktime_format_edit');
            if (!$this->m_bootstrapdatetime_format_view) $this->m_bootstrapdatetime_format_view = atktext('btktime_format_view');
            $this->m_db_format = 'HH:mm:ss';
        }

        if (!$this->m_db_format) {
            atkerror('bootstrapDateAttribute must have Date or Time');
        }
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.
     */
    function dbFieldType()
    {
        return $this->m_type;
    }

    function setType($type)
    {
        $this->m_type = $type;
        return $this;
    }

    function setPickerParams($pickerparams)
    {
        $this->m_pickerparams = $pickerparams;
        return $this;
    }

    /**
     * Set the format for the boxes in view mode.
     *
     * @param String $format_view The format (see format for momentJs function)
     */
    function setFormatView($format_view)
    {
        $this->m_bootstrapdatetime_format_view = $format_view;
        return $this;
    }

    /**
     * Set the format for the boxes in edit mode.
     *
     * @param String $format_view The format (see format for momentJs function)
     */
    function setFormatEdit($format_edit)
    {
        $this->m_bootstrapdatetime_format_edit = $format_edit;
        return $this;
    }


    function draw($record = "", $fieldprefix = "", $postfix = "", $mode = "")
    {
        $fieldName = $id = $fieldprefix . $this->fieldName() . $postfix;
        $pickerId = $id . '_picker';

        $value = $this->arrayToValue($record[$this->fieldName()]);
        $icon = 'glyphicon-calendar';

        if ($this->m_type == 'time') {
            $icon = 'glyphicon-time';
            $value = '1970-01-01 ' . $value;
        }

        $params = array(
            'language' => atkConfig::getGlobal('language'),
            'pickTime' => ($this->m_type == 'datetime' || $this->m_type == 'time'),
            'pickDate' => ($this->m_type == 'datetime' || $this->m_type == 'date'),
            'sideBySide' => $this->m_sideBySide
        );

        if (count($this->m_disabledDates)) {
            $dates = array();
            foreach ($this->m_disabledDates as $d) {
                $dates[] = 'function:moment("' . $d . '")';
            }

            $params['disabledDates'] = $dates;
        }

        if ($this->m_minDate) {
            $params['minDate'] = "function:moment('" . $this->m_minDate . "')";
        }

        if ($this->m_maxDate) {
            $params['maxDate'] = "function:moment('" . $this->m_maxDate . "')";
        }


        $params = array_merge($params, $this->m_pickerparams);
        $params = $this->json_stringify($params);

        $js = "";
        $js .= "jQuery(function ($) {";
        $js .= "  $('#" . $pickerId . "').datetimepicker(" . $params . ");";
        $js .= "  $('#" . $pickerId . "').on('dp.change', function(e){";
        $js .= "     $('#" . $id . "').val($('#" . $pickerId . "').data('DateTimePicker').getDate().format('" . $this->m_db_format . "'));";
        $js .= "  });";
        if ($value) {
            $js .= "  $('#" . $pickerId . "').data('DateTimePicker').setDate(moment('" . $value . "'));";
        }
        $js .= "});";

        $page = atkinstance('atk.ui.atkpage');
        $srcPath = atkConfig::getGlobal('atkroot') . 'atk/themes/bootstrap/lib/bootstrap-datetimepicker/build/';
        $page->register_script($srcPath . 'js/bootstrap-datetimepicker.min.js');
        $page->register_style($srcPath . 'css/bootstrap-datetimepicker.min.css');
        $page->register_scriptcode($js);

        $result = '';
        $result .= '    <div class="input-group date col-md-4" id="' . $pickerId . '">';
        $result .= '      <input type="text" class="form-control" data-date-format="' . $this->m_bootstrapdatetime_format_edit . '" />';
        $result .= '      <input type="hidden" id="' . $id . '" name="' . $fieldName . '" />';
        $result .= '      <span class="input-group-addon"><span class="glyphicon ' . $icon . '"></span>';
        $result .= '    </div>';

        return $result;
    }

    function edit($record = "", $fieldprefix = "", $mode = "")
    {
        $dateEdit = $this->draw($record, $fieldprefix, '', $mode);
        return $dateEdit;
    }

    function display($record, $mode)
    {
        $value = $this->arrayToValue($record[$this->fieldName()]);
        if (!$value) return null;

        $m = new \Moment\Moment($value);
        $result = $m->format($this->m_bootstrapdatetime_format_view, atkMomentphpProvider::getFormatInstance());
        return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param array $record Array with values
     * @param string $fieldprefix The fieldprefix
     * @return Piece of htmlcode
     */
    function hide($record = '', $fieldprefix = '')
    {
        $result = '';
        $field = $record[$this->fieldName()];

        if (is_array($field)) {
            foreach ($field as $key => $value)
                $result .= '<input type="hidden" name="' . $fieldprefix . $this->formName() . '[' . $key . ']" ' . 'value="' . $value . '">';
        } else {
            $result = '<input type="hidden" name="' . $fieldprefix . $this->formName() . '" value="' . $field . '">';
        }
        return $result;
    }


    /**
     * Returns a piece of html code that can be used in a form to search values.
     * Searching is disabled for the date attribute, we only return a space.
     *
     * @param array $record
     * @param boolean $extended if set to false, a simple search input is
     *                          returned for use in the searchbar of the
     *                          recordlist. If set to true, a more extended
     *                          search may be returned for the 'extended'
     *                          search page. The atkAttribute does not
     *                          make a difference for $extended is true, but
     *                          derived attributes may reimplement this.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     * @return piece of HTML code
     */
    function search($record = "", $extended = false, $fieldprefix = "")
    {
        if (!$extended) {
            $res = $this->draw($record, "atksearch_AE_" . $fieldprefix, '', 'search');

            return $res;
        }

        // Set default values to null.
        if (!isset($record[$this->fieldName()]) || empty($record[$this->fieldName()]))
            $record[$this->fieldName()] = NULL;

        $rec = isset($record[$this->fieldName()]['from']) ? array($this->fieldName() => $record[$this->fieldName()]['from'])
            : $record;
        $res = $this->draw($rec, 'atksearch_AE_' . $fieldprefix, '_AE_from', 'search');
        $rec = isset($record[$this->fieldName()]['to']) ? array($this->fieldName() => $record[$this->fieldName()]['to'])
            : $record;
        $res .= atktext("until") . $this->draw($rec, 'atksearch_AE_' . $fieldprefix, "_AE_to", 'search');

        return $res;
    }

    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * @param atkQuery $query The query object where the search condition should be placed on
     * @param String $table The name of the table in which this attribute
     *                              is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param String $searchmode The searchmode to use. This can be any one
     *                              of the supported modes, as returned by this
     *                              attribute's getSearchModes() method.
     * @return String The searchcondition to use.
     */
    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
        $db = & $this->getDb();

        // If we search through datagrid we got no from/to values
        // Therefore we will simulate them
        if (!is_array($value)) {
            if (substr_count($value, '/') == 1) {
                list($from, $to) = explode('/', $value);
                $value = array('from' => trim($from), 'to' => trim($to));
            } else if (substr_count($value, '/') == 5) {
                $parts = explode('/', $value);
                $value = array('from' => trim($parts[0] . '/' . $parts[1] . '/' . $parts[2]),
                    'to' => trim($parts[3] . '/' . $parts[4] . '/' . $parts[5]));
            } else {
                $value = array('from' => $value, 'to' => $value);
            }
        }

        $valueFrom = $this->fetchValue(array($this->fieldName() => $value["from"]));
        $valueTo = $this->fetchValue(array($this->fieldName() => $value["to"]));

        $fromval = $this->value2db(array($this->fieldName() => $valueFrom));
        $toval = $this->value2db(array($this->fieldName() => $valueTo));

        if ($this->m_type == 'datetime') {
            $field = $db->func_datetimetochar($table . "." . $this->fieldName());
        } else if ($this->m_type == 'date') {
            $field = $db->func_datetochar($table . "." . $this->fieldName());
        } else if ($this->m_type == 'time') {
            $field = $table . "." . $this->fieldName();
        }


        if ($fromval == NULL && $toval == NULL)
            ; // do nothing
        else if ($fromval != NULL && $toval != NULL) {
            if ($fromval > $toval) {
                // User entered dates in wrong order. Let's put them in the right order.
                $tmp = $fromval;
                $fromval = $toval;
                $toval = $tmp;
            }
            $searchcondition = $query->betweenCondition(
                $field, $fromval, $toval);
        } else if ($fromval != NULL && $toval == NULL) {
            $searchcondition = $query->greaterthanequalCondition(
                $field, $fromval);
        } else if ($fromval == NULL && $toval != NULL) {
            $searchcondition = $query->lessthanequalCondition(
                $field, $toval);
        } else if ((is_array($value["from"])) or (is_array($value["to"]))) {
            $searchcondition = $this->_getDateArraySearchCondition($query, $table, $value);
        } else {
            // plain text search condition
            $value = $this->_autoCompleteDateString($value);
            $searchcondition = $query->exactCondition($field, $value);
        }

        return $searchcondition;
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * Note that not all modes may be supported by the database driver.
     * Compare this list to the one returned by the databasedriver, to
     * determine which searchmodes may be used.
     *
     * @return array List of supported searchmodes
     */
    function getSearchModes()
    {
        return array("between");
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

            $result = array();
            if ($this->m_type == 'datetime' || $this->m_type == 'date') {
                $result['year'] = $m->format('YYYY', $format);
                $result['month'] = $m->format('M', $format);
                $result['day'] = $m->format('D', $format);
            }

            if ($this->m_type == 'datetime' || $this->m_type == 'time') {
                $result['hours'] = $m->format('H', $format);
                $result['minutes'] = $m->format('m', $format);
                $result['seconds'] = $m->format('s', $format);
            }

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
        $result = null;
        if ($this->m_type == 'datetime') {
            if (empty($a['year']) || empty($a['month']) || empty($a['day']))
                return null;
            if (empty($a['hours']) || empty($a['minutes']) || empty($a['seconds']))
                return null;

            $date = sprintf('%04d-%02d-%02d', $a['year'], $a['month'], $a['day']);
            $time = sprintf('%02d:%02d:%02d', $a['hours'], $a['minutes'], $a['seconds']);
            $result = $date . ' ' . $time;
        } else if ($this->m_type == 'date') {
            if (empty($a['year']) || empty($a['month']) || empty($a['day']))
                return null;

            $result = sprintf('%04d-%02d-%02d', $a['year'], $a['month'], $a['day']);
        } else if ($this->m_type == 'time') {
            if (empty($a['hours']) || empty($a['minutes']) || empty($a['seconds']))
                return null;

            $result = sprintf('%02d:%02d:%02d', $a['hours'], $a['minutes'], $a['seconds']);
        }


        return $result;
    }

    function addDisabledDates($dates)
    {
        if (!is_array($dates)) {
            $dates = array($dates);
        }

        $this->m_disabledDates = array_merge($this->m_disabledDates, $dates);
        return $this;
    }


    function json_stringify($input)
    {
        $outtext = '';
        $opening = '{';
        $closing = '}';
        $inner = array();
        $numericarray = array_keys($input) === range(0, count($input) - 1);
        if ($numericarray) {
            // This is a numerically sequential array
            $opening = '[';
            $closing = ']';
        }
        foreach ($input as $key => $val) {
            if (is_string($val) && preg_match("/^function\:/", $val)) {
                // The value is a string and begins with 'function:'. Do not encase it in quotes
                $val = substr($val, 9);
            } else if (is_int($val)) {
                // The value is an integer
                $val = (int)$val;
            } else if (is_float($val)) {
                // The value is a float
                $val = (float)$val;
            } else if (is_bool($val)) {
                if ($val) {
                    $val = 'true';
                } else {
                    $val = 'false';
                }
            } else {
                $val = is_array($val) ? self::json_stringify($val) : "\"$val\"";
            }
            $inner[] = ($numericarray ? '' : "\"$key\":") . $val;
        }
        $outtext .= implode(',', $inner);
        return $opening . $outtext . $closing;
    }

    function setMinDate($date)
    {
        $this->m_minDate = $date;
    }

    function setMaxDate($date)
    {
        $this->m_maxDate = $date;
    }

    function setSideBySide($side)
    {
        $this->m_sideBySide = $side;
    }
}