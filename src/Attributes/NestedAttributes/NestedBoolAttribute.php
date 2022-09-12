<?php


namespace Sintattica\Atk\Attributes\NestedAttributes;

use Sintattica\Atk\Attributes\BoolAttribute;
use Sintattica\Atk\Db\Query;


class NestedBoolAttribute extends BoolAttribute
{

    public function __construct($name, $flags = 0)
    {
        $this->setIsNestedAttribute(true);
        parent::__construct($name, $flags);
    }

    public function getOrderByStatement($extra = [], $table = '', $direction = 'ASC'): string
    {
        $json_query = NestedAttribute::getOrderByStatementStatic($this, $extra, $table, $direction);
        if ($json_query) {
            return $json_query;
        }

        return parent::getOrderByStatement($extra, $table, $direction);
    }

    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = ''): string
    {
        if (!$this->getOwnerInstance()->isNestedAttribute($this->fieldName())) {
            return parent::getSearchCondition($query, $table, $value, $searchmode, $fieldname);
        }
        return parent::getSearchCondition($query, $table, $value, $searchmode, $this->buildSQLSearchValue($table));
    }

    protected function buildSQLSearchValue($table): string
    {
        return "JSON_UNQUOTE(" . NestedAttribute::buildJSONExtractValue($this, $table) . ")";
    }

}
