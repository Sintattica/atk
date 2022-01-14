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

    // TODO implementare
    public function getOrderByStatement($extra = [], $table = '', $direction = 'ASC')
    {
        return '';
    }

    // TODO implementare
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        return '';
    }

}
