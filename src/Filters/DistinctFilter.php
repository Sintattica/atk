<?php

namespace Sintattica\Atk\Filters;

/**
 * Add a distinct clause to a query.
 *
 * Use this filter, like you use an attribute, for example:
 * $this->add(new atkDistinctFilter());
 *
 * @author Ivo Jansch <ivo@ibuildings.nl>
 */
class DistinctFilter extends Filter
{
    /**
     * constructor.
     */
    public function __construct()
    {
        parent::__construct('distinctfilter');
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record = [], $level = 0, $mode = '')
    {
        $query->setDistinct(true);
    }
}
