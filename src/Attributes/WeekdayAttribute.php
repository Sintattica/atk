<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;

/**
 * Attribute for selection the days of the week.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
class WeekdayAttribute extends NumberAttribute
{
    /**
     * Flags for atkWeekdayAttribute.
     */
    const AF_WEEKDAY_SMALL_EDIT = 33554432;

    /**
     * Bitwise flags for weekdays.
     */
    const WD_MONDAY = 1;
    const WD_TUESDAY = 2;
    const WD_WEDNESDAY = 4;
    const WD_THURSDAY = 8;
    const WD_FRIDAY = 16;
    const WD_SATURDAY = 32;
    const WD_SUNDAY = 64;

    public $m_mapping = [
        1 => 'monday',
        2 => 'tuesday',
        4 => 'wednesday',
        8 => 'thursday',
        16 => 'friday',
        32 => 'saturday',
        64 => 'sunday',
    ];
    public $m_extra = [];

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute (unique within a node, and
     *                             corresponds to the name of the datetime field
     *                             in the database where the stamp is stored.
     * @param int $flags Flags for the attribute
     * @param array $extra Array of extra options. these options will be numbered from 2^7 (128) to 2^x.
     */
    public function __construct($name, $flags = 0, $extra = [])
    {
        $flags = ($flags | self::AF_HIDE_SEARCH) ^ self::AF_SEARCHABLE;
        $this->m_extra = $extra;

        parent::__construct($name, $flags);
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
        $result = parent::fetchValue();
        if (!is_array($result)) {
            return 0;
        } else {
            return array_sum($result);
        }
    }

    public function edit($record, $fieldprefix, $mode)
    {
        $result = '';
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);
        $value = intval(Tools::atkArrayNvl($record, $this->fieldName()));

        $separator = $this->hasFlag(self::AF_WEEKDAY_SMALL_EDIT) || $mode == 'list' ? '&nbsp;' : '<br>';

        $max = 7 + Tools::count($this->m_extra);
        for ($i = 1; $i <= $max; ++$i) {
            $day = pow(2, $i - 1);

            if ($i <= 7) {
                $weekday = Tools::atktext($this->m_mapping[$day]);
            } else {
                $weekday = $this->m_extra[$i - 8];
            }

            $weekday = ucfirst($weekday);
            $fullWeekday = $weekday;
            if ($this->hasFlag(self::AF_WEEKDAY_SMALL_EDIT) || $mode == 'list') {
                $weekday = substr($weekday, 0, 2);
            }

            $checked = Tools::hasFlag($value, $day) ? ' checked' : '';

            $result .= '<span title="'.$fullWeekday.'"><input type="checkbox" id="'.$id.'" name="'.$name.'['.$i.']" '.$this->getCSSClassAttribute('atkcheckbox').' value="'.$day.'" '.$checked.'> '.$weekday.'</span>'.($i < $max ? $separator : '');
        }

        return $result;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * In this case, the timestamp is returned in human readable format.
     *
     * @param array $record The record that holds the value for this attribute
     * @param string $mode The display mode ("view" for viewpages, or "list"
     *                       for displaying in recordlists). The regular
     *                       Attribute does not use this parameter, but
     *                       derived attributes may use it to distinguish
     *                       between the two display modes.
     *
     * @return string HTML String
     */
    public function display($record, $mode)
    {
        $result = '';
        $value = (int)$record[$this->fieldName()];

        $max = 7 + Tools::count($this->m_extra);
        for ($i = 1; $i <= $max; ++$i) {
            $day = pow(2, $i - 1);

            if (Tools::hasFlag($value, $day)) {
                if ($i <= 7) {
                    $weekday = $this->m_mapping[$day];
                    if ($mode == 'list') {
                        $weekday = substr($weekday, 0, 3);
                    }
                    $weekday = Tools::atktext($weekday);
                } else {
                    $weekday = $this->m_extra[$i - 8];
                }

                $result .= (empty($result) ? '' : ($mode == 'list' ? ', ' : '<br>')).$weekday;
            }
        }

        if (empty($result)) {
            return Tools::atktext('none');
        } else {
            return $result;
        }
    }
}
