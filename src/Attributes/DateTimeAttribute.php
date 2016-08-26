<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;

/**
 * The DateTimeAttribute class can be used for date and time entry.
 * It corresponds to a DATETIME field in the database.
 *
 * @author Sandy Pleyte <sandy@achievo.org>
 */
class DateTimeAttribute extends Attribute
{
    public $m_time;
    public $m_date;
    public $m_utcOffset = null;
    public $m_timezoneAttribute = null;

    /**
     * Converts a date array to a timestamp
     * year, month, day are obligatory !!
     *
     * @param array $dateArray Date Array
     *
     * @return int Timestamp
     */
    public static function arrayToDateTime($dateArray)
    {
        $hour = 0;
        $min = 0;
        $sec = 0;
        $dateValid = true;
        $month = $day = $year = null;

        if (!empty($dateArray['hours'])) {
            $hour = $dateArray['hours'];
        }
        if (!empty($dateArray['minutes'])) {
            $min = $dateArray['minutes'];
        }
        if (!empty($dateArray['seconds'])) {
            $sec = $dateArray['seconds'];
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
        } else {
            return adodb_mktime(0, 0, 0);
        }
    }

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     */
    public function __construct($name, $flags = 0)
    {
        $default_steps = [];
        for ($i = 0; $i < 60; ++$i) {
            $default_steps[$i] = $i;
        }

        $this->m_date = new DateAttribute($name, $flags);
        $this->m_time = new TimeAttribute($name, $flags, 0, 23, $default_steps);

        parent::__construct($name, $flags); // base class constructor
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
    public function setDateMax($max = 0)
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
        //if the datetime string is not an array, make it one to make sure the
        //validation functions of atkDateAttribute and atkTimeAttribute do not
        //cripple the data.
        if (!is_array($record[$this->fieldName()])) {
            $stamp = strtotime($record[$this->fieldName()]);
            $record[$this->fieldName()] = $this->datetimeArray(date('YmdHi', $stamp));
        }

        $this->m_date->validate($record, $mode);
        $this->m_time->validate($record, $mode);
    }

    /**
     * Converts a date/time string (YYYYMMDDHHMISS) to an
     * array with 5 fields (day, month, year, hours, minutes, seconds).
     * Defaults to current date/time.
     *
     * @param string $datetime the time string
     *
     * @return array with 6 fields (day, month, year, hours, minutes, seconds)
     */
    public static function datetimeArray($datetime = null)
    {
        if ($datetime == null) {
            $datetime = date('YmdHis');
        }
        $date = substr($datetime, 0, 8);
        $time = substr($datetime, 8, 6);

        return array_merge(DateAttribute::dateArray($date), TimeAttribute::timeArray($time));
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
        $this->m_date->fetchMeta($metadata);
        $this->m_time->fetchMeta($metadata);
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
        $date = $this->m_date->display($record, $mode);
        $time = $this->m_time->display($record, $mode);
        if ($date != '' && $time != '') {
            return $date.(($mode == 'csv' || $mode == 'plain') ? ' ' : '&nbsp;').$time;
        } else {
            return '';
        }
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
        $date = $this->m_date->fetchValue($postvars);
        if ($date == null) {
            return;
        }

        $time = $this->m_time->fetchValue($postvars);
        if ($time == null) {
            $time = array('hours' => '00', 'minutes' => '00', 'seconds' => '00');
        }

        return array_merge($date, $time);
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
        $dateEdit = $this->m_date->edit($record, $fieldprefix, $mode);
        $this->m_time->m_htmlid = $this->m_date->m_htmlid;
        $timeEdit = $this->m_time->edit($record, $fieldprefix, $mode);

        return '<div class="'.$this->get_class_name().'">'.$dateEdit.'<span> - </span>'.$timeEdit.'</div>';
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
        if (is_array($rec[$this->fieldName()])) {
            $value = $rec[$this->fieldName()];
            $value = $this->toUTC($value, $rec);
            $rec[$this->fieldName()] = $value;

            $date = $this->m_date->value2db($rec);
            $time = $this->m_time->value2db($rec);

            if ($date != null && $time != null) {
                return $date.' '.$time;
            }
        } else {
            if (!empty($rec[$this->fieldName()])) {
                $stamp = strtotime($rec[$this->fieldName()]);
                $stamp = $this->toUTC($stamp, $rec);

                return date('Y-m-d H:i:s', $stamp);
            }
        }

        return;
    }

