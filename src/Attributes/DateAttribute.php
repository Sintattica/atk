<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Utils\BrowserInfo;
use Sintattica\Atk\Utils\Json;

/**
 * The DateAttribute class offers a date widget for date fields.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @author Tom Schenkenberg <tom@achievo.org>
 */
class DateAttribute extends Attribute
{
    /** flag(s) specific for atkDateAttribute */
    const AF_DATE_STRING = 33554432; // date must be entered as an english date string (strtotime), also checks edit format
    const AF_DATE_EMPTYFIELD = 67108864; // Fields have one empty option
    const AF_DATE_NO_CALENDAR = 134217728; // Do not append the popup calendar.
    const AF_DATE_DISPLAY_DAY = 268435456; // Show the day of the week in the display
    const AF_DATE_EDIT_NO_DAY = 536870912; // Don't display the day of the week in edit mode
    const AF_DATE_DEFAULT_EMPTY = 1073741824; // Display butons to clear and 'touch' date

    /**
     * Possible values for sorting the year dropdown.
     */
    const SORT_YEAR_ASC = 1;
    const SORT_YEAR_DESC = 2;

    /**
     * Static var to keep track if js scripts are already registered.
     *
     * @var bool
     */
    protected static $s_baseScriptsRegistered = false;

    /**
     * Are we in simple mode?
     *
     * @see DateAttribute::setSimpleMode()
     *
     * @var bool
     */
    protected $m_simplemode = false;

    /**
     * Do we have a year dropdown.
     *
     * @var bool
     */
    protected $m_yeardropdown = false;

    /**
     * sorting of the year dropdown.
     *
     * @var int
     */
    protected $m_year_sorting = self::SORT_YEAR_ASC;
    public $m_date_min;
    public $m_date_max;
    public $m_date_format_edit;
    public $m_date_format_view;
    public $m_maxyears = 25;

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_DATE;

    /**
     * Format date according to a format string.
     *
     * @param array $date date array (gotten with getdate())
     * @param string $format format string, compatible with PHP's date format functions
     * @param bool $weekday include day-of-week or not
     *
     * @return string with formatted date
     */
    public function formatDate($date, $format, $weekday = true)
    {
        return Tools::atkFormatDate($date, $format, $weekday);
    }

    /**
     * Returns the days in a certain month in a certain year.
     *
     * @param array $date date array (gotten with getdate())
     *
     * @return int with number of days
     */
    public function getDays($date)
    {
        /* the last day of any given month can be expressed as the "0" day of the next month! */
        if (isset($date['mon']) && isset($date['year'])) {
            $date = adodb_getdate(adodb_mktime(0, 0, 0, $date['mon'] + 1, 0, $date['year']));

            return $date['mday'];
        }

        return 31;
    }

    /**
     * Converts a date string (YYYYMMDD) to an
     * array with 3 fields (day, month, year).
     *
     * @param string $date the date string
     *
     * @return array with 3 fields (day, month, year)
     */
    public static function dateArray($date = null)
    {
        if ($date == null) {
            $date = date('Ymd');
        }

        if (strstr($date, '-')) {
            return array(
                'day' => substr($date, 8, 2),
                'month' => substr($date, 5, 2),
                'year' => substr($date, 0, 4),
            );
        } else {
            return array(
                'day' => substr($date, 6, 2),
                'month' => substr($date, 4, 2),
                'year' => substr($date, 0, 4),
            );
        }
    }

    /**
     * Converts a date array to a timestamp
     * year, month, day are obligatory !!
     *
     * @param array $dateArray Date Array
     *
     * @return int Timestamp
     */
    public function arrayToTime($dateArray)
    {
        return self::_arrayToTime($dateArray);
    }

    /**
     * Validates a given date array.
     *
     * @param array $datearray Array with 3 fields (day, month, year)
     *
     * @return bool True if valid, false if not.
     */
    public function checkDateArray($datearray)
    {
        return checkdate((int)$datearray['month'], (int)$datearray['day'], (int)$datearray['year']);
    }

    /**
     * Converts a date array to a timestamp
     * year, month, day are obligatory !!
     *
     * @param array $dateArray Date Array
     *
     * @return int Timestamp
     */
    public function _arrayToTime($dateArray)
    {
        $hour = 0;
        $min = 0;
        $sec = 0;
        $dateValid = true;
        $month = false;
        $day = false;
        $year = false;

        if (!empty($dateArray['hour'])) {
            $hour = $dateArray['hour'];
        }
        if (!empty($dateArray['min'])) {
            $min = $dateArray['min'];
        }
        if (!empty($dateArray['sec'])) {
            $sec = $dateArray['sec'];
        }
        if (!empty($dateArray['day'])) {
            $day = $dateArray['day'];
        } else {
            $dateValid = false;
        }
        if (!empty($dateArray['month'])) {
            $month = $dateArray['month'];
        } else {
            $dateValid = false;
        }
        if (!empty($dateArray['year'])) {
            $year = $dateArray['year'];
        } else {
            $dateValid = false;
        }

        if ($dateValid) {
            return adodb_mktime($hour, $min, $sec, $month, $day, $year);
        }

        return adodb_mktime(0, 0, 0);
    }

    /**
     * Default Constructor, sets up the atkDateAttribute
     * The API of this method has changed, but is has been made
     * backwards compatible with existing modules!
     *
     * @param string $name the attribute's name
     * @param int $flags the attribute's flags
     * @param string $format_edit the format the edit/add box(es) will look like
     * @param string $format_view the format in which dates are listed
     * @param mixed $min the minimum date that has to be selected (0 is unlimited)
     * @param mixed $max the maximum date that may be selected (0 is unlimited)
     *
     * @see Attribute
     */
    public function __construct($name, $flags = 0, $format_edit = '', $format_view = '', $min = 0, $max = 0)
    {

        /* edit and display date format */
        $this->setFormatEdit($format_edit);
        $this->setFormatView($format_view);

        /* max / min date */
        $this->setDateMin($min);
        $this->setDateMax($max);

        /* base class constructor */
        parent::__construct($name, $flags);

        if($this->hasFlag(self::AF_OBLIGATORY) && $this->hasFlag(self::AF_DATE_DEFAULT_EMPTY)){
            $this->addFlag(self::AF_DATE_EMPTYFIELD);
        }
    }

