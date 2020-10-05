<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;

/**
 * The atkTimeAttribute class represents an attribute of a node
 * that has a selectbox to select from predefined time values.
 *
 * @author Wim Kosten <wim@ibuildings.nl>
 */
class TimeAttribute extends Attribute
{
    /**
     * Flags for atkTimeAttribute.
     */
    const AF_TIME_SECONDS = 33554432; // Display seconds after hours & minutes
    const AF_TIME_DEFAULT_EMPTY = 1073741824; // Always use the empty value on new record

    public $m_beginTime = 0;
    public $m_endTime = 23;
    public $m_steps = array('0', '30');
    public $m_default = '';

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_TIME;

    /**
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param int $beginTime Time to start with (eg 8)
     * @param int $endTime Time to end with (eg 24)
     * @param int|array $steps containing possible minute or seconds values (eg array("00","15","30","45"))
     *                             or the interval (eg 5 for 00,05,10,15, etc.)
     *                             if the flag self::AF_TIME_SECONDS is set, this is for seconds, the minutes will be range(0, 59)
     *                             else this is for the minutes and the seconds will not be displayed
     */
    public function __construct($name, $flags = 0, $beginTime = 0, $endTime = 23, $steps = ['00', '15', '30', '45'])
    {
        parent::__construct($name, $flags);

        $this->m_beginTime = $beginTime;
        $this->m_endTime = $endTime;
        if (is_array($steps)) {
            $this->m_steps = $steps;
        } else {
            $this->m_steps = $this->intervalToSteps($steps);
        }
    }

    /**
     * Convert an interval (integer) to an array with steps.
     *
     * @param int $interval The interval to convert
     *
     * @return array The array with steps.
     */
    public function intervalToSteps($interval)
    {
        $steps = [];
        for ($i = 0; $i <= 59; ++$i) {
            if ($i % $interval === 0) {
                $steps[] = $i;
            }
        }

        return $steps;
    }

    /**
     * Converts a date string (HHMISS) to an
     * array with 2 fields (hours, minutes, seconds).
     *
     * @param string $time the time string
     *
     * @return array with 3 fields (hours, minutes, seconds)
     */
    public static function timeArray($time)
    {
        if (strstr($time, ':')) {
            return array(
                'hours' => substr($time, 0, 2),
                'minutes' => substr($time, 3, 2),
                'seconds' => substr($time, 5, 2),
            );
        } else {
            return array(
                'hours' => substr($time, 0, 2),
                'minutes' => substr($time, 2, 2),
                'seconds' => substr($time, 4, 2),
            );
        }
    }

