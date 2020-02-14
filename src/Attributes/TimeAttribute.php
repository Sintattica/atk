<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
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

    private $m_beginTime = 0;
    private $m_endTime = 23;

    /**
     * Granularity of proposed time (in seconds)
     *
     * @var int
     */
    private $m_steps = 900; // 15 minutes

    /**
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param int $beginTime Time to start with (eg 8)
     * @param int $endTime Time to end with (eg 24)
     * @param int $steps interval between successive minutes or seconds (depending on AF_TIME_SECONDS being set or not)
     */
    public function __construct($name, $flags = 0, $beginTime = 0, $endTime = 23, $steps = 15)
    {
        parent::__construct($name, $flags);

        $this->m_beginTime = $beginTime;
        $this->m_endTime = $endTime;
        // Computing the minimal distance between 2 steps when steps is an array (old ATK where you would put steps list)
        if (is_array($steps)) {
            $minimalInterval = 60;
            for ($i = 0, $count = count($steps); $i < $count - 2; $i++) {
                $minimalInterval = min($minimalInterval, $steps[$i+1]-$steps[$i]);
            }
            $steps = $minimalInterval;
        }
        $this->m_steps = $this->hasFlag(self::AF_TIME_SECONDS) ? $steps : 60 * $steps;
    }

    /**
     * Converts a date string (HH:II:SS or HHIISS) to a valid
     * array with 3 fields (hours, minutes, seconds).
     *
     * @param string $time the time string
     *
     * @return null|array with 3 fields (hours, minutes, seconds)
     */
    public static function timeArray($time)
    {
        if (empty($time)) {
            return null;
        }
        if (is_array($time)) {
            $time = array_values($time);
        } elseif (is_string($time) and strstr($time, ':') !== false) {
            $time = explode(':', $time);
        } elseif (strlen($time) == 6) {
            $time = [substr($time, 0, 2), substr($time, 2, 2), substr($time, 5, 2)];
        }
        return [
            'hours' => sprintf('%02d', max(0, min(23, $time[0] ?? 0))),
            'minutes' => sprintf('%02d', max(0, min(59, $time[1] ?? 0))),
            'seconds' => sprintf('%02d', max(0, min(59, $time[2] ?? 0))),
        ];
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
        if (!$this->hasFlag(self::AF_TIME_SECONDS)) {
            unset($value['seconds']);
        }
        return implode(':', $value);
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
        return self::timeArray(parent::fetchValue($postvars));
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
        if ((($this->m_initialValue == "NOW" && $mode == "add") ||
            ($this->m_initialValue == "" && $this->hasFlag(self::AF_OBLIGATORY)) && !$this->hasFlag(self::AF_TIME_DEFAULT_EMPTY))
        ) {
            // Get the last time clock matched with wanted 'steps' :
            $ts = date('H')*3600+date('i')*60+date('s');
            $ts = $ts - ($ts % $this->m_steps);
            $h = sprintf('%02d', floor($ts/3600));
            $i = sprintf('%02d', floor(($ts-3600*$h)/60));
            $s = sprintf('%02d', $ts-3600*$h-60*$s);
            $defaultValue = $h.':'.$i.':'.$s;
        } else {
            $defaultValue = '';
        }


        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);
        $field = Tools::atkArrayNvl($record, $this->fieldName());
        $value = is_array($field) ? implode(':', $field) : (empty($field) ? $defaultValue : $field);
        // Limiting value to HH:ii when seconds are not used
        if (!$this->hasFlag(self::AF_TIME_SECONDS)) {
            $value = substr($value, 0, 5);
        }

        $onChangeCode = $begin = $end = $required = '';
        if (Tools::count($this->m_onchangecode)) {
            $this->_renderChangeHandler($fieldprefix);
            $onChangeCode = ' onChange="'.$this->getHtmlId($fieldprefix).'_onChange(this);"';
        }
        if ($this->m_beginTime > 0) {
            $begin = ' min="'.((int) $this->m_beginTime).':00:00"';
        }
        if ($this->m_endTime <= 23) {
            $end = ' max="'.((int) $this->m_endTime).':00:00"';
        }
        if ($this->hasFlag(self::AF_OBLIGATORY))
        {
            $required = ' required';
        }

        return '<input type="time" name="'.$name.'" value="'.htmlspecialchars($value).'" step="'.((int)$this->m_steps).'" class="atktimeattribute form-control"'.$onChangecode.$begin.$end.$required.' />';
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
        if (is_null($rec[$this->fieldName()])) {
            return null;
        }
        return implode(':', $rec[$this->fieldName()]);
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
        return self::timeArray($rec[$this->fieldName()]);
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * @return array List of supported searchmodes
     */
    public function getSearchModes()
    {
        return array('exact');
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return string The 'generic' type of the database field for this
     *                attribute.
     */
    public function dbFieldType()
    {
        return 'time';
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
        $value = self::timeArray(isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null);
        $txtValue = is_null($value) ? '' : implode(':', $value);

        return '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'" value="'.htmlspecialchars($txtValue).'"/>';
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
     * @return string The searchcondition to use.
     */
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        // When we get $value as a substring, we autocomplete the time
        // So 9 becomes 09:00:00 and 11:15 becomes 11:15:00
        $value = implode(':', self::timeArray($value));

        return parent::getSearchCondition($query, $table, $value, $searchmode);
    }
}
