<?php

namespace Sintattica\Atk\Attributes;

/**
 * The RowCounter can be added to a node to have a column in listviews
 * that sequentially numbers records.
 *
 * @author Przemek Piotrowski <przemek.piotrowski@nic.com.pl>
 * @author Ivo Jansch <ivo@achievo.org>
 */
class RowCounterAttribute extends DummyAttribute
{
    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     */
    public function __construct($name, $flags = 0)
    {
        $flags = $flags | self::AF_HIDE_VIEW | self::AF_HIDE_EDIT | self::AF_HIDE_ADD;
        parent::__construct($name, $flags, '');
    }

    /**
     * Returns a number corresponding to the row count per record.
     *
     * @param array $record
     * @param string $mode
     *
     * @return int Counter, starting at 1
     */
    public function display($record, $mode)
    {
        static $s_counter = 0;
        $node = $this->m_ownerInstance;

        return $node->m_postvars['atkstartat'] + (++$s_counter);
    }
}
