<?php

namespace Sintattica\Atk\Attributes\NestedAttributes;

use Sintattica\Atk\Attributes\DateTimeAttribute;

class NestedDateTimeAttribute extends DateTimeAttribute
{
    public function __construct($name, $flags = 0, $format_edit = '', $format_view = '', $min = 0, $max = 0)
    {
        $this->setIsNestedAttribute(true);
        parent::__construct($name, $flags, $format_edit, $format_view, $min, $max);
    }

    public function getOrderByStatement($extra = [], $table = '', $direction = 'ASC')
    {
        $json_query = NestedAttribute::getOrderByStatementStatic($this, $extra, $table, $direction);
        if ($json_query) {
            return $json_query;
        }

        return parent::getOrderByStatement($extra, $table, $direction);
    }

    public function searchCondition($query, $table, $value, $searchmode, $fieldaliasprefix = '')
    {
        if (!$this->getOwnerInstance()->isNestedAttribute($this->fieldName())) {
            parent::searchCondition($query, $table, $value, $searchmode, $fieldaliasprefix);
        }
        $this->m_date->searchCondition($query, $table, $value, $searchmode, $this->buildSQLSearchValue($table));
    }

    protected function buildSQLSearchValue($table): string
    {
        return "JSON_UNQUOTE(" . NestedAttribute::buildJSONExtractValue($this, $table) . ")";
    }

}