    /**
     * Convert database value to datetime array.
     *
     * @param array $rec database record with date field
     *
     * @return array with 3 fields (hours:minutes:seconds)
     */
    public function db2value($rec)
    {
        if (isset($rec[$this->fieldName()]) && $rec[$this->fieldName()] != null) {
            /*
             * @todo Fix handling of 0 and NULL db values in the date, time and datetime attributes
             * Currently the date attribute gives an empty string when parsing 0000-00-00,
             * the time attribute gives an array with all three values set to 00,
             * and the datetimeattribute gives an empty string now (previously it gave a php warning
             * because it was trying to array_merge the empty string from the date attribute with the
             * array of the time attribute).
             */
            if ($rec[$this->fieldName()] == '0000-00-00 00:00:00') {
                return '';
            }

            $datetime = explode(' ', $rec[$this->fieldName()]);

            $tmp_rec = $rec;
            $tmp_rec[$this->fieldName()] = $datetime[0];
            $result_date = $this->m_date->db2value($tmp_rec);
            if ($result_date == null) {
                return;
            }

            $tmp_rec = $rec;
            $tmp_rec[$this->fieldName()] = isset($datetime[1]) ? $datetime[1] : null;
            $result_time = $this->m_time->db2value($tmp_rec);
            if ($result_time == null) {
                $result_time = array('hours' => '00', 'minutes' => '00', 'seconds' => '00');
            }

            $value = array_merge((array)$result_date, (array)$result_time);
            $value = $this->fromUTC($value, $tmp_rec);

            return $value;
        } else {
            return;
        }
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        if ($mode == 'add' || $mode == 'update') {
            if ($this->value2db($record) == null) {
                $query->addField($this->fieldName(), 'NULL', '', '', false);
            } else {
                $db = $this->m_ownerInstance->getDb();
                if ($db->getType() != 'oci9') {
                    $query->addField($this->fieldName(), $this->value2db($record), '', '', !$this->hasFlag(self::AF_NO_QUOTES));
                } else {
                    $value = $this->value2db($record);
                    $query->addField($this->fieldName(), $value, '', '', !$this->hasFlag(self::AF_NO_QUOTES), true);
                }
            }
        } else {
            if (Config::getGlobal('database') != 'oci9') {
                $query->addField($this->fieldName(), '', $tablename, $fieldaliasprefix, !$this->hasFlag(self::AF_NO_QUOTES));
            } else {
                $query->addField($this->fieldName(), '', $tablename, $fieldaliasprefix, !$this->hasFlag(self::AF_NO_QUOTES), true);
            }
        }
    }

    /**
     * Returns a piece of html code that can be used to get search terms input
     * from the user.
     *
     * @param array $record Array with values
     * @param bool $extended if set to false, a simple search input is
     *                            returned for use in the searchbar of the
     *                            recordlist. If set to true, a more extended
     *                            search may be returned for the 'extended'
     *                            search page. The Attribute does not
     *                            make a difference for $extended is true, but
     *                            derived attributes may reimplement this.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     *
     * @return string A piece of html-code
     */
    public function search($record, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        $this->m_date->m_searchsize = 10;
        $this->m_time->m_htmlid = $this->m_date->m_htmlid;
        return $this->m_date->search($record, $extended, $fieldprefix);
    }

