<?php namespace Sintattica\Atk\Attributes;

/**
 * The atkRowCounter can be added to a node to have a column in listviews
 * that sequentially numbers records.
 *
 * The attribute evolved from a discussion at http://achievo.org/forum/viewtopic.php?t=478
 * and was added to ATK based on suggestion and documentation by Jorge Garifuna.
 *
 * @author Przemek Piotrowski <przemek.piotrowski@nic.com.pl>
 * @author Ivo Jansch <ivo@achievo.org>
 *
 * @package atk
 * @subpackage attributes
 *
 */
class RowCounter extends DummyAttribute
{

    /**
     * Constructor
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     */
    function __construct($name, $flags = 0)
    {
        parent::__construct($name, '', $flags | self::AF_HIDE_VIEW | self::AF_HIDE_EDIT | self::AF_HIDE_ADD);
    }

    /**
     * Returns a number corresponding to the row count per record.
     * @param array $record
     * @param string $mode
     * @return int Counter, starting at 1
     */
    function display($record, $mode)
    {
        static $s_counter = 0;
        $node = $this->m_ownerInstance;
        return $node->m_postvars["atkstartat"] + (++$s_counter);
    }

}

