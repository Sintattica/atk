<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Language;

class ExpressionBoolAttribute extends ExpressionListAttribute
{
    /**
     * If TRUE, it shows 'n.a.' instead of empty string, when the sql query returns NULL.
     * @var bool $null_item
     */
    public $null_item = false;

    function __construct($name, $flags, $expression)
    {
        $listAttr = new ListAttribute($name, ListAttribute::AF_LIST_NO_NULL_ITEM, ['no', 'yes'], [0, 1]);
        
        parent::__construct($name, $flags, $expression, $listAttr);
    }

    function display($record, $mode)
    {
        if ($this->null_item and $record[$this->fieldName()] === null) {
            // the sql query has returned NULL
            return Language::text('n.d.', null);
        }

        // the sql query has returned NULL 0/1
        return parent::display($record, $mode);
    }
}
