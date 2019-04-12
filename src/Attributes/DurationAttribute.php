<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;

/**
 * The DurationAttribute is an attribute for entering a length of time.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class DurationAttribute extends Attribute
{
    /** flag(s) specific for atkDurationAttribute */
    const AF_DURATION_STRING = 33554432; // date must be entered as an english date string (strtotime)
    const DURATIONFORMAT_TIME = 0;
    const DURATIONFORMAT_DECIMAL = 1;

    /* member vars * */
    public $m_resolution_min;
    public $m_maxtime_min;

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_NUMBER;

    /**
     * Default Constructor, sets up Attribute.
     *
     * @param string $name The name of this attribute
     * @param int $flags The falgs of this attribute
     * @param string $resolution
     * @param string $maxtime
     *
     * @see Attribute
     */
    public function __construct($name, $flags = 0, $resolution = '1m', $maxtime = '10h')
    {
        parent::__construct($name, $flags);
        
        $hms = substr($resolution, -1);
        $resolution = substr($resolution, 0, -1);

        if (strtoupper($hms) == 'H') {
            $factor = 60;
        } else {
            $factor = 1;
        }

        $this->m_resolution_min = $resolution * $factor;

        $hms = substr($maxtime, -1);
        $maxtime = substr($maxtime, 0, -1);
        if (strtoupper($hms) == 'H') {
            $factor = 60;
        } else {
            $factor = 1;
        }

        $this->m_maxtime_min = $maxtime * $factor;
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">).
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        // hide as a parseable string
        $record[$this->fieldName()] = self::_minutes2string($record[$this->fieldName()]);

        return parent::hide($record, $fieldprefix, $mode);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value. (hours, minutes and seconds will be a dropdownbox).
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string Piece a of HTML Code
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $id = $this->getHtmlId($fieldprefix);
        $fieldvalue = Tools::atkArrayNvl($record, $this->fieldName(), '');
        if (!$this->hasFlag(self::AF_DURATION_STRING)) {
            $curminutes = null;
            $result = '<div class="form-inline">';
            if ($this->m_maxtime_min >= 60) {
                $curhours = self::_getHourPart($fieldvalue);
                $curminutes = self::_getMinutePart($fieldvalue);
                $result .= '<select id="'.$id.'_hours" name="'.$this->getHtmlName($fieldprefix).'[hours]" class="form-control select-standard">';
                for ($h = 0; $h <= $this->m_maxtime_min / 60;) {
                    $result .= '<option value="'.$h.'" ';
                    if ($curhours == $h) {
                        $result .= 'selected';
                    }
                    $result .= '>'.$h.' '.Tools::atktext('hours', 'atk');
                    if ($this->m_resolution_min <= 60) {
                        ++$h;
                    } else {
                        $h = floor($h + $this->m_resolution_min / 60);
                    }
                }
                $result .= '</select>';
            }
            if ($this->m_maxtime_min >= 1 && $this->m_resolution_min < 60) {
                $result .= '&nbsp;<select id="'.$id.'_minutes" name="'.$this->getHtmlName($fieldprefix).'[minutes]" class="form-control select-standard">';
                for ($m = 0; $m < 60 || ($this->m_maxtime_min < 60 && $m < $this->m_maxtime_min);) {
                    $result .= '<option value="'.$m.'" ';
                    if ($curminutes == $m) {
                        $result .= 'selected';
                    }
                    $result .= '>'.$m.' '.Tools::atktext('minutes', 'atk');
                    if ($this->m_resolution_min <= 1) {
                        ++$m;
                    } else {
                        $m = $m + $this->m_resolution_min;
                    }
                }
                $result .= '</select>';
            }

            if ($this->m_postfixlabel) {
                $result .= '&nbsp;' . $this->m_postfixlabel;
            }

            $result .= '</div>';
        } else {
            $curval = ($fieldvalue > 0) ? self::_minutes2string($fieldvalue) : '';
            $result = '<input type="text" name="'.$this->getHtmlName($fieldprefix).'" value="'.$curval.'"'.($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').'>';
        }

        return $result;
    }


    public function search($atksearch, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        return ''; // currently not searchable.
    }

    /**
     * This function displays the time.
     *
     * The regular Attribute uses PHP's nl2br() and htmlspecialchars()
     * methods to prepare a value for display, unless $mode is "cvs".
     *
     * @param array $record The record that holds the value for this attribute
     * @param string $mode The display mode ("view" for viewpages, or "list"
     *                       for displaying in recordlists, "edit" for
     *                       displaying in editscreens, "add" for displaying in
     *                       add screens. "csv" for csv files. Applications can
     *                       use additional modes.
     *
     * @return string with YYYY-MM-DD
     */
    public function display($record, $mode)
    {
        return self::_minutes2string($record[$this->fieldName()]);
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
     * Convert the value into minutes.
     *
     * @param string $value
     *
     * @return int with number of minutes
     */
    public static function _string2minutes($value)
    {
        if (strpos($value, ':') === false) {
            // decimal format
            $tmp = explode('.', $value);
            if (strlen($tmp[1]) == 1) {
                $tmp[1] = $tmp[1] * 10;
            }

            return $tmp[0] * 60 + $tmp[1] * (60 / 100);
        } else {
            // hh:mm format
            $tmp = explode(':', $value);

            return $tmp[0] * 60 + $tmp[1];
        }
    }

    /**
     * Convert minutes to string.
     *
     * @param mixed $minutes
     *
     * @return string with minutes
     */
    public static function _minutes2string($minutes)
    {
        $prefix = '';
        if ($minutes < 0) {
            $prefix = '- ';
            $minutes = abs($minutes);
        }

        if (Config::getGlobal('durationformat', 0) == self::DURATIONFORMAT_DECIMAL) {
            $decimalvalue = (int)self::_getHourPart($minutes) + ((int)self::_getMinutePart($minutes) / 60);

            return $prefix.sprintf('%02.02f', $decimalvalue);
        } elseif (Config::getGlobal('durationformat', 0) == self::DURATIONFORMAT_TIME) {
            return $prefix.sprintf('%d:%02d', self::_getHourPart($minutes), self::_getMinutePart($minutes));
        }
    }

    /**
     * Get the hour part from the number of minutes.
     *
     * @param mixed $minutes
     *
     * @return string with hours
     */
    public static function _getHourPart($minutes)
    {
        if (!is_array($minutes)) {
            return floor(floatval($minutes) / 60);
        } else {
            return $minutes['hours'];
        }
    }

    /**
     * Get the minute part from the number of minutes.
     *
     * @param mixed $minutes
     *
     * @return string with minutes
     */
    public static function _getMinutePart($minutes)
    {
        if (!is_array($minutes)) {
            $minutes = floatval($minutes);
            return $minutes - (floor($minutes / 60) * 60);
        } else {
            return $minutes['minutes'];
        }
    }

    /**
     * Fetch values.
     *
     * @param array $postvars Array with values
     *
     * @return string without slashes
     */
    public function fetchValue($postvars)
    {
        $value = parent::fetchValue($postvars);
        if ($this->hasFlag(self::AF_DURATION_STRING) || !is_array($value)) {
            return self::_string2minutes($value);
        } else {
            return $value['hours'] * 60 + $value['minutes'];
        }
    }

    /**
     * Check if a record has an empty value for this attribute.
     *
     * @param array $record The record that holds this attribute's value.
     *
     * @return bool
     */
    public function isEmpty($record)
    {
        return parent::isEmpty($record) || $record[$this->fieldName()] == 0;
    }
}