    /**
     * Creates a search condition for a given search value, and adds it to the
     * query that will be used for performing the actual search.
     *
     * @param Query $query The query to which the condition will be added.
     * @param string $table The name of the table in which this attribute
     *                                 is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one
     *                                 of the supported modes, as returned by this
     *                                 attribute's getSearchModes() method.
     * @param string $fieldaliasprefix optional prefix for the fieldalias in the table
     */
    public function searchCondition($query, $table, $value, $searchmode, $fieldaliasprefix = '')
    {
        $this->m_date->searchCondition($query, $table, $value, $searchmode, $fieldaliasprefix);
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
        // we only need to return the date part, because the dateattribute also
        // hides the other (time) elements that are present in the record (is that
        // a bug of the dateattribute?)
        $this->m_time->m_htmlid = $this->m_date->m_htmlid;
        return $this->m_date->hide($record, $fieldprefix, $mode);
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
     * Return the database field type of the attribute.
     *
     * @return string The 'generic' type of the database field for this
     *                attribute.
     */
    public function dbFieldType()
    {
        // TODO FIXME: Is this correct? Or does the datetimeattribute currently only support varchar fields?
        return 'datetime';
    }

    /**
     * Parse the string to convert a datetime string to an array.
     *
     * @param string $stringvalue
     *
     * @return array with date and time information
     */
    public function parseStringValue($stringvalue)
    {
        $datetime = explode(' ', $stringvalue);
        $formatsdate = array(
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
        $retval = array_merge(DateAttribute::parseDate($datetime[0], $formatsdate), TimeAttribute::parseTime($datetime[1]));

        return $retval;
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
        } else {
            if ($this->m_timezoneAttribute !== null) {
                $parts = explode('.', $this->m_timezoneAttribute);
                $node = $this->getOwnerInstance();

                while (count($parts) > 0) {
                    $part = array_shift($parts);
                    $attr = $node->getAttribute($part);

                    // relation, prepare for next iteration
                    if (is_a($attr, 'ManyToOneRelation')) {
                        if (count($parts) > 0 && !isset($record[$part][$parts[0]])) {
                            $attr->populate($record, array($parts[0]));
                        }

                        $record = $record[$attr->fieldName()];
                        $node = $attr->m_destInstance;
                    } // timezone attribute, calculate and return offset
                    else {
                        if (is_a($attr, 'TimezoneAttribute')) {
                            return $attr->getUTCOffset($record[$attr->fieldName()], $stamp);
                        } // assume the attribute in question already has the offset saved in seconds
                        else {
                            return (int)$record[$attr->fieldName()];
                        }
                    }
                }

                Tools::atkdebug('WARNING: could not determine UTC offset for atkDateTimeAttribute "'.$this->fieldName().'"!');

                return 0;
            } else {
                return 0;
            }
        }
    }

    /**
     * Convert the given ATK date/time array to a UTC date/time array.
     *
     * @param mixed $value UNIX timestamp or ATK date/time array
     * @param array $record record
     *
     * @return int|array UNIX timestamp or ATK date/time array (depending on input)
     */
    public function toUTC($value, &$record)
    {
        $stamp = is_int($value) ? $value : $this->arrayToDateTime($value);
        $offset = $this->_getUTCOffset($record, $stamp);
        $stamp = $stamp - $offset;
        $value = is_int($value) ? $stamp : $this->datetimeArray(date('YmdHis', $stamp));

        return $value;
    }

    /**
     * Convert the given UTC ATK date/time array to a date/time array in a certain timezone.
     *
     * @param mixed $value UNIX timestamp or ATK date/time array
     * @param array $record record
     *
     * @return int|array UNIX timestamp or ATK date/time array (depending on input)
     */
    public function fromUTC($value, &$record)
    {
        $stamp = is_int($value) ? $value : $this->arrayToDateTime($value);
        $offset = $this->_getUTCOffset($record, $stamp);
        $stamp = $stamp + $offset;
        $value = is_int($value) ? $stamp : $this->datetimeArray(date('YmdHis', $stamp));

        return $value;
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
