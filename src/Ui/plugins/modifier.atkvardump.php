<?php

use Sintattica\Atk\Core\Tools;

/**
 * Dump variable to debug output
 *
 * Usage: {$sometplvar|atkvardump:label}
 * @author Boy Baukema <boy@ibuildings.nl>
 *
 * @param mixed $data
 * @param string $name Label for the dump
 * @return String
 */
function smarty_modifier_atkvardump($data, $name = '')
{
    Tools::atk_var_dump($data, $name);

    return $data;
}
