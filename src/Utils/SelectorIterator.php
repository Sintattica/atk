<?php namespace Sintattica\Atk\Utils;

use \IteratorIterator;
use \Iterator;

/**
 * Selector iterator, makes sure that each each row returned by the internal
 * iterator gets transformed before it is returned to the user.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage utils
 */
class SelectorIterator extends IteratorIterator
{
    /**
     * Selector.
     *
     * @var Selector
     */
    private $m_selector;

    /**
     * Constructor.
     *
     * @param Iterator $iterator iterator
     * @param Selector $selector selector
     */
    public function __construct(\Iterator $iterator, Selector $selector)
    {
        parent::__construct($iterator);
        $this->m_selector = $selector;
    }

    /**
     * Returns the selector.
     *
     * @return Selector selector
     */
    public function getSelector()
    {
        return $this->m_selector;
    }

    /**
     * Returns the current row transformed.
     */
    public function current()
    {
        $row = parent::current();

        if ($row != null) {
            $row = $this->getSelector()->transformRow($row);
        }

        return $row;
    }
}
