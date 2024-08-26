<?php


namespace Sintattica\Atk\Attributes\NestedAttributes;


use Sintattica\Atk\Attributes\DateAttribute;
use Sintattica\Atk\Db\Query;

class NestedDateAttribute extends DateAttribute
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

    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        if (!$this->getOwnerInstance()->hasNestedAttribute($this->fieldName())) {
            return parent::getSearchCondition($query, $table, $value, $searchmode, $fieldname);
        }
        return parent::getSearchCondition($query, $table, $value, $searchmode, $this->buildSQLSearchValue($table));
    }

    protected function buildSQLSearchValue($table)
    {
        return "JSON_UNQUOTE(" . NestedAttribute::buildJSONExtractValue($this, $table) . ")";
    }

}
