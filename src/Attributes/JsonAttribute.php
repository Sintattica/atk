<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;

class JsonAttribute extends Attribute
{
    /** @var bool If true, in display shows only list of values without label. */
    protected $displayOnlyValues = false;

    function setDisplayOnlyValues(bool $value): self
    {
        $this->displayOnlyValues = $value;
        return $this;
    }

    public function display($record, $mode)
    {
        $value = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null;
        if (!$value) {
            return parent::display($record, $mode);
        }

        // TODO: check jsontable css
        $html = '<table class="jsontable">';

        foreach ($value as $a => $v) {
            $html .= "<tr>"; // start row

            if (!$this->displayOnlyValues) {
                // add label
                // TODO: translate label?
                $html .= "<td>" . $a . "</td>";
            }

            if (!is_array($v) or array_key_exists('value', $v)) {
                // it is an array with the field "value", otherwise it is not an array
                $html .= "<td>" . (array_key_exists('value', is_array($v) ? $v : []) ? $v['value'] : $v) . "</td>";
            } else {
                // it is an array without the field "value"
                $html .= (implode("<br>", $v)) . "</td>";
            }

            $html .= "</tr>"; // end row
        }

        $html .= '</table>';

        return $html;
    }

    public function edit($record, $fieldprefix, $mode)
    {
        if (is_array($record[$this->fieldName()])) {
            $record[$this->fieldName()] = json_encode($record[$this->fieldName()] ?: []);
        }
        return parent::edit($record, $fieldprefix, $mode);
    }

    public function validate(&$record, $mode)
    {
        if (is_array($record[$this->fieldName()])) {
            if (is_null($record[$this->fieldName()])) {
                $record[$this->fieldName()] = [];
            }
            $record[$this->fieldName()] = json_encode($record[$this->fieldName()]);
            if (json_last_error() != JSON_ERROR_NONE) {
                Tools::atkTriggerError($record, $this, 'error_invalid_json');
            }
        }

        if (!self::isJson($record[$this->fieldName()])) {
            Tools::atkTriggerError($record, $this, 'error_invalid_json');
        }
    }

    public function db2value($record)
    {
        $val = parent::db2value($record);
        if ($val and self::isJson($val)) {
            return json_decode($val, true);
        }
        return $val;
    }

    /**
     * Check if the passed string is a JSON.
     *
     * @param string $string
     * @return bool
     */
    public static function isJson($string)
    {
        if (!is_string($string)) {
            return false;
        }

        json_decode($string);
        return (json_last_error() == JSON_ERROR_NONE);
    }
}
