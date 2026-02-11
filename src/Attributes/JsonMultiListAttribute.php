<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Query;

/**
 * Class JsonMultiListAttribute
 *
 * Extends MultiListAttribute to support JSON array format.
 */
class JsonMultiListAttribute extends MultiListAttribute
{
    public function __construct($name, $flags, $optionArray, $valueArray = null)
    {
        parent::__construct($name, $flags, $optionArray, $valueArray);

        $this->m_fieldSeparator = null; // the format is JSON, so it has no separators
    }

    /**
     * Stores the values in JSON format (["value1", "value2", ...])
     *
     * @param array $record
     * @return string JSON encoded array or empty string
     */
    function value2db($record): string
    {
        if (is_array($record[$this->fieldName()]) && Tools::count($record[$this->fieldName()]) >= 1) {
            return $this->escapeSQL(json_encode($record[$this->fieldName()]));
        }

        return '';
    }

    /**
     * Converts a database value to an internal value.
     * Supports JSON format (["value1", "value2", ...])
     *
     * @param array $record The database record that holds this attribute's value
     * @return array The internal value
     */
    function db2value($record): array
    {
        if (!isset($record[$this->fieldName()]) || $record[$this->fieldName()] === '') {
            return [];
        }

        return json_decode($record[$this->fieldName()], true);
    }

    function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = ''): string
    {
        /**
         * MultiListAttribute has only 1 searchmode and that is "substring".
         * @see getSearchModes()
         */
        $searchconditions = [];
        $fieldExpression = $table . '.' . $this->fieldName();

        if (is_array($value) && $value[0] != '' && count($value) > 0) {
            if (in_array('__NONE__', $value)) {
                return $query->nullCondition($fieldExpression, true);
            }
            foreach ($value as $str) {
                $searchconditions[] = "JSON_CONTAINS($fieldExpression, '\"" . $this->escapeSQL($str) . "\"')";
            }
        }

        return count($searchconditions) ? '(' . implode(' OR ', $searchconditions) . ')' : '';
    }

    public function setFieldSeparator(string $m_fieldSeparator): static
    {
        $this->m_fieldSeparator = null; // override the parent method and force JSON format
        return $this;
    }
}
