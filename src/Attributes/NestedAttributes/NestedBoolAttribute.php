<?php


namespace Sintattica\Atk\Attributes\NestedAttributes;

use Sintattica\Atk\Attributes\BoolAttribute;
use Sintattica\Atk\Db\Query;


class NestedBoolAttribute extends BoolAttribute
{
    use JsonSearchable;

    public function __construct($name, $flags = 0)
    {
        $this->setIsNestedAttribute(true);
        parent::__construct($name, $flags);
    }

}
