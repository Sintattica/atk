<?php

namespace Sintattica\Atk\Attributes;

/**
 * Class ExpressionCountAttribute
 */
class ExpressionCountAttribute extends ExpressionAttribute
{
    function __construct($name, $flags, $table, $condition)
    {
        $expression = "SELECT COUNT(*) FROM $table WHERE $condition";
        parent::__construct($name, $flags | self::AF_HIDE_VIEW | self::AF_HIDE_EDIT | self::AF_HIDE_SEARCH, $expression, 'number');
    }

    public function display(array $record, string $mode): string
    {
        return '<div style="text-align:center">' . ($record[$this->fieldName()] ?: '') . '</div>';
    }
}