    /**
     * Set the format for the boxes in edit mode.
     *
     * @param string $format_edit The format (see format for date() function)
     */
    public function setFormatEdit($format_edit)
    {
        $txt_date_format_edit = Tools::atktext('date_format_edit', 'atk', '', '', '', true);

        if ($this->hasFlag(self::AF_DATE_STRING) && empty($format_edit)) {
            $this->m_date_format_edit = 'Y-m-d';
        } elseif (!empty($format_edit)) {
            $this->m_date_format_edit = $format_edit;
        } elseif (!empty($txt_date_format_edit)) {
            $this->m_date_format_edit = $txt_date_format_edit;
        } else {
            $this->m_date_format_edit = 'F j Y';
        }
    }

    /**
     * Set the format for the boxes in view mode.
     *
     * @param string $format_view The format (see format for date() function)
     */
    public function setFormatView($format_view)
    {
        $txt_date_format_view = Tools::atktext('date_format_view', 'atk', '', '', '', true);

        if (!empty($format_view)) {
            $this->m_date_format_view = $format_view;
        } elseif (!empty($txt_date_format_view)) {
            $this->m_date_format_view = $txt_date_format_view;
        } else {
            $this->m_date_format_view = 'F j Y';
        }
    }

    /**
     * Set the maximum date that may be select (0 means unlimited).
     * It can be set in 3 formats:
     * 1. Unix timestamp.
     * 2. String (parsed by strtotime)
     * 3. Array (with year,month,day,hour,min,sec).
     *
     * @param mixed $max The maximum date that may be selected.
     */
    public function setDateMax($max = 0)
    {
        if ($max === 0) {
            $this->m_date_max = 0;
        } else {
            if (is_array($max)) {
                $this->m_date_max = $this->_arrayToTime($max);
            } else {
                if (is_integer($max)) {
                    $this->m_date_max = $max;
                } else {
                    $this->m_date_max = strtotime($max);
                }
            }
        }
    }

    /**
     * Set the minimum date that may be select (0 means unlimited).
     * It can be set in 3 formats:
     * 1. Unix timestamp.
     * 2. String (parsed by strtotime)
     * 3. Array (with year,month,day,hour,min,sec).
     *
     * @param mixed $min The minimum date that may be selected.
     */
    public function setDateMin($min = 0)
    {
        if ($min === 0) {
            $this->m_date_min = 0;
        } else {
            if (is_array($min)) {
                $this->m_date_min = $this->_arrayToTime($min);
            } else {
                if (is_integer($min)) {
                    $this->m_date_min = $min;
                } else {
                    $this->m_date_min = strtotime($min);
                }
            }
        }
    }

    public function postInit()
    {
        parent::postInit();
        if ($this->hasFlag(self::AF_OBLIGATORY) && !$this->hasFlag(self::AF_DATE_DEFAULT_EMPTY)) {
            $this->setInitialValue(self::dateArray(date('Ymd')));
        }
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string A piece of htmlcode for editing this attribute
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);

