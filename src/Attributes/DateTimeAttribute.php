<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Relations\ManyToOneRelation;

/**
 * The DateTimeAttribute class can be used for date and time entry.
 * It corresponds to a DATETIME field in the database.
 *
 * Internally, it uses 2 attributes (m_time and m_date), and the record
 * value is represented as null (empty datetime) or an array: 
 *   ['date' => dateval, 'time' => timeval]
 * where dateval is a valid array for DateAttribute (['year', 'month', 'day'])
 * and timeval is a valid array for TimeAttribute (['hour', 'minutes', seconds'])
 *
 * @author Sandy Pleyte <sandy@achievo.org>
 * @author Samuel BF
 */
class DateTimeAttribute extends Attribute
{
    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_DATETIME;

    /**
     * Date/Time sub-attributes
     * @private DateAttribute, TimeAttribute
     */
    public $m_time = null;
    public $m_date = null;
    
    public $m_utcOffset = null;
    public $m_timezoneAttribute = null;

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     */
    public function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags); // base class constructor
        $this->m_date = new DateAttribute($name.'_AE_date', $flags);
        $this->m_time = new TimeAttribute($name.'_AE_time', $flags);
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
    public function setDateMin($min = null)
    {
        $this->m_date->setDateMin($min);
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
    public function setDateMax($max = null)
    {
        $this->m_date->setDateMax($max);
    }

    /**
     * Validate the value of this attribute.
     *
     * @param array $record The record that holds the value for this
     *                       attribute. If an error occurs, the error will
     *                       be stored in the 'atkerror' field of the record.
     * @param string $mode The mode for which should be validated ("add" or
     *                       "update")
     */
    public function validate(&$record, $mode)
    {
        $this->m_date->validate($record, $mode);
        $this->m_time->validate($record, $mode);
    }

    /**
     * Init this attribute.
     */
    public function init()
    {
        $this->m_time->m_owner = $this->m_owner;
        $this->m_date->m_owner = $this->m_owner;
        $this->m_time->m_ownerInstance = $this->m_ownerInstance;
        $this->m_date->m_ownerInstance = $this->m_ownerInstance;
    }


    /**
     * Fetch the metadata about this attrib from the table metadata, and
     * process it.
     *
     * Lengths for the edit and searchboxes, and maximum lengths are retrieved
     * from the table metadata by this method.
     *
     * @param array $metadata The table metadata from the table for this
     *                        attribute.
     */
    public function fetchMeta($metadata)
    {
        parent::fetchMeta($metadata);
        $this->m_date->setAttribSize([$this->m_maxsize, $this->m_size, 10]);
        $this->m_time->setAttribSize([$this->m_maxsize, $this->m_size, $this->m_searchsize]);
    }

    /**
     * Display's html version of Record.
     *
     * @param array $record The record
     * @param string $mode The display mode ("view" for viewpages, or "list"
     *                       for displaying in recordlists, "edit" for
     *                       displaying in editscreens, "add" for displaying in
     *                       add screens. "csv" for csv files. Applications can
     *                       use additional modes.
     *
     * @return string text string of $record
     */
    public function display($record, $mode)
    {
        $this->setDateTimeValues($record);
        $date = $this->m_date->display($record, $mode);
        $time = $this->m_time->display($record, $mode);
        if ($date != '' && $time != '') {
            return $date.(($mode == 'csv' || $mode == 'plain') ? ' ' : '&nbsp;').$time;
        } else {
            return '';
        }
    }

    /**
     * Set _date and _time values in $record for processing by corresponding attributes
     *
     * $record[$fieldName] is expected to be a valid internal value (array or null)
     *
     * @param &array $record to append '_time' and '_value' to,
     *                       $record[$fieldName] can be a string, an array ['date', 'value'] or null.
     */
    private function setDateTimeValues(&$record)
    {
        $fieldName = $this->fieldName();
        if (is_null($record[$fieldName])) {
            $record[$fieldName.'_AE_time'] = $record[$fieldName.'_AE_date'] = null;
            return;
        }
        $record[$fieldName.'_AE_date'] = $record[$fieldName]['date'];
        $record[$fieldName.'_AE_time'] = $record[$fieldName]['time'];
        return;
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
     * @return null|array the internal value
     */
    public function fetchValue($postvars)
    {
        $postvars[$this->fieldName().'_AE_date'] = $postvars[$this->fieldName()]['date'] ?? null;
        $postvars[$this->fieldName().'_AE_time'] = $postvars[$this->fieldName()]['time'] ?? null;
        $date = $this->m_date->fetchValue($postvars);
        if ($date == null) {
            return null;
        }

        $time = $this->m_time->fetchValue($postvars);
        if ($time == null) {
            $time = ['hours' => '00', 'minutes' => '00', 'seconds' => '00'];
        }

        return ['date' => $date, 'time' => $time];
    }

    public function addDependency($callback)
    {
        parent::addDependency($callback);
        $this->m_date->addDependency($callback);
        $this->m_time->addDependency($callback);

        return $this;
    }

    public function addOnChangeHandler($jscode)
    {
        $this->m_date->addOnChangeHandler($jscode);
        $this->m_time->addOnChangeHandler($jscode);

        return $this;
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
        $this->setDateTimeValues($record);
        $dateEdit = $this->m_date->edit($record, $fieldprefix, $mode);
        $timeEdit = $this->m_time->edit($record, $fieldprefix, $mode);

        return '<div class="DateTimeAttribute">'.$dateEdit.'<span> - </span>'.$timeEdit.'</div>';
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * @param null|array $rec The record that holds this attribute's value.
     *
     * @return string The database compatible value
     */
    public function value2db($rec)
    {
        if (empty($rec[$this->fieldName()])) {
            return null;
        }
        $rec[$this->fieldName()] = $this->toUTC($rec[$this->fieldName()], $rec);

        $this->setDateTimeValues($rec);
        $date = $this->m_date->value2db($rec);
        $time = $this->m_time->value2db($rec);

        if ($date == null || $time == null) {
            return null;
        }
        return $date.' '.$time;
    }

    /**
     * Convert database value to datetime array.
     *
     * @param array $rec database record with date field
     *
     * @return mixed array with 3 fields (hours:minutes:seconds)
     */
    public function db2value($rec)
    {
        if (!isset($rec[$this->fieldName()]) or is_null($rec[$this->fieldName()] or $rec[$this->fieldName()] == '0000-00-00 00:00:00')) {
            return null;
        }
        list($rec[$this->fieldName().'_AE_date'], $rec[$this->fieldName().'_AE_time']) = explode(' ', $rec[$this->fieldName()]);
        $value = ['date' => $this->m_date->db2value($rec), 'time' => $this->m_time->db2value($rec)];
        if (is_null($value['date'])) {
            return null;
        }
        if (is_null($value['time'])) {
            $value['time'] = ['hours' => '00', 'minutes' => '00', 'seconds' => '00'];
        }
        return $this->fromUTC($value, $rec);
    }

    /**
     * Returns a piece of html code that can be used to get search terms input
     * from the user.
     *
     * @param array $atksearch Array with values from POST request
     * @param bool $extended if set to false, a simple search input is
     *                            returned for use in the searchbar of the
     *                            recordlist. If set to true, a more extended
     *                            search may be returned for the 'extended'
     *                            search page. The Attribute does not
     *                            make a difference for $extended is true, but
     *                            derived attributes may reimplement this.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     * @param DataGrid $grid
     * @return string A piece of html-code
     */
    public function search($atksearch, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        $record[$this->fieldName().'_AE_date'] = $record[$this->fieldName()]['date'];
        return $this->m_date->search($record, $extended, $fieldprefix);
    }

    /**
     * Creates a searchcondition for the field
     *
     * We use m_date getSearchCondition, but overriding fieldName
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
        return $this->m_date->getSearchCondition($query, $table, $value['date'], $searchmode, $this->fieldName());
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
        $record[$this->fieldName().'_AE_date'] = $record[$this->fieldName()]['date'];
        $record[$this->fieldName().'_AE_time'] = $record[$this->fieldName()]['time'];
        return $this->m_date->hide($record, $fieldprefix, $mode).$this->m_time->hide($record, $fieldprefix, $mode);
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * @return array List of supported searchmodes
     */
    public function getSearchModes()
    {
        return $this->m_date->getSearchModes();
    }

    /**
     * Sets the timezone attribute. This can also be a timezone
     * attribute retrieved from a atkManyToOneRelation. If so then please
     * use the dot notation.
     *
     * @param string $attrName attribute name
     */
    public function setTimezoneAttribute($attrName)
    {
        $this->m_timezoneAttribute = $attrName;
    }

    /**
     * Returns the timezone attribute name.
     *
     * @return string timezone attribute name
     */
    public function getTimezoneAttribute()
    {
        return $this->m_timezoneAttribute;
    }

    /**
     * Sets the UTF offset in seconds.
     *
     * @param int $offset UTC offset in seconds
     */
    public function setUTCOffset($offset)
    {
        $this->m_utcOffset = $offset;
    }

    /**
     * Resets the UTC offset.
     */
    public function resetUTCOffset()
    {
        $this->m_utcOffset = null;
    }

    /**
     * Returns the UTC offset if set.
     *
     * @return int UTC offset in seconds if set.
     */
    public function getUTCOffset()
    {
        return $this->m_utcOffset;
    }

    /**
     * Returns the UTC offset in seconds. If the UTC offset is set explicitly
     * using the setUTCOffset method this offset is returned. Else if a timezone
     * attribute is set the offset is determined by looking at the timezone
     * using the given timezone attribute. If no offset and no attribute are set
     * an offset of 0 is returned.
     *
     * @param array $record record
     * @param string $stamp timestamp
     *
     * @return int UTC offset in seconds
     */
    public function _getUTCOffset(&$record, $stamp = null)
    {
        if ($this->m_utcOffset !== null) {
            return $this->m_utcOffset;
        }
        if ($this->m_timezoneAttribute === null) {
            return 0;
        }
        $parts = explode('.', $this->m_timezoneAttribute);
        $node = $this->getOwnerInstance();

        while (Tools::count($parts) > 0) {
            $part = array_shift($parts);
            $attr = $node->getAttribute($part);

            // relation, prepare for next iteration
            if ($attr instanceof ManyToOneRelation) {
                if (Tools::count($parts) > 0 && !isset($record[$part][$parts[0]])) {
                    /** @var ManyToOneRelation $attr */
                    $attr->populate($record, array($parts[0]));
                }

                $record = $record[$attr->fieldName()];
                $node = $attr->m_destInstance;
            } // timezone attribute, calculate and return offset
            else {
                if ($attr instanceof TimezoneAttribute) {
                    /** @var TimezoneAttribute $attr */
                    return $attr->getUTCOffset($record[$attr->fieldName()], $stamp);
                } // assume the attribute in question already has the offset saved in seconds
                else {
                    return (int)$record[$attr->fieldName()];
                }
            }
        }

        Tools::atkdebug('WARNING: could not determine UTC offset for atkDateTimeAttribute "'.$this->fieldName().'"!');

        return 0;
    }

    /**
     * Converts a date array to a timestamp
     *
     * @param array $internalValue (we expect a valid internal value, as described in the intro of this document)
     *
     * @return int Timestamp
     */
    public static function toTimestamp(array $internalValue) : int
    {
        $date = $internalValue['date'];
        $time = $internalValue['time'] ?? ['hours' => 0, 'minutes' => 0, 'seconds' => 0];
        return mktime($time['hours'] ?? 0, $time['minutes'] ?? 0, $time['seconds'] ?? 0,
            $internalValue['date']['month'], $internalValue['date']['day'], $internalValue['date']['year']);
    }

    /**
     * Converts a timestamp to a valid internal value for date/time :
     * array ['date' => ['year', 'month', 'day'], 'time' => ['hours', 'minutes', 'seconds']]
     *
     * @param int $stamp UNIX timestamp
     *
     * @return array
     */
    public static function fromTimestamp(int $stamp) : array
    {
        return [
            'date' => [
                'year' => date('Y', $stamp),
                'month' => date('m', $stamp),
                'day' => date('d', $stamp)
            ],
            'time' => [
                'hours' => date('H', $stamp),
                'minutes' => date('i', $stamp),
                'seconds' => date('s', $stamp)
            ]
        ];
    }

    /**
     * Convert the given date/time array with offset to a UTC date/time array.
     *
     * @param mixed $value initial date/time array
     * @param array $record record
     *
     * @return null|array date/time array without offset
     */
    public function toUTC($value, &$record)
    {
        if (is_null($value)) {
            return null;
        }
        $stamp = self::toTimestamp($value);
        $offset = $this->_getUTCOffset($record, $stamp);
        $stamp = $stamp - $offset;
        return self::fromTimestamp($stamp);
    }

    /**
     * Convert the UTC date/time array a date/time array without offset.
     *
     * @param mixed $value UNIX timestamp or ATK date/time array
     * @param array $record record
     *
     * @return null|array date/time array without offset
     */
    public function fromUTC($value, &$record)
    {
        $stamp = self::toTimestamp($value);
        $offset = $this->_getUTCOffset($record, $stamp);
        $stamp = $stamp + $offset;
        return self::fromTimestamp($stamp);
    }

    /**
     * If a timezone attribute is set, make sure
     * it's always loaded.
     */
    public function postInit()
    {
        parent::postInit();
        if ($this->m_timezoneAttribute !== null) {
            $node = $this->getOwnerInstance();
            $parts = explode('.', $this->m_timezoneAttribute);
            $attr = $node->getAttribute($parts[0]);
            $attr->addFlag(self::AF_FORCE_LOAD);
        }

        $this->m_date->postInit();
        $this->m_time->postInit();
    }
}
