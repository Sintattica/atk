<?php

namespace Sintattica\Atk\Attributes\NestedAttributes;

use Sintattica\Atk\Db\Query;

interface NestedAttributeInterface
{
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '');
}