<?php

namespace Sintattica\Atk\Attributes\NestedAttributes;

use Exception;
use Sintattica\Atk\Attributes\MultiListAttribute;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Query;

class NestedMultiListAttribute extends MultiListAttribute implements NestedAttributeInterface
{
    /**
     * @throws Exception
     */
    function __construct($name, $flags, string $nestedAttributeField, $optionArray, $valueArray = null)
    {
        $this->setNestedAttributeField($nestedAttributeField);

        parent::__construct($name, $flags | parent::AF_NO_SORT, $optionArray, $valueArray);
    }

    /**
     * Overload the parent function to allow search through the JSON field
     *
     * @return string
     */
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        if (!$this->getOwnerInstance()->hasNestedAttribute($this->fieldName(), $this->getNestedAttributeField())) {
            return parent::getSearchCondition($query, $table, $value, $searchmode, $fieldname);
        }

        // Multiselect attribute has only 1 searchmode, and that is substring.

        $searchconditions = [];
        $fieldExpression = NestedAttribute::buildJSONExtractValue($this, $table);

        if (is_array($value) && $value[0] != '' && count($value) > 0) {
            if (in_array('__NONE__', $value)) {
                return $query->nullCondition($fieldExpression, true);
            }
            // includes the separators in the value to search; in this way the search is more secure
            if (count($value) == 1) {
                $searchconditions[] = $query->substringCondition($fieldExpression, Tools::escapeSQL($this->m_fieldSeparator . $value[0] . $this->m_fieldSeparator));
            } else {
                foreach ($value as $str) {
                    $searchconditions[] = $query->substringCondition($fieldExpression, Tools::escapeSQL($this->m_fieldSeparator . $str . $this->m_fieldSeparator));
                }
            }
        }

        if (count($searchconditions)) {
            return '(' . implode(' OR ', $searchconditions) . ')';
        }

        return '';
    }
}
