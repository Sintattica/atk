<?php

namespace Sintattica\Atk\Attributes;

/**
 * Class ListBoolAttribute
 * 
 * Useful to have a "nullable" bool attribute
 *
 * Note: it must be a char, because in MySQL 0 == ''
 */
class ListBoolAttribute extends ListAttribute
{
    function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags, ['no', 'yes'], ['0', '1']);
    }
}