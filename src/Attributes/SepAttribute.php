<?php

namespace Sintattica\Atk\Attributes;

class SepAttribute extends DummyAttribute
{
    public function __construct($name, $flags = 0, $class = '')
    {
        $text = $class ? '<hr class="' . $class . '">' : '<hr>';
        parent::__construct($name, $flags | self::AF_NO_LABEL | self::AF_HIDE_LIST, $text);
    }
}
