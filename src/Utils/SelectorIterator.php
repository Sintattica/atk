<?php

namespace Sintattica\Atk\Utils;

use Exception;
use IteratorIterator;
use Iterator;

/**
 * Selector iterator, makes sure that each row returned by the internal
 * iterator gets transformed before it is returned to the user.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class SelectorIterator extends IteratorIterator
{
    private Selector $m_selector;

    /**
     * Constructor.
     *
     * @param Iterator $iterator iterator
     * @param Selector $selector selector
     */
    public function __construct(Iterator $iterator, Selector $selector)
    {
        parent::__construct($iterator);
        $this->m_selector = $selector;
    }

    public function getSelector(): Selector
    {
        return $this->m_selector;
    }

    /**
     * Returns the current row transformed.
     * @throws Exception
     */
    public function current(): mixed
    {
        $row = parent::current();

        if ($row != null) {
            $row = $this->getSelector()->transformRow($row);
        }

        return $row;
    }
}
