<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
/**
 * The DateAttribute class offers a date widget for date fields.
 *
 * Internally, the value is stored as an array with 'year', 'month' and 'day' keys,
 * and is null if date is not set.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @author Tom Schenkenberg <tom@achievo.org>
 * @author Samuel BF
 */
class DateAttribute extends Attribute
{
    /** flag(s) specific for atkDateAttribute */
    const AF_DATE_STRING = 33554432; // date must be entered according to date_format_edit format
    const AF_DATE_DEFAULT_EMPTY = 67108864; // Set initially empty date event when the attribute is mandatory
    // Deprecated flags :
    const AF_DATE_EMPTYFIELD = 0; // Fields have one empty option
    const AF_DATE_NO_CALENDAR = 0; // Do not append the popup calendar.
    const AF_DATE_DISPLAY_DAY = 0; // Show the day of the week in the display
    const AF_DATE_EDIT_NO_DAY = 0; // Don't display the day of the week in edit mode

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_DATE;

    /**
     * Possible values for sorting the year dropdown.
     *
     * @Deprecated
     */
    const SORT_YEAR_ASC = 1;
    const SORT_YEAR_DESC = 2;
    
   /**
    * Minimum and maximum values for this field
    *
    * @var array ['year', 'month', 'day']
    */
    protected $m_date_min = null;
    protected $m_date_max = null;

    /**
    .* Formats for date on view and edit (edit only used with AF_DATE_STRING)
     *
     * @var string
     */
    protected $m_date_format_view;
    protected $m_date_format_edit;

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
    public function __construct($name, $flags = 0, $format_edit = '', $format_view = '', $min = null, $max = null)
    {
        /* base class constructor */
        parent::__construct($name, $flags);

        /* edit and display date format */
        $this->setFormatView($format_view);
        $this->setFormatEdit($format_edit);

        /* max / min date */
        $this->setDateMin($min);
        $this->setDateMax($max);
    }

