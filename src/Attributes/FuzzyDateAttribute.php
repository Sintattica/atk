<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Db;
/**
 * The FuzzyDateAttribute class allows user selecting a date without
 * specifying the precise day or month
 *
 * Internally, the value is stored as an array with 'year', 'month' and 'day' keys,
 * where 'month' and 'day' value can be empty. In DB, values are stored as strings in
 * the form 'YYYY--", "YYYY-MM-" or "YYYY-MM-DD" (varchar types should be used).
 *
 * @author Samuel BF
 */
class FuzzyDateAttribute extends DateAttribute
{
    const AF_FUZZYDATE_MONTH_OBLIGATORY = 134217728; // to make month is mandatory (to make day also mandatory, use DateAttribute)

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_STRING;

    /**
     * constructor : calls parent constructor with AF_DATE_STRING flag
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
        parent::__construct($name, $flags | DateAttribute::AF_DATE_STRING, $format_edit, $format_view, $min, $max);
    }

    /**
     * Remove day specifiers from format string (and separators)
     *
     * @param string $format to strip day specifiers out
     *
     * @result string $format
     */
    public static function stripDayFromFormat(string $format) : string
    {
        return preg_replace('/[dDjlNSwz][ \/,-]*/', '', $format);
    }

    /**
     * Remove month specifiers from format string (and separators)
     *
     * @param string $format to strip day specifiers out
     *
     * @result string $format
     */
    public static function stripMonthFromFormat(string $format) : string
    {
        return preg_replace('/[FmMn][ \/,-]*/', '', $format);
    }

    /**
     * Tries to build a date from $string, with otional $format
     *
     * If an error is encountered, null is returned.
     *
     * @param string $string describing the date
     * @param string|null $format optional format to parse the date.
     *
     * @result \DateTime|null
     */
    protected function dateOrNull(string $string, string $format = null)
    {
        try {
            if (is_null($format)) {
                return new \DateTime($string);
            } else {
                $date = \DateTime::createFromFormat($format, $string);
                if ($date === false or $date->getLastErrors()['error_count'] > 0) {
                    return null;
                } else {
                    return $date;
                }
            }
        } catch(\Exception $e) {
            return null;
        }
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
        // First test if it's an array
        $dateObject = null;
        $dayEmpty = $monthEmpty = false;
        if (is_array($input)) {
            if (isset($input['year']) and isset($input['month']) and isset($input['day'])) {
                $dateObject = $this->dateOrNull("{$input['year']}-{$input['month']}-{$input['day']}");
            } else {
                return null;
            }
        } else {
            $input = trim($input);
        }
        // Then, try with given format
        if (is_null($dateObject) and !empty($format)) {
            $dateObject = $this->dateOrNull($input, $format);
            // If it failed, try without day
            if (is_null($dateObject)) {
                $format = static::stripDayFromFormat($format);
                $dateObject = $this->dateOrNull($input, $format);
                if (!is_null($dateObject)) {
                    $dayEmpty = true;
                // Now, let's try without month
                } elseif (!$this->hasFlag(static::AF_FUZZYDATE_MONTH_OBLIGATORY)) {
                    $format = static::stripMonthFromFormat($format);
                    $dateObject = $this->dateOrNull($input, $format);
                    if (!is_null($dateObject)) {
                        $dayEmpty = true;
                        $monthEmpty = true;
                    }
                }
            }
        }
        // Then test for integers (as strings or as int) and parse them as 'YYYY', 'YYYYMM', 'YYYYMMDD' or timestamp
        if (is_null($dateObject) and is_int($input) or !preg_match('/[^0-9]/', $input)) {
            if (strlen($input) == 4) {
                if ($this->hasFlag(static::AF_FUZZYDATE_MONTH_OBLIGATORY)) {
                    return null;
                }
                $dateObject = $this->dateOrNull($input, 'Y');
                $dayEmpty = true;
                $monthEmpty = true;
            } elseif (strlen($input) == 6) {
                $dateObject = $this->dateOrNull($input, 'Ym');
                $dayEmpty = true;
            } elseif (strlen($input) == 8) {
                $dateObject = $this->dateOrNull($input, 'Ymd');
            } else {
                $dateObject = new \DateTime('@'.$input);
            }
        }
        // Then, if other tries failed, try as a generic string (will match "now", "yesterday", "2014-05-03" ...)
        // Formats understood by PHP : https://www.php.net/manual/en/datetime.formats.date.php
        // note : aa/bb/cccc will be interpreted the US way : aa = month, bb = day, cccc = year.
        if (is_null($dateObject)) {
            $dateObject = $this->dateOrNull($input);
        }
        if (is_null($dateObject)) {
            return null;
        }
        return [
            'year' => $dateObject->format('Y'),
            'month' => $monthEmpty ? '' : $dateObject->format('m'),
            'day' => $dayEmpty ? '' : $dateObject->format('d')
        ];
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
        // Removing the day from format if absent :
        if (empty($value['day'])) {
            $value['day'] = 1;
            $format = static::stripDayFromFormat($format);
            // Also removing the month from format if absent :
            if (empty($value['month'])) {
                $value['month'] = 1;
                $format = static::stripMonthFromFormat($format);
            }
        }
        return Tools::atkFormatDate((new \DateTime(static::dateString($value)))->format('U'), $format);
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
        $value = $rec[$this->fieldName()]; // in the form YYYY-- or YYYY-MM- or YYYY-MM-DD
        if (empty($value)) {
            return null;
        }
        if (strlen($value) == 6) {
            return ['year' => substr($value, 0, 4), 'month' => '', 'day' => ''];
        }
        if (strlen($value) == 8) {
            return ['year' => substr($value, 0, 4), 'month' => substr($value, 5, 2), 'day' => ''];
        }
        if (strlen($value) == 10) {
            return ['year' => substr($value, 0, 4), 'month' => substr($value, 5, 2), 'day' => substr($value, 8, 2)];
        }
        Tools::atkwarning("FuzzyDateAttribute(".$this->fieldName().")Unable to decode database value '$value'.");
        return null;
    }

    /**
     * Return the database field type of the attribute.
     *
     * Note that the type returned is a 'generic' type. Each database
     * vendor might have his own types, therefor, the type should be
     * converted to a database specific type using $db->fieldType().
     *
     * @return string The 'generic' type of the database field for this
     *                attribute.
     */
    public function dbFieldType()
    {
        return 'string';
    }
}
