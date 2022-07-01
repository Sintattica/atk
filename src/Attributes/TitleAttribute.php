<?php

namespace Sintattica\Atk\Attributes;

class TitleAttribute extends DummyAttribute
{
    public function __construct($name, $flags = 0, $text = '', $sep = '')
    {
        $title = "<h4 class='text-center'><strong>$text</strong></h4>";
        if ($sep) {
            $title = $sep . $title;
        }
        parent::__construct($name, $flags | self::AF_NO_LABEL | self::AF_HIDE_LIST, $title);
    }
}