        return $this->draw($record, $id, $name, $fieldprefix, '', $mode, $this->hasFlag(self::AF_OBLIGATORY));
    }

    /**
     * Renders a year dropdown or text box.
     *
     * @param string $fieldid
     * @param string $fieldname current fieldname
     * @param string $str_script onchange script
     * @param array $current current array
     * @param string $format current format
     * @param bool $obligatory
     *
     * @return string
     */
    protected function renderYear($fieldid, $fieldname, $str_script, $current, $format, $obligatory)
    {
        $result = '';
        $emptyfield = null;
        /* date must be within specified (default: 25) years */
        if (!empty($current['y_max']) && !empty($current['y_min']) && $current['y_max'] - $current['y_min'] <= $this->m_maxyears) {
            $result .= '<select id="'.$fieldid.'[year]" name="'.$fieldname.'[year]" class="atkdateattribute form-control select-standard" onChange="'.$str_script.'">';
            if (!$obligatory || $this->hasFlag(self::AF_DATE_EMPTYFIELD)) {
                $result .= '<option value="0"'.($current === null ? ' selected' : '').'></option>';
            }

            if (empty($current['mon']) && !$emptyfield) {
                $current['mon'] = 1;
            }
            if (empty($current['mday']) && !$emptyfield) {
                $current['mday'] = 1;
            }

            if ($this->m_year_sorting == self::SORT_YEAR_DESC) {
                for ($j = $current['y_max']; $j >= $current['y_min']; --$j) {
                    $tmp_date = adodb_getdate(adodb_mktime(0, 0, 0, $current['mon'], $current['mday'], $j));
                    $str_year = $this->formatDate($tmp_date, $format);
                    $result .= '<option value="'.$j.'" '.($current !== null && $j == $current['year'] ? 'selected' : '').'>'.$str_year.'</option>';
                }
            } else {
                for ($j = $current['y_min']; $j <= $current['y_max']; ++$j) {
                    $tmp_date = adodb_getdate(adodb_mktime(0, 0, 0, $current['mon'], $current['mday'], $j));
                    $str_year = $this->formatDate($tmp_date, $format);
                    $result .= '<option value="'.$j.'" '.($current !== null && $j == $current['year'] ? 'selected' : '').'>'.$str_year.'</option>';
                }
            }

            $result .= '</select>';

            $this->m_yeardropdown = true;
        } /* normal input box */ else {
            $result .= '<input type="text" id="'.$fieldid.'[year]" name="'.$fieldname.'[year]" class="atkdateattribute form-control" size="4" maxlength="4" onChange="'.$str_script.'" value="'.(isset($current['year']) ? $current['year'] : '').'">';
        }

        return $result;
    }

    /**
     * Renders month combo.
     *
     * @param string $fieldid
     * @param string $fieldname current fieldname
     * @param string $str_script onchange script
     * @param array $current current array
     * @param string $format current format
     * @param bool $obligatory
     *
     * @return string
     */
    protected function renderMonth($fieldid, $fieldname, $str_script, $current, $format, $obligatory)
    {
        $result = '<select id="'.$fieldid.'[month]" name="'.$fieldname.'[month]" class="atkdateattribute form-control select-standard" onChange="'.$str_script.'">';
        if (!$obligatory || $this->hasFlag(self::AF_DATE_EMPTYFIELD)) {
            $result .= '<option value=""'.($current === null ? ' selected' : '').'></option>';
        }
        if (!$this->m_simplemode) {
            for ($j = $current['m_min']; $j <= $current['m_max']; ++$j) {
                $tmp_date = adodb_getdate(adodb_mktime(0, 0, 0, $j, 1, (isset($current['year']) ? $current['year'] : 0)));
                $str_month = $this->formatDate($tmp_date, $format);
                $result .= '<option value="'.$j.'" '.(isset($current['mon']) && $j == $current['mon'] ? 'selected' : '').'>'.$str_month.'</option>';
            }
        } else {
            for ($j = 1; $j <= 12; ++$j) {
                $result .= '<option value="'.$j.'" '.($current !== null && $j == $current['mon'] ? 'selected' : '').'>'.sprintf('%02d', $j).'</option>';
            }
        }
        $result .= '</select>';

        return $result;
    }

    /**
     * Renders the day dropdown.
     *
     * @param string $fieldid
     * @param string $fieldname current fieldname
     * @param string $str_script onchange script
     * @param array $current current array
     * @param string $format current format
     * @param bool $obligatory
     * @param string $weekdayFormat
     *
     * @return string
     * */
    protected function renderDay($fieldid, $fieldname, $str_script, $current, $format, $obligatory, $weekdayFormat)
    {
        $c_mon = isset($current['mon']) ? $current['mon'] : null;
        $c_year = isset($current['year']) ? $current['year'] : null;
        $c_dmin = isset($current['d_min']) ? $current['d_min'] : null;
        $c_dmax = isset($current['d_max']) ? $current['d_max'] : null;
        $c_mday = isset($current['mday']) ? $current['mday'] : null;

        $result = '<select id="'.$fieldid.'[day]" name="'.$fieldname.'[day]" class="atkdateattribute form-control select-standard" onChange="'.$str_script.'">';
        if (!$obligatory || $this->hasFlag(self::AF_DATE_EMPTYFIELD)) {
            $result .= '<option value=""'.($current === null ? ' selected' : '').'></option>';
        }
        if (!$this->m_simplemode) {
            for ($j = $c_dmin; $j <= $c_dmax; ++$j) {
                $tmp_date = adodb_getdate(adodb_mktime(0, 0, 0, $c_mon, $j, $c_year));
                if (($c_year != '') && ($c_mon != '')) {
                    $str_day = $this->formatDate($tmp_date, (empty($weekdayFormat) ? $format : "$weekdayFormat {$format}"),
                        !$this->hasFlag(self::AF_DATE_EDIT_NO_DAY));
                } else {
                    $str_day = $this->formatDate($tmp_date, (empty($weekdayFormat) ? $format : "$weekdayFormat {$format}"), 0);
                }
                $result .= '<option value="'.$j.'" '.($current !== null && $j == $c_mday ? 'selected' : '').'>'.$str_day.'</option>';
            }
        } else {
            for ($j = 1; $j <= 31; ++$j) {
                $result .= '<option value="'.$j.'" '.($current !== null && $j == $c_mday ? 'selected' : '').'>'.sprintf('%02d', $j).'</option>';
            }
        }
        $result .= '</select>';

        return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value. (Month will be a dropdownbox, year and day text fields).
     *
     * @todo We can't show a calendar when we have a year dropdown?
     * @todo The calendar doesn't use the min/max values?
     *
     * @param array $record Array with 3 fields (year, month, day)
     * @param string $id html id
     * @param string $name html name
     * @param string $fieldprefix The fieldprefix
     * @param string $postfix
     * @param string $mode The mode ('add' or 'edit')
     * @param bool $obligatory Is this field obligatory or not
     *
     * @return string Piece a of HTML Code
     */
    public function draw($record = [], $id, $name, $fieldprefix = '', $postfix = '', $mode = '', $obligatory = false)
    {
        $result = '';

        // go in simplemode when a pda is detected
        if (BrowserInfo::detectPDA()) {
            $this->setSimpleMode(true);
        }

        $this->m_yeardropdown = false;

        if (!$this->m_simplemode) {
            self::registerScriptsAndStyles(!$this->hasFlag(self::AF_DATE_NO_CALENDAR));
        }

        $fieldname = $name.$postfix;

        /* text mode? */
        if ($this->hasFlag(self::AF_DATE_STRING) || $mode == 'list') {
            $value = &$record[$this->fieldName()];

            if (is_array($value)) {
                $value = adodb_date($this->m_date_format_edit, adodb_mktime(0, 0, 0, $value['month'], $value['day'], $value['year']));
            } elseif ($obligatory) {
                $value = adodb_date($this->m_date_format_edit);
            } else {
                $value = '';
            }

            $fieldname = $fieldname.'[date]';
            $result = '<input type="text" id="'.$id.'" class="atkdateattribute form-control" name="'.$fieldname.'" value="'.$value.'">';

            if (!$this->hasFlag(self::AF_DATE_NO_CALENDAR) && $mode != 'list') {
                $format = str_replace(array('y', 'Y', 'm', 'n', 'j', 'd'), array('yy', 'y', 'mm', 'm', 'd', 'dd'), $this->m_date_format_edit);
                $mondayFirst = 'false';
                if (is_bool(Tools::atktext('date_monday_first'))) {
                    $mondayFirst = Tools::atktext('date_monday_first') === true ? 'true' : $mondayFirst;
                }
                $result .= ' <input '.$this->getCSSClassAttribute([
                        'btn',
                        'btn-default',
                        'button',
                        'atkbutton',
                        'form-control',
                    ]).' type="button" value="..." onclick="return showCalendar(\''.$id.'\', \''.$id.'\', \''.$format.'\', false, '.$mondayFirst.');">';
            }

            return $result;
        }

        /* this field */
        $field = Tools::atkArrayNvl($record, $this->fieldName());
        $str_format = $this->m_date_format_edit;

        /* currently selected date */
        if (is_array($field) && $field['year'] == 0 && $field['month'] == 0 && $field['day'] == 0) {
            $current = null;
        } /* NULL date selected (normal date selection) */ elseif (!is_array($field) && empty($field)) {
            $current = null;
        } /* NULL date selected (NULL value in database) */ elseif (is_array($field)) {
            if ($this->checkDateArray($field)) {
                $current = adodb_mktime(0, 0, 0, $field['month'], $field['day'], $field['year']);
            } else {
                $current = null;
                Tools::triggerError($record, $this->fieldName(), 'error_date_invalid');
            }
        } else {
            $date = self::dateArray($field);
            if ($this->checkDateArray($date)) {
                $current = adodb_mktime(0, 0, 0, $date['month'], $date['day'], $date['year']);
            } else {
                $current = null;
            }
        }

        /* minimum date */
        $minimum = $mode != 'search' ? $this->m_date_min : 0;
        if ($minimum != 0) {
            $str_min = adodb_date('Ymd', $minimum);
        } else {
            $str_min = 0;
        }

        /* maximum date */
        $maximum = $mode != 'search' ? $this->m_date_max : 0;
        if ($maximum != 0) {
            $str_max = adodb_date('Ymd', $maximum);
        } else {
            $str_max = 0;
        }

        $current = $this->getValidCurrentDate($current, $minimum, $maximum, $mode);

        /* get dates in array format */
        if ($current !== null) {
            $current = adodb_getdate($current);
        }
        if (!empty($minimum)) {
            $minimum = adodb_getdate($minimum);
        }
        if (!empty($maximum)) {
            $maximum = adodb_getdate($maximum);
        }

        /* minimum and maximum */
        $current['d_min'] = (!empty($minimum) && $current['year'] == $minimum['year'] && $current['mon'] == $minimum['mon'] ? $minimum['mday'] : 1);
        $current['d_max'] = (!empty($maximum) && $current['year'] == $maximum['year'] && $current['mon'] == $maximum['mon'] ? $maximum['mday'] : $this->getDays($current));
        $current['m_min'] = (!empty($minimum) && $current['year'] == $minimum['year'] ? $minimum['mon'] : 1);
        $current['m_max'] = (!empty($maximum) && $current['year'] == $maximum['year'] ? $maximum['mon'] : 12);
        $current['y_min'] = (!empty($minimum) ? $minimum['year'] : 0);
        $current['y_max'] = (!empty($maximum) ? $maximum['year'] : 0);

        /* small date selections, never possible is field isn't obligatory (no min/max date) */
        if (!empty($maximum) && !empty($minimum) && $str_max - $str_min < 25) {
            $str_script = '';
            if (Tools::count($this->m_onchangecode)) {
                $this->_renderChangeHandler($fieldprefix);
                $str_script = $this->getHtmlId($fieldprefix).'_onChange(this);';
            }

            $result = '<select id="'.$id.'" name="'.$fieldname.'" onChange="'.$str_script.'" class="form-control select-standard">';
            for ($i = $str_min; $i <= $str_max; ++$i) {
                $tmp_date = adodb_getdate(adodb_mktime(0, 0, 0, substr($i, 4, 2), substr($i, 6, 2), substr($i, 0, 4)));
                $result .= '<option value="'.$i.'"'.($current !== null && $tmp_date[0] == $current[0] ? ' selected' : '').'>'.$this->formatDate($tmp_date,
                        $str_format, !$this->hasFlag(self::AF_DATE_EDIT_NO_DAY)).'</option>';
            }
            $result .= '</select>';

            return $result;
        }

        if ($this->hasFlag(self::AF_DATE_EMPTYFIELD)) {
            $emptyfield = true;
        } else {
            if (!$obligatory) {
                $emptyfield = true;
            } else {
                $emptyfield = false;
            }
        }

        $info = array(
            'format' => $str_format,
            'min' => $str_min,
            'max' => $str_max,
            'emptyfield' => $emptyfield,
            'weekday' => !$this->hasFlag(self::AF_DATE_EDIT_NO_DAY),
        );

        if (!$this->m_simplemode) {
            $result .= '<div class="DateAttribute form-inline"><script>var atkdateattribute_'.$id.' = '.Json::encode($info).';</script>';
        }

        /* other date selections */
        $weekdayFormat = null;
        $str_script = '';
        for ($i = 0; $i < strlen($str_format); ++$i) {
            /* javascript method */
            if (!$this->m_simplemode) {
                $str_script = "ATK.DateAttribute.adjustDate(this, '".$id."');";
            }

            if (Tools::count($this->m_onchangecode)) {
                $this->_renderChangeHandler($fieldprefix);
                $str_script .= $this->getHtmlId($fieldprefix).'_onChange(this);';
            }

            /* year input box */
            if ($str_format[$i] == 'y' || $str_format[$i] == 'Y') {
                $result .= $this->renderYear($id, $fieldname, $str_script, $current, $str_format[$i], $obligatory);
            } /* weekday format */ elseif ($str_format[$i] == 'D' || $str_format[$i] == 'l') {
                $weekdayFormat = $str_format[$i];
            } /* day input box */ elseif ($str_format[$i] == 'j' || $str_format[$i] == 'd') {
                $result .= $this->renderDay($id, $fieldname, $str_script, $current, $str_format[$i], $obligatory, $weekdayFormat);
            } /* month input box */ elseif ($str_format[$i] == 'm' || $str_format[$i] == 'n' || $str_format[$i] == 'M' || $str_format[$i] == 'F') {
                $result .= $this->renderMonth($id, $fieldname, $str_script, $current, $str_format[$i], $obligatory);
            } /* other characters */ else {
                $result .= $str_format[$i];
            }
        }

        if (!$this->hasFlag(self::AF_DATE_NO_CALENDAR) && !$this->m_yeardropdown && !$this->m_simplemode && $mode != 'list') {
            $mondayFirst = 'false';
            if (is_bool(Tools::atktext('date_monday_first'))) {
                $mondayFirst = Tools::atktext('date_monday_first') === true ? 'true' : $mondayFirst;
            }
            $result .= ' <input '.$this->getCSSClassAttribute(array(
                    'button',
                    'atkbutton',
                    'btn',
                    'btn-default',
                    'form-control',
                )).' type="reset" value="..." onclick="return showCalendar(\''.$id.'\', \''.$id.'[year]\', \'y-mm-dd\', true, '.$mondayFirst.');">';
        }

        if (!$this->m_simplemode) {
            $result .= '</div>'; // form-inline
        }

        return $result;
    }

    /**
     * Check the given $current date and return a current date that fits in the allowed range.
     *
     * @param int $current time
     * @param int $minimum
     * @param int $maximum
     * @param string $mode The mode
     *
     * @return int time
     */
    public function getValidCurrentDate($current, $minimum, $maximum, $mode)
    {
        if ($current === null && (!$this->hasFlag(self::AF_OBLIGATORY) || $mode == 'search' || $this->hasFlag(self::AF_DATE_DEFAULT_EMPTY))
        ) {
        } elseif (!empty($current) && !empty($minimum) && $current < $minimum) {
            $current = $minimum;
        } elseif (!empty($current) && !empty($maximum) && $current > $maximum) {
            $current = $maximum;
        } elseif (empty($current) && !empty($minimum) && time() < $minimum) {
            $current = $minimum;
        } elseif (empty($current) && !empty($maximum) && time() > $maximum) {
            $current = $maximum;
        } elseif (empty($current)) {
            $current = time();
        }

        return $current;
    }

    /**
     * Registers the scripts and styles for the date attribute. Can be used
     * to load the scripts beforehand from another location.
     *
     * @param bool $useCalendar use calendar widget? (defaults to true)
     */
    public static function registerScriptsAndStyles($useCalendar = true)
    {
        $page = Page::getInstance();

        // make sure we register the script code with translations for the months etc. only once!
        if (!self::$s_baseScriptsRegistered) {
            self::$s_baseScriptsRegistered = true;

            $m_months_short = array(
                1 => 'jan',
                'feb',
                'mar',
                'apr',
                'may',
                'jun',
                'jul',
                'aug',
                'sep',
                'oct',
                'nov',
                'dec',
            );
            $m_months_long = array(
                1 => 'january',
                'february',
                'march',
                'april',
                'may',
                'june',
                'july',
                'august',
                'september',
                'october',
                'november',
                'december',
            );
            $m_weekdays_long = array(
                'sunday',
                'monday',
                'tuesday',
                'wednesday',
                'thursday',
                'friday',
                'saturday',
            );
            $m_weekdays_short = array(
                'sun',
                'mon',
                'tue',
                'wed',
                'thu',
                'fri',
                'sat',
            );

            foreach ($m_months_short as &$m) {
                $m = Tools::atktext($m, 'atk');
            }
            foreach ($m_months_long as &$m) {
                $m = Tools::atktext($m, 'atk');
            }
            foreach ($m_weekdays_long as &$m) {
                $m = Tools::atktext($m, 'atk');
            }
            foreach ($m_weekdays_short as &$m) {
                $m = Tools::atktext($m, 'atk');
            }

            $page->register_scriptcode('
          var m_months_long    = Array("'.implode('","', $m_months_long).'");
          var m_months_short   = Array("'.implode('","', $m_months_short).'");
          var m_weekdays_long  = Array("'.implode('","', $m_weekdays_long).'");
          var m_weekdays_short = Array("'.implode('","', $m_weekdays_short).'");
        ', true);

            $page->register_script(Config::getGlobal('assets_url').'javascript/dateattribute.js');
        }

        if ($useCalendar) {
            $page->register_script(Config::getGlobal('assets_url').'lib/calendar/calendar.js');
            $page->register_script(Config::getGlobal('assets_url').'lib/calendar/calendar-runner.js');
            $page->register_script(Config::getGlobal('assets_url').'lib/calendar/lang/calendar-'.Config::getGlobal('language').'.js');
        }
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        $result = '';
        $field = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null;

        if (is_array($field)) {
            foreach (['year', 'month', 'day'] as $key) {
                $value = filter_var($field[$key] ?? 0, FILTER_VALIDATE_INT);
                $result .= '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'['.$key.']" '.'value="'.$fieldvalue.'">';
            }
        } else {
            $result = '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'" value="'.htmlspecialchars($field).'">';
        }

        return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to search values.
     * Searching is disabled for the date attribute, we only return a space.
     *
     * @param array $record array with 3 fields (year, month, day)
     * @param bool $extended if set to false, a simple search input is
     *                            returned for use in the searchbar of the
     *                            recordlist. If set to true, a more extended
     *                            search may be returned for the 'extended'
     *                            search page. The Attribute does not
     *                            make a difference for $extended is true, but
     *                            derived attributes may reimplement this.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     * @param DataGrid $grid
     *
     * @return string piece of HTML code
     */
    public function search($record, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        if (!$extended) {
            // plain text search, check if we didn't come from extended search (then current value is an array)
            if (isset($record[$this->fieldName()]) && is_array($record[$this->fieldName()])) {
                // TODO try to set the value
                $record[$this->fieldName()] = null;
            }

            $maxSize = $this->m_maxsize;
            $this->m_maxsize = 25; // temporary increase max size to allow from/to dates
            $result = parent::search($record, $extended, $fieldprefix);
            $this->m_maxsize = $maxSize;

            return $result;
        }

        // Set default values to null.
        if (!isset($record[$this->fieldName()]) || empty($record[$this->fieldName()])) {
            $record[$this->fieldName()] = null;
        }

        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getSearchFieldName($fieldprefix);

        $rec = isset($record[$this->fieldName()]['from']) ? array($this->fieldName() => $record[$this->fieldName()]['from']) : $record;
        $res = $this->draw($rec, $id.'_from', $name, 'atksearch_AE_'.$fieldprefix, '_AE_from', 'search');
        $rec = isset($record[$this->fieldName()]['to']) ? array($this->fieldName() => $record[$this->fieldName()]['to']) : $record;
        $res .= '&nbsp;'.Tools::atktext('until').': '.$this->draw($rec, $id.'_to', $name, 'atksearch_AE_'.$fieldprefix, '_AE_to', 'search');

        return $res;
    }

    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * @param Query $query The query object where the search condition should be placed on
     * @param string $table The name of the table in which this attribute is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one of the supported modes,
     *                           as returned by this attribute's getSearchModes() method.
     * @param string $fieldname The name of the field in the database (used by atkExpressionAttribute)
     *
     * @return string The searchcondition to use.
     */
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        $db = $this->getDb();
        $searchcondition = '';

        // If we search through datagrid we got no from/to values
        // Therefore we will simulate them
        if (!is_array($value)) {
            // exact or between (two values divided by "-")
            // ex. value "d/m/yyyy", "d/m" (use current year), "m/yyyy" (from 1 to number of days in month), "yyyy" (from 1/1 to 31/12)
            // >=: one value followed by "-"
            // <=: one value preceded by "-"
            $value = trim($value);
            if (strpos($value, '-') !== false) {
                list($from, $to) = explode('-', $value);
                $value = array('from' => trim($from), 'to' => trim($to));
            } else {
                $value = array('from' => $value, 'to' => $value);
            }
            foreach ($value as $k => $v) {
                // yyyy
                if (strlen($v) == 4 && is_numeric($v)) {
                    if ($k == 'from') {
                        $v = "1/1/$v";
                    } else {
                        $v = "31/12/$v";
                    }
                }
                // m/yyyy
                if (!is_numeric($v) && substr_count($v, '/') == 1 && (strlen($v) == 6 || strlen($v) == 7)) {
                    // if we always set the day to 31, the framework somewhere modifies the query for months with less than 31 days
                    // eg. '2015-09-31' becomes '2015-10-01'
                    $parts = explode('/', $v);
                    $daysInMonth = date('t', mktime(0, 0, 0, $parts[0], 0, $parts[1]));
                    if ($k == 'from') {
                        $v = "1/$v";
                    } else {
                        $v = "$daysInMonth/$v";
                    }
                }
                // d/m
                if (substr_count($v, '/') == 1) {
                    $v .= '/'.date('Y');
                }
                $value[$k] = $v;
            }
        }

        $valueFrom = $this->fetchValue(array($this->getHtmlName() => $value['from']));
        $valueTo = $this->fetchValue(array($this->getHtmlName() => $value['to']));

        $fromval = $this->value2db(array($this->fieldName() => $valueFrom));
        $toval = $this->value2db(array($this->fieldName() => $valueTo));

        $fieldname = $fieldname ? Db::quoteIdentifier($fieldname) : Db::quoteIdentifier($table, $this->fieldName());

        switch($fromval == null) {
        case true:
            switch($toval == null) {
            case true:
                // Both null, do nothing
                return null;
            case false:
                return $query->lessthanequalCondition($fieldname, $toval);
            }
            break;
        case false:
            switch($toval == null) {
            case true:
                return $query->greaterthanequalCondition($fieldname, $fromval);
            case false:
                // Both present : reorder if needed
                if ($fromval > $toval) {
                    $tmp = $fromval;
                    $fromval = $toval;
                    $toval = $tmp;
                }
                return $query->betweenCondition($fieldname, $fromval, $toval);
            }
            break;
        }
        return null; // but you can't get there, even if you try very hard :)
    }

    /**
     * Convert date array to database value.
     *
     * @param array $rec database record with a date attribute
     *                   field $rec[{name of the date attribute}]
     *
     * @return string database value for date
     */
    public function value2db($rec)
    {
        if (!is_array($rec[$this->fieldName()])) {
            return;
        }

        $year = $rec[$this->fieldName()]['year'];
        $month = $rec[$this->fieldName()]['month'];
        $day = $rec[$this->fieldName()]['day'];

        if (empty($year) || empty($month) || empty($day)) {
            return;
        }
        if ($year == '' || $month == 0 || $day == 0) {
            return;
        } //one of the fields is left empty

        $result = $year.'-'.sprintf('%02d', $month).'-'.sprintf('%02d', $day);

        return $result;
    }

    /**
     * Convert database value to date array.
     *
     * @param array $rec database record with date field
     *
     * @return array|null array with 3 fields (year, month, day) or null
     */
    public function db2value($rec)
    {
        if (!isset($rec[$this->fieldName()]) || strlen($rec[$this->fieldName()]) == 0 || (int)substr($rec[$this->fieldName()], 0, 4) == 0) {
            return null;
        }

        return array(
            'year' => substr($rec[$this->fieldName()], 0, 4),
            'month' => substr($rec[$this->fieldName()], 5, 2),
            'day' => substr($rec[$this->fieldName()], 8, 2),
        );
    }

    /**
     * Return the HTTP post values for this attribute.
     *
     * @param array $postvars the HTTP post vars
     *
     * @return array|null with 3 fields (year, month, day) or null
     */
    public function fetchValue($postvars)
    {
        $value = parent::fetchValue($postvars);
        if (is_null($value)) {
            return null;
        }

        // edit in text mode
        if (is_array($value) && array_key_exists('date', $value)) {
            $value = $value['date'];
        }

        // array with year / month / day
        if (is_array($value)) {
            if (empty($value['year']) || empty($value['month']) || empty($value['day'])) {
                return null;
            } else {
                return $value;
            }
        } // text format
        else {
            if (!empty($value)) {
                // maybe we should use strptime in PHP >= 5.1
                $formats = [];
                $formats[] = str_replace(array('y', 'Y', 'm', 'n', 'F', 'd', 'j'), array('yyyy', 'yyyy', 'mm', 'mm', 'mm', 'dd', 'dd'),
                    $this->m_date_format_edit);
                $formats[] = str_replace(array('y', 'Y', 'm', 'n', 'F', 'd', 'j'), array('yyyy', 'yyyy', 'm', 'm', 'm', 'dd', 'dd'), $this->m_date_format_edit);
                $formats[] = str_replace(array('y', 'Y', 'm', 'n', 'F', 'd', 'j'), array('yyyy', 'yyyy', 'mm', 'mm', 'mm', 'd', 'd'),
                    $this->m_date_format_edit);
                $formats[] = str_replace(array('y', 'Y', 'm', 'n', 'F', 'd', 'j'), array('yyyy', 'yyyy', 'm', 'm', 'm', 'd', 'd'), $this->m_date_format_edit);
                $formats[] = str_replace(array('y', 'Y', 'm', 'n', 'F', 'd', 'j'), array('yy', 'yy', 'mm', 'mm', 'mm', 'dd', 'dd'), $this->m_date_format_edit);
                $formats[] = str_replace(array('y', 'Y', 'm', 'n', 'F', 'd', 'j'), array('yy', 'yy', 'm', 'm', 'm', 'dd', 'dd'), $this->m_date_format_edit);
                $formats[] = str_replace(array('y', 'Y', 'm', 'n', 'F', 'd', 'j'), array('yy', 'yy', 'mm', 'mm', 'mm', 'd', 'd'), $this->m_date_format_edit);
                $formats[] = str_replace(array('y', 'Y', 'm', 'n', 'F', 'd', 'j'), array('yy', 'yy', 'm', 'm', 'm', 'd', 'd'), $this->m_date_format_edit);
                $arr = self::parseDate($value, $formats);
                if ($arr['day'] == 0 || $arr['month'] == 0 || $arr['year'] == 0) {
                    return self::dateArray(adodb_date('Ymd', strtotime($value)));
                } else {
                    return $arr;
                }
            }
        }

        return null;
    }

    /**
     * Validate's dates.
     *
     * @param array $record Record that contains value to be validated.
     *                       Errors are saved in this record
     * @param string $mode can be either "add" or "update"
     *
     * @return array|null $record or null
     */
    public function validate(&$record, $mode)
    {
        $value = &$record[$this->fieldName()];
        $current = 0;

        /* array or no array */
        if (!is_array($value)) {
            $value = self::dateArray(adodb_date('Ymd', strtotime($value)));
        }

        /* if not obligatory and one of the fields is null then the date will be saved as null */
        if (!$this->hasFlag(self::AF_OBLIGATORY) && (empty($value['year']) || empty($value['month']) || empty($value['day']))) {
            return null;
        }

        // If one of the fields is not filled, we don't check
        if (!($value['year'] == '' || $value['month'] == 0 || $value['day'] == 0)) {
            /* currently selected date */
            if ($this->checkDateArray($value)) {
                $current = adodb_mktime(0, 0, 0, $value['month'], $value['day'], $value['year']);
            } else {
                Tools::triggerError($record, $this->fieldName(), 'error_date_invalid');

                return null;
            }
        }

        /* allright, if not obligatory, and we have come all this way, we'll bail out */
        if (!$this->hasFlag(self::AF_OBLIGATORY)) {
            return null;
        } else {
            if ($value['year'] == '' || $value['month'] == 0 || $value['day'] == 0) {
                Tools::triggerError($record, $this->fieldName(), 'error_obligatoryfield');

                return null;
            }
        }

        /* minimum date */
        $minimum = 0;
        $str_min = $this->m_date_min;
        if (strlen($str_min) == 8) {
            $date = self::dateArray($str_min);
            if ($this->checkDateArray($date)) {
                $minimum = adodb_mktime(0, 0, 0, $date['month'], $date['day'], $date['year']);
            }
        }

        /* maximum date */
        $maximum = 0;
        $str_max = $this->m_date_max;
        if (strlen($str_max) == 8) {
            $date = self::dateArray($str_max);
            if ($this->checkDateArray($date)) {
                $maximum = adodb_mktime(0, 0, 0, $date['month'], $date['day'], $date['year']);
            }
        }

        /* date < minimum */
        if (!empty($minimum) && $current < $minimum) {
            Tools::triggerError($record, $this->fieldName(), 'error_date_minimum',
                Tools::atktext('error_date_minimum').' '.$this->formatDate(adodb_getdate($minimum), $this->m_date_format_view, 0));

            return null;
        }

        /* date > maximum */
        if (!empty($maximum) && $current > $maximum) {
            Tools::triggerError($record, $this->fieldName(), 'error_date_maximum',
                Tools::atktext('error_date_maximum').' '.$this->formatDate(adodb_getdate($maximum), $this->m_date_format_view, 0));
        }
    }

    /**
     * Function display's the date.
     *
     * @param array $record array with date
     * @param string $mode
     *
     * @return string formatted date string
     */
    public function display($record, $mode)
    {
        $value = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null;
        if (!is_array($value) || empty($value['month']) || empty($value['day']) || empty($value['year'])) {
            return '';
        }
        $tmp_date = adodb_getdate(adodb_mktime(0, 0, 0, $value['month'], $value['day'], $value['year']));
        if (!empty($tmp_date)) {
            $d = $this->formatDate($tmp_date, $this->m_date_format_view, $this->hasFlag(self::AF_DATE_DISPLAY_DAY));
            if ($mode == 'list') {
                $d = str_replace(' ', '&nbsp;', $d);
            }

            return $d;
        } else {
            return '&nbsp;';
        }
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
    public function getSearchModes()
    {
        return self::getStaticSearchModes();
    }

    public static function getStaticSearchModes()
    {
        return array('between');
    }

    /**
     * Convert a String representation into an internal value.
     *
     * This implementation converts datestring to a array with day, month and
     * year separated
     *
     * @param string $stringvalue The value to parse.
     *
     * @return array Internal value for a date
     */
    public function parseStringValue($stringvalue)
    {
        $formats = array(
            'dd-mm-yyyy',
            'dd-mm-yy',
            'd-mm-yyyy',
            'dd-m-yyyy',
            'd-m-yyyy',
            'yyyy-mm-dd',
            'yyyy-mm-d',
            'yyyy-m-dd',
            'yyyy-m-d',
        );

        return self::parseDate($stringvalue, $formats);
    }

    /**
     * Parse a string to a date array.
     *
     * @param string $stringvalue The value to parse
     * @param array $formats The formats
     *
     * @return array with day, month and year of the parsed datestring
     */
    public static function parseDate($stringvalue, $formats)
    {
        //looking in which format the stringvalue match and then get the data
        foreach ($formats as $format) {
            //make vars to know te position of the d,m and y symbols
            $dayBegin = strpos($format, 'd');
            $dayLength = 0;
            while (substr($format, $dayBegin + $dayLength, 1) == 'd') {
                ++$dayLength;
            }

            $monthBegin = strpos($format, 'm');
            $monthLength = 0;
            while (substr($format, $monthBegin + $monthLength, 1) == 'm') {
                ++$monthLength;
            }

            $yearBegin = strpos($format, 'y');
            $yearLength = 0;
            while (substr($format, $yearBegin + $yearLength, 1) == 'y') {
                ++$yearLength;
            }

            //analyze the formate and make a regular expression
            $replaces = [];
            $replaces[$dayBegin] = array('[0-9]{'.$dayLength.'}', $dayLength);
            $replaces[$monthBegin] = array('[0-9]{'.$monthLength.'}', $monthLength);
            $replaces[$yearBegin] = array('[0-9]{'.$yearLength.'}', $yearLength);

            ksort($replaces);

            $regexpr = str_replace('-', ' ', $format);
            $marge = 0; //this is the marge that the new string greater is than the old one
            foreach ($replaces as $begin => $replace) {
                $newpart = $replace[0];
                $length = $replace[1];
                $newbegin = $begin + $marge;

                $regexpr = substr($regexpr, 0, $newbegin).$newpart.substr($regexpr, $newbegin + $length);

                $marge = strlen($regexpr) - strlen($format);
            }

            $regexpr = "^$regexpr$";

            $valueSeparators = array('-', '/', "\.", '\\\\', 'a');

            //if the value has the format given by regexpr.
            //also try to replace - by "/","." or "\""
            foreach ($valueSeparators as $valueSeparator) {
                $expr = str_replace(' ', $valueSeparator, $regexpr);
                if (preg_match('/'.addcslashes($expr, '/').'/', $stringvalue)) {
                    $day = substr($stringvalue, $dayBegin, $dayLength);
                    $month = substr($stringvalue, $monthBegin, $monthLength);
                    $year = substr($stringvalue, $yearBegin, $yearLength);

                    if ($month > 12 && $day <= 12) {
                        $month += $day;
                        $day = $month - $day;
                        $month -= $day;
                    }
                    if (strlen($year) == 2) {
                        $year = '20'.$year;
                    }

                    return array('day' => $day, 'month' => $month, 'year' => $year);
                }
            }
        }

        return array('day' => 0, 'month' => 0, 'year' => 0);
    }

    /**
     * Setter for max years, this specifies the maximum amount of years in the dropdown
     * if the amount is more than specified in the max years the years field is shown
     * as a normal textbox instead of a dropdown.
     *
     * @param int $maxyears The maximum amount of years for the years dropdown
     *
     * @return bool Wether or not we succeed in setting the variable
     */
    public function setMaxYears($maxyears)
    {
        if (is_numeric($maxyears)) {
            $this->m_maxyears = (int)$maxyears;
        } else {
            return false;
        }

        return true;
    }

    /**
     * Getter for max years, this specifies the maximum amount of years in the dropdown
     * if the amount is more than specified in the max years the years field is shown
     * as a normal textbox instead of a dropdown.
     *
     * @return int The maximum years for the dropdown
     */
    public function getMaxYears()
    {
        return $this->m_maxyears;
    }

    /**
     * Setter to enable simplemode of the atkDateAttribute
     * In simplemode only the dropdowns are visible and no javascript is used to update these dropdowns
     * The date is only validated by saving the form.
     *
     * @param bool $simplemode
     */
    public function setSimpleMode($simplemode)
    {
        $this->m_simplemode = (bool)$simplemode;
        $this->addFlag(self::AF_DATE_EDIT_NO_DAY | self::AF_DATE_NO_CALENDAR | self::AF_DATE_EMPTYFIELD);
    }

    /**
     * Are we in simplemode.
     *
     * @return bool
     */
    public function getSimpleMode()
    {
        return $this->m_simplemode;
    }

    /**
     * Set year sorting.
     *
     * @param bool $sorting
     */
    public function setYearSorting($sorting)
    {
        if ($sorting == self::SORT_YEAR_ASC || $sorting == self::SORT_YEAR_DESC) {
            $this->m_year_sorting = $sorting;
        }
    }

    /**
     * Get year sorting.
     *
     * @return bool
     */
    public function getYearSorting()
    {
        return $this->m_year_sorting;
    }
}