    /**
     * Display's text version of Record.
     *
     * @param array $record
     * @param string $mode
     *
     * @return string text string of $record
     */
    public function display($record, $mode)
    {
        $value = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null;
        if (!is_array($value)) {
            return '';
        }
        if ($value['hours'] === '') {
            return '';
        }
        $tmp_time = sprintf('%02d:%02d', $value['hours'], $value['minutes']);
        if ($value['seconds'] && $this->hasFlag(self::AF_TIME_SECONDS)) {
            $tmp_time .= sprintf(':%02d', $value['seconds']);
        }

        return $tmp_time;
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * For the regular Attribute, this means getting the field with the
     * same name as the attribute from the html posting.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     *
     * @return string The internal value
     */
    public function fetchValue($postvars)
    {
        $result = parent::fetchValue($postvars);

        if (!is_array($result)) {
            $result = trim($result);
            // $result could contain "date" data (when TimeAttribute is embedded in the DateTimeAttribute)
            // we extract the time information assuming the space is the separator between data and time
            if(strpos($result, ' ') !== false) {
                $result = trim(substr($result, strpos($result, ' ')));
            }

            $exploded = explode(':', $result);
            if (Tools::count($exploded) <= 1) {
                return '';
            }
            $result = [];
            $result['hours'] = $exploded[0];
            $result['minutes'] = $exploded[1];
            if ($exploded[2]) {
                $result['seconds'] = $exploded[2];
            }
        } else {
            if (strlen($result['hours']) == 0 || strlen($result['minutes']) == 0) {
                return;
            } else {
                $result = array(
                    'hours' => $result['hours'],
                    'minutes' => $result['minutes'],
                    'seconds' => $result['seconds'],
                );
            }
        }

        return $result;
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
        if ((($this->m_default == "NOW" && $this->m_ownerInstance->m_action == "add") ||
            ($this->m_default == "" && $this->hasFlag(self::AF_OBLIGATORY)) && !$this->hasFlag(self::AF_TIME_DEFAULT_EMPTY))
        ) {
            $this->m_default = date("H:i:s");
        }

        $default = explode(":", $this->m_default);

        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);
        $field = Tools::atkArrayNvl($record, $this->fieldName());
        if($field && !is_array($field)){
            $field = self::parseTime($field);
        }

        $onChangeCode = '';
        if (Tools::count($this->m_onchangecode)) {
            $this->_renderChangeHandler($fieldprefix);
            $onChangeCode = ' onChange="'.$this->getHtmlId($fieldprefix).'_onChange(this);"';
        }

        $m_hourBox = '<select id="'.$id.'[hours]" name="'.$name."[hours]\" class=\"atktimeattribute form-control select-standard\"{$onChangeCode}>";
        $m_minBox = '<select id="'.$id.'[minutes]" name="'.$name."[minutes]\" class=\"atktimeattribute form-control select-standard\"{$onChangeCode}>";
        $m_secBox = '<select id="'.$id.'[seconds]" name="'.$name."[seconds]\" class=\"atktimeattribute form-control select-standard\"{$onChangeCode}>";

        if (is_array($field)) {
            $m_defHour = $field['hours'];
            $m_defMin = $field['minutes'];
            $m_defSec = $field['seconds'];
        } else {
            $m_defHour = $default[0];
            $m_defMin = isset($default[1])?$default[1]:null;
            $m_defSec = isset($default[2])?$default[2]:null;
        }

        Tools::atkdebug("defhour=$m_defHour   defmin=$m_defMin");
        // generate hour dropdown
        if (!$this->hasFlag(self::AF_OBLIGATORY) || $this->hasFlag(self::AF_TIME_DEFAULT_EMPTY)) {
            $m_hourBox .= '<option value=""'.($m_defHour === '' ? ' selected' : '').'></option>';
        }
        for ($i = $this->m_beginTime; $i <= $this->m_endTime; ++$i) {
            if ($m_defHour !== '' && ($i == $m_defHour)) {
                $sel = ' selected';
            } else {
                $sel = '';
            }
            $m_hourBox .= sprintf("<option value='%02d'%s>%02d</option>", $i, $sel, $i);
        }

        // generate minute dropdown
        if (!$this->hasFlag(self::AF_OBLIGATORY) || $this->hasFlag(self::AF_TIME_DEFAULT_EMPTY)) {
            $m_minBox .= '<option value=""'.($m_defMin === '' ? ' selected' : '').'></option>';
        }

        if ($this->hasFlag(self::AF_TIME_SECONDS)) {
            $minute_steps = range(00, 59);
        } else {
            $minute_steps = $this->m_steps;
        }

        for ($i = 0; $i <= Tools::count($minute_steps) - 1; ++$i) {
            if ($i != 0) {
                $prev = $minute_steps[$i - 1];
            } else {
                $prev = -1;
            }
            if ($minute_steps[$i] >= $m_defMin && $prev < $m_defMin && ($m_defMin != '')) {
                $sel = ' selected';
            } else {
                $sel = '';
            }

            $m_minBox .= sprintf("<option value='%02d'%s>%02d</option>", $minute_steps[$i], $sel, $minute_steps[$i]);
        }

        // generate second dropdown
        if (!$this->hasFlag(self::AF_OBLIGATORY) || $this->hasFlag(self::AF_TIME_DEFAULT_EMPTY)) {
            $m_secBox .= '<option value""'.($m_defSec === '' ? ' selected' : '').'></option>';
        }
        for ($i = 0; $i <= Tools::count($this->m_steps) - 1; ++$i) {
            if ($i != 0) {
                $prev = $this->m_steps[$i - 1];
            } else {
                $prev = -1;
            }
            if ($this->m_steps[$i] >= $m_defSec && $prev < $m_defSec && ($m_defSec != '')) {
                $sel = ' selected';
            } else {
                $sel = '';
            }

            $m_secBox .= sprintf("<option value='%02d' %s>%02d</option>", $this->m_steps[$i], $sel, $this->m_steps[$i]);
        }

        // close dropdown structures
        $m_hourBox .= '</select>';
        $m_minBox .= '</select>';
        if ($this->hasFlag(self::AF_TIME_SECONDS)) {
            $m_secBox .= '</select>';
            $m_secBox = ':'.$m_secBox;
        } else {
            $m_secBox = '<input type="hidden" id="'.$fieldprefix.$this->fieldName().'[seconds]" name="'.$fieldprefix.$this->fieldName()."[seconds]\" value=\"00\">";
        }

        // assemble display version
        $timeedit = $m_hourBox.':'.$m_minBox.$m_secBox;

        return '<div class="TimeAttribute form-inline">'.$timeedit.'</div>';
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * @param array $rec The record that holds this attribute's value.
     *
     * @return string The database compatible value
     */
    public function value2db($rec)
    {
        $hours = $rec[$this->fieldName()]['hours'];
        $minutes = $rec[$this->fieldName()]['minutes'];
        $seconds = $rec[$this->fieldName()]['seconds'];

        if ($hours == '' || $minutes == '' || ($this->hasFlag(self::AF_TIME_SECONDS) && $seconds == '')) {
            return;
        }

        $result = sprintf('%02d', $hours).':'.sprintf('%02d', $minutes).':'.sprintf('%02d', $seconds);

        return $result;
    }

    /**
     * Convert database value to time array.
     *
     * @param array $rec database record with date field
     *
     * @return array with 3 fields (hours:minutes:seconds)
     */
    public function db2value($rec)
    {
        if (!isset($rec[$this->fieldName()]) || strlen($rec[$this->fieldName()]) == 0) {
            $retval = null;
        } else {
            $retval = array(
                'hours' => substr($rec[$this->fieldName()], 0, 2),
                'minutes' => substr($rec[$this->fieldName()], 3, 2),
                'seconds' => substr($rec[$this->fieldName()], 6, 2),
            );
        }

        return $retval;
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * @return array List of supported searchmodes
     */
    public function getSearchModes()
    {
        // exact match and substring search should be supported by any database.
        // (the LIKE function is ANSI standard SQL, and both substring and wildcard
        // searches can be implemented using LIKE)
        // Possible values
        //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
        return array('exact');
    }

    /**
     * Checks if a value is valid.
     *
     * @param array $rec The record that holds the value for this
     *                     attribute. If an error occurs, the error will
     *                     be stored in the 'atkerror' field of the record.
     * @param string $mode The mode for which should be validated ("add" or
     *                     "update")
     */
    public function validate(&$rec, $mode)
    {
        $value = $rec[$this->fieldName()];
        if ($this->hasFlag(self::AF_OBLIGATORY) && ($value['hours'] == -1 || $value['minutes'] == -1)) {
            Tools::triggerError($rec, $this->fieldName(), 'error_obligatoryfield');
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
        $field = $record[$this->fieldName()];
        $result = '';
        if (is_array($field)) {
            foreach (['hours', 'minutes', 'seconds'] as $key) {
                $value = filter_var($field[$key] ?? 0, FILTER_VALIDATE_INT);
                $result .= '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'['.$key.']" '.'value="'.$fieldvalue.'">';
            }
        } else {
            $result = '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'" value="'.htmlspecialchars($field).'">';
        }

        return $result;
    }

    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * @param Query $query The query object where the search condition should be placed on
     * @param string $table The name of the table in which this attribute
     *                           is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
     * @param string $fieldname
     *
     * @return QueryPart The searchcondition to use.
     */
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        // When we get $value as a substring, we autocomplete the time
        // So 9 becomes 09:00:00 and 11:15 becomes 11:15:00
        if (!is_array($value)) {
            $retval = array(
                'hours' => substr($value, 0, 2),
                'minutes' => substr($value, 3, 2),
                'seconds' => substr($value, 6, 2),
            );

            if (!$retval['seconds']) {
                $retval['seconds'] = '00';
            }
            if (!$retval['minutes']) {
                $retval['minutes'] = '00';
            }

            if (strlen($retval['hours']) == 1) {
                $retval['hours'] = '0'.$retval['hours'];
            }
            if (strlen($retval['minutes']) == 1) {
                $retval['minutes'] = '0'.$retval['minutes'];
            }
            if (strlen($retval['seconds']) == 1) {
                $retval['seconds'] = '0'.$retval['seconds'];
            }

            $value = implode(':', $retval);
        }

        return parent::getSearchCondition($query, $table, $value, $searchmode);
    }

    /**
     * Parse a timestring to an array.
     *
     * @param string $stringvalue The time to parse
     *
     * @return array array with hours, minutes and seconds
     */
    public static function parseTime($stringvalue)
    {
        //Assuming hh:mm:ss
        //Using negative substr because $stringvalue may contains date values (eg: "YYYY-MM-DD hh:mm:ss")
        $retval = array(
            'hours' => substr($stringvalue, -8, 2),
            'minutes' => substr($stringvalue, -5, 2),
            'seconds' => substr($stringvalue, -2, 2),
        );

        if (!$retval['seconds']) {
            $retval['seconds'] = '00';
        }
        if (!$retval['minutes']) {
            $retval['minutes'] = '00';
        }

        if (strlen($retval['hours']) == 1) {
            $retval['hours'] = '0'.$retval['hours'];
        }
        if (strlen($retval['minutes']) == 1) {
            $retval['minutes'] = '0'.$retval['minutes'];
        }
        if (strlen($retval['seconds']) == 1) {
            $retval['seconds'] = '0'.$retval['seconds'];
        }

        return $retval;
    }
}
