<?php namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;

/**
 * Attribute for selection the days of the week.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage attributes
 *
 */
class WeekdayAttribute extends NumberAttribute
{
    /**
     * Flags for atkWeekdayAttribute
     */
    const AF_WEEKDAY_SMALL_EDIT = self::AF_SPECIFIC_1;

    /**
     * Bitwise flags for weekdays.
     * @access private
     */
    const WD_MONDAY = 1;
    const WD_TUESDAY = 2;
    const WD_WEDNESDAY = 4;
    const WD_THURSDAY = 8;
    const WD_FRIDAY = 16;
    const WD_SATURDAY = 32;
    const WD_SUNDAY = 64;

    var $m_mapping = array(
        1 => 'monday',
        2 => 'tuesday',
        4 => 'wednesday',
        8 => 'thursday',
        16 => 'friday',
        32 => 'saturday',
        64 => 'sunday'
    );
    var $m_extra = array();

    /**
     * Constructor.
     *
     * @param String $name Name of the attribute (unique within a node, and
     *                     corresponds to the name of the datetime field
     *                     in the database where the stamp is stored.
     * @param int $extraOrFlags Flags for the attribute or array of extra options
     *                           these options will be numbered from 2^7 (128) to 2^x.
     * @param int $flags Flags for the attribute. Only used if no set in previous param.
     */
    function __construct($name, $extraOrFlags = 0, $flags = 0)
    {

        if (is_numeric($extraOrFlags)) {
            $flags = $extraOrFlags;
        } elseif (is_array($extraOrFlags)) {
            $this->m_extra = $extraOrFlags;
        }

        parent::__construct($name, ($flags | self::AF_HIDE_SEARCH) ^ self::AF_SEARCHABLE);
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
     * @return String The internal value
     */
    function fetchValue($postvars)
    {
        if (!is_array($postvars) || !is_array($postvars[$this->fieldName()])) {
            return 0;
        } else {
            return array_sum($postvars[$this->fieldName()]);
        }
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record, $fieldprefix = '', $mode = 'add')
    {
        $result = '';

        $name = $fieldprefix . $this->fieldName();
        $value = (int)$record[$this->fieldName()];

        $separator = $this->hasFlag(self::AF_WEEKDAY_SMALL_EDIT) || $mode == 'list' ? '&nbsp;'
            : '<br>';

        $max = 7 + count($this->m_extra);
        for ($i = 1; $i <= $max; $i++) {
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

            $result .= '<span title="' . $fullWeekday . '"><input type="checkbox" id="' . $name . '" name="' . $name . '[' . $i . ']" ' . $this->getCSSClassAttribute("atkcheckbox") . ' value="' . $day . '" ' . $checked . '> ' . $weekday . '</span>' . ($i < $max
                    ? $separator : '');
            $this->registerKeyListener($name . '[' . $i . ']', KB_CTRLCURSOR | KB_CURSOR);
        }

        return $result;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * In this case, the timestamp is returned in human readable format.
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists). The regular
     *                     Attribute does not use this parameter, but
     *                     derived attributes may use it to distinguish
     *                     between the two display modes.
     * @return String HTML String
     */
    function display($record, $mode = "list")
    {
        $result = '';
        $value = (int)$record[$this->fieldName()];

        $max = 7 + count($this->m_extra);
        for ($i = 1; $i <= $max; $i++) {
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

                $result .= (empty($result) ? '' : ($mode == 'list' ? ', ' : '<br>')) . $weekday;
            }
        }

        if (empty($result)) {
            return Tools::atktext('none');
        } else {
            return $result;
        }
    }

}

