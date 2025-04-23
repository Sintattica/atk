<?php

namespace Sintattica\Atk\Attributes\NestedAttributes;

use Exception;
use Sintattica\Atk\Attributes\DateTimeAttribute;
use Sintattica\Atk\Db\Query;

class NestedDateTimeAttribute extends DateTimeAttribute implements NestedAttributeInterface
{
    /**
     * @throws Exception
     */
    public function __construct($name, $flags, string $nestedAttributeField, $format_edit = '', $format_view = '', $min = 0, $max = 0)
    {
        $this->setNestedAttributeField($nestedAttributeField);
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

//    public function searchCondition($query, $table, $value, $searchmode, $fieldaliasprefix = '')
//    {
//        if (!$this->getOwnerInstance()->hasNestedAttribute($this->fieldName(), $this->getNestedAttributeField())) {
//            parent::searchCondition($query, $table, $value, $searchmode, $fieldaliasprefix);
//        }
//        $this->m_date->searchCondition($query, $table, $value, $searchmode, $this->buildSQLSearchValue($table));
//    }

    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        if (!$this->getOwnerInstance()->hasNestedAttribute($this->fieldName(), $this->getNestedAttributeField())) {
            return parent::getSearchCondition($query, $table, $value, $searchmode, $fieldname);
        }
        return parent::getSearchCondition($query, $table, $value, $searchmode, $this->buildSQLSearchValue($table));
    }


    protected function buildSQLSearchValue($table): string
    {
        return "JSON_UNQUOTE(" . NestedAttribute::buildJSONExtractValue($this, $table) . ")";
    }

}