    public function postInit()
    {
        parent::postInit();
        if ($this->hasFlag(static::AF_OBLIGATORY) && !$this->hasFlag(static::AF_DATE_DEFAULT_EMPTY)) {
            $this->setInitialValue($this->dateArray('now'));
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

        if ($this->hasFlag(static::AF_DATE_STRING) && empty($format_edit)) {
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
     * Set the maximum date that may be select (null means unlimited).
     *
     * @param mixed $max The maximum date that may be selected.
     */
    public function setDateMax($max = null)
    {
        $this->m_date_max = $this->dateArray($max);
    }

    /**
     * Set the minimum date that may be select (null means unlimited).
     *
     * @param mixed $min The minimum date that may be selected.
     */
    public function setDateMin($min = null)
    {
        $this->m_date_max = $this->dateArray($min);
    }

    /**
     * Return a valid internal state for date object
     *
     * @param mixed $input as an array, a string (parsed by DateTime::construct) or a timestamp (int)
     * @param string $format to parse date if it's a string. If not specified or if it fails,
     *                       we'll try with DateTime::__construct
     *
     * @return null|array
     */
    public function dateArray($input, $format = null)
    {
        if (empty($input)) {
            return null;
        }
        $dateObject = null;
        try {
            // First test if it's an array
            if (is_array($input)) {
                if (isset($input['year']) and isset($input['month']) and isset($input['day'])) {
                    $dateObject = new \DateTime($input['year'].'-'.$input['month'].'-'.$input['day']);
                } else {
                    return null;
                }
            // Then test for integers (as strings or as int) and parse them as timestamps
            } elseif (is_int($input) or !preg_match('/[^0-9]/', $input)) {
                $dateObject = new \DateTime('@'.$input);
            // Then try to get the date as specified by the format
            } elseif (!empty($format)) {
                $dateObject = \DateTime::createFromFormat($format, $input);
                if ($dateObject === false or $dateObject->getLastErrors()['error_count'] >= 1) {
                    $dateObject = null;
                }
            }
            // Then, if other options failed, try as a generic string (will match "now", "yesterday", "2014-05-03" ...)
            // if the string is not matched, it will raise an exception that we catch few lines later
            // Formats understood by PHP : https://www.php.net/manual/en/datetime.formats.date.php
            // note : aa/bb/cccc will be interpreted the US way : aa = month, bb = day, cccc = year.
            if (is_null($dateObject)) {
                $dateObject = new \DateTime($input);
            }
        } catch (\Exception $e) {
            return null;
        }
        return [
            'year' => $dateObject->format('Y'),
            'month' => $dateObject->format('m'),
            'day' => $dateObject->format('d')
        ];
    }

    /**
     * Return date from internal format (array) as 'yyyy-mm-dd'
     *
     * @param array $date as built from $this->dateArray (but not null !)
     *
     * @return string 'YYYY-MM-DD'
     */
    public static function dateString(array $date) : string
    {
        return implode('-', $date);
    }

    /**
     * format the internal value according to specified format, translated with ATK
     *
     * @param array|null $value as specified in the beginning of the document
     * @param string $format to display the date
     *
     * @return string
     */
    public static function format($value, string $format) : string
    {
        if (empty($value)) {
            return '';
        }
        return Tools::atkFormatDate((new \DateTime(static::dateString($value)))->format('U'), $format);
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
        return static::format($record[$this->fieldName()], $this->m_date_format_view);
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

        return $this->draw($record, $id, $name, $fieldprefix, '', $mode, $this->hasFlag(static::AF_OBLIGATORY));
    }


    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value. (Month will be a dropdownbox, year and day text fields).
     *
     * @todo We can't show a calendar when we have a year dropdown?
     * @todo The calendar doesn't use the min/max values?
     *
     * @param array $record
     * @param string $id html id
     * @param string $name html name
     * @param string $fieldprefix The fieldprefix
     * @param string $postfix
     * @param string $mode The mode ('add' or 'edit')
     * @param boolean $obligatory
     *
     * @return string Piece a of HTML Code
     */
    public function draw($record, $id, $name, $fieldprefix = '', $postfix = '', $mode = '', $obligatory = false)
    {
        $result = '<input id="'.$id.'" name="'.$name.$postfix.'" ';
        $result .= ' '.$this->getCSSClassAttribute(['form-control', 'atkdateattribute']);
        if ($obligatory) {
            $result .= ' required';
        }
        if (Tools::count($this->m_onchangecode)) {
            $this->_renderChangeHandler($fieldprefix);
            $result .= ' onChange="'.$id.'_onChange(this);"';
        }
        
        $value = $record[$this->fieldName()];
        $txtValue = '';
        if (!is_null($value)) {
            if($this->hasFlag(static::AF_DATE_STRING)) {
                $txtValue = static::format($value, $this->m_date_format_edit);
            } else {
                $txtValue = static::dateString($value);
            }
        }
        if (!empty($txtValue)) {
            $result .= ' value="'.htmlspecialchars($txtValue).'"';
        }

        if ($this->hasFlag(static::AF_DATE_STRING)) {
            $result .= ' type="text"';
        } else {
            $result .= ' type="date"';
        }
        if($placeholder = $this->getPlaceholder()){
            $result .= ' placeholder="'.htmlspecialchars($placeholder).'"';
        }

        if ($this->m_date_min !== null) {
            $result .= ' min="'.static::dateString($this->m_date_min).'"';
        }
        if ($this->m_date_max !== null) {
            $result .= ' max="'.static::dateString($this->m_date_max).'"';
        }

        $result .= ' />';

        return $result;
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
        $value = $this->dateArray(isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null);
        $txtValue = is_null($value) ? '' : static::dateString($value);

        return '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'" value="'.htmlspecialchars($txtValue).'"/>';
    }

    /**
     * Returns a piece of html code that can be used in a form to search values.
     * Searching is disabled for the date attribute, we only return a space.
     *
     * @param array $record array with 3 fields (year, month, day)
     * @param bool $extended if set to false, a simple search input is
     *                            returned for use in the searchbar of the
     *                            recordlist.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     * @param DataGrid $grid
     *
     * @return string piece of HTML code
     */
    public function search($record, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        if (!$extended) {
            // We switch value to string format for presetting in the form
            $value = $record[$this->fieldName()];
            if (is_array($value)) {
                $keys = array_keys($value);
                if (in_array('from', $keys) and in_array('to', $keys)) {
                    $record[$this->fieldName()] = "{$value['from']}-{$value['to']}";
                } elseif (in_array('from', $keys) or in_array('to', $keys)) {
                    $record[$this->fieldName()] = $value[$keys[0]];
                } else {
                    $record[$this->fieldName()] = static::dateString($record[$this->fieldName()]);
                }
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
                // We format the date to 'yyyy-mm-dd'
                // yyyy
                if (strlen($v) == 4 && is_numeric($v)) {
                    if ($k == 'from') {
                        $v = "$v-1-1";
                    } else {
                        $v = "$v-12-31";
                    }
                }
                // m/yyyy
                elseif (!is_numeric($v) && substr_count($v, '/') == 1 && (strlen($v) == 6 || strlen($v) == 7)) {
                    // if we always set the day to 31, the framework somewhere modifies the query for months with less than 31 days
                    // eg. '2015-09-31' becomes '2015-10-01'
                    $parts = explode('/', $v);
                    if ($k == 'from') {
                        $v = "{$parts[1]}-{$parts[0]}-1";
                    } else {
                        $v = (new \DateTime("{$parts[1]}-{$parts[0]}-1"))->format('Y-m-t');
                    }
                }
                // d/m
                elseif (substr_count($v, '/') == 1) {
                    $parts = explode('/', $v);
                    $v = date('Y')."-{$parts[1]}-{$parts[0]}";
                }
                // d/m/Y
                elseif (substr_count($v, '/') == 2) {
                    $parts = explode('/', $v);
                    $v = "{$parts[2]}-{$parts[1]}-{$parts[0]}";
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
        if (is_null($rec[$this->fieldName()])) {
            return null;
        }
        return static::dateString($rec[$this->fieldName()]);
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
        return $this->dateArray($rec[$this->fieldName()], 'Y-m-d');
    }

    /**
     * Return the HTTP post values for this attribute.
     *
     * @param array $postvars the HTTP post vars
     *
     * @return null|array with 3 fields (year, month, day)
     */
    public function fetchValue($postvars)
    {
        // First, let's translate months and weeks back to english
        // (value may contain such words when AF_DATE_STRING and m_date_edit_format contains 'F', 'M' or 'l')
        $localizedValue = parent::fetchValue($postvars);
        $dateWords = ['january', 'jan', 'february', 'feb', 'march', 'mar', 'april', 'apr', 'may', 'june', 'jun',
                      'july', 'jul', 'august', 'aug', 'september', 'sep', 'october', 'oct', 'november', 'nov', 'december', 'dec',
                      'monday', 'tuesday', 'wednesday', 'thursday', 'friday', 'saturday', 'sunday'];
        $localizedWords = array_map(function ($x) { return ucfirst(Tools::atktext($x)); }, $dateWords);
        $englishValue = str_ireplace($localizedWords, $dateWords, $localizedValue);
        return $this->dateArray($englishValue, $this->m_date_format_edit);
    }

    /**
     * Validate's dates.
     *
     * @param array $record Record that contains value to be validated.
     *                       Errors are saved in this record
     * @param string $mode can be either "add" or "update"
     *
     * @return null
     */
    public function validate(&$record, $mode)
    {
        // No value : don't check min/max (AF_OBLIGATORY flag got checked elsewhere)
        if (is_null($record[$this->fieldName()])) {
            return;
        }

        $value = static::dateString($record[$this->fieldName()]);
        if (!is_null($this->m_date_max) && $value > static::dateString($this->m_date_max)) {
            Tools::triggerError($record, $this->fieldName(), 'error_date_maximum');
        }
        if (!is_null($this->m_date_min) && $value < static::dateString($this->m_date_min)) {
            Tools::triggerError($record, $this->fieldName(), 'error_date_minimum');
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
        return static::getStaticSearchModes();
    }

    public static function getStaticSearchModes()
    {
        return array('between');
    }

    /**
     * Convert a String representation into an internal value.
     *
     * @deprecated : use dateArray directly
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
        return $this->dateArray($stringvalue);
    }

    /**
     * Setter to enable simplemode of the atkDateAttribute
     * In simplemode only the dropdowns are visible and no javascript is used to update these dropdowns
     * The date is only validated by saving the form.
     *
     * @DEPRECATED
     *
     * @param bool $simplemode
     */
    public function setSimpleMode($simplemode)
    {
        Tools::atkwarning('DateAttribute::setSimpleMode function is deprecated and have no effect');
    }
}
