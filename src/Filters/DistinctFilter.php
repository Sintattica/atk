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

    /**
     * add the distinct statement to the query
     *
     * @param Query $query The SQL query object
     * @return void
     */
    function addToQuery(&$query)
    {
        $query->setDistinct(true);
    }

}


