<?php namespace Sintattica\Atk\Filters;

use Sintattica\Atk\Db\Query;

/**
 * Add a distinct clause to a query.
 *
 * Use this filter, like you use an attribute, for example:
 * $this->add(new atkDistinctFilter());
 *
 * @author Ivo Jansch <ivo@ibuildings.nl>
 * @package atk
 * @subpackage filters
 *
 */
class DistinctFilter extends Filter
{

    /**
     * constructor
     */
    function __construct()
    {
        parent::__construct("distinctfilter");
    }


    function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        $query->setDistinct(true);
    }

}


