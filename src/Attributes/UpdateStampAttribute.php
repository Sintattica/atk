<?php

namespace Sintattica\Atk\Attributes;

/**
 * Attribute for keeping track of last-modification times.
 *
 * The atkUpdateStampAttribute class can be used to automatically store the
 * date and time of the last modification of a record.
 * To use this attribute, add a DATETIME field to your table and add this
 * attribute to your node. No params are necessary, no initial_values need
 * to be set. The timestamps are generated automatically.
 * This attribute is automatically set to readonly, and to af_hide_add
 * (because we only have the first timestamp AFTER a record is added).
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class UpdateStampAttribute extends DateTimeAttribute
{
    /**
     * Constructor.
     *
     * @param string $name Name of the attribute (unique within a node, and
     *                      corresponds to the name of the datetime field
     *                      in the database where the stamp is stored.
     * @param int $flags Flags for the attribute.
     */
    public function __construct($name, $flags = 0)
    {
        parent::__construct($name, date('Y-m-d'), date('H:i:s'), $flags | self::AF_READONLY | self::AF_HIDE_ADD);
        $this->setForceInsert(true);
        $this->setForceUpdate(true);
        $this->setInitialValue(self::datetimeArray());
    }

    /**
     * Value to DB.
     *
     * @param array $record The record
     *
     * @return string The value to store in the database
     */
    public function value2db($record)
    {
        // if record not created using a form this situation can occur, so set the value here
        // Every time we must overwrite the value of this attribute, because this is UPDATE stamp
        $record[$this->fieldName()] = $this->initialValue();

        return parent::value2db($record);
    }

    /**
     * Override the initial value.
     *
     * @return array
     */
    public function initialValue()
    {
        return self::datetimeArray();
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
        $field = $record[$this->fieldName()];
        $result = '';
        if (is_array($field)) {
            foreach ($field as $key => $value) {
                $result .= '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'['.$key.']" '.'value="'.$value.'">';
            }
        } else {
            $result = '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'" value="'.$field.'">';
        }

        return $result;
    }

    /**
     * We always have a value, even if we're not even in the record.
     *
     * @return bool false
     * */
    public function isEmpty()
    {
        return false;
    }
}
