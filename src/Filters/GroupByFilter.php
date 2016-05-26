<?php

namespace Sintattica\Atk\Filters;

/**
 * Add a group by clausule to a query.
 *
 * Use this filter, like you use an attribute, for example:
 * $this->add(new atkGroupByFilter("street_place", "street, place"));
 *
 * @author Kees van Dieren <kees@ibuildings.nl>
 * @author Ivo Jansch <ivo@ibuildings.nl>
 */
class GroupByFilter extends Filter
{
    /*
     * the group by statement
     *
     * @access private
     * @var string groupbystmt
     */
    public $m_groupbystmt;

    /**
     * constructor.
     *
     * @param string $name
     * @param string $groupbystmt
     * @param int $flags
     */
    public function __construct($name, $groupbystmt, $flags = 0)
    {
        $this->m_groupbystmt = $groupbystmt;
        parent::__construct($name, $flags);
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        $query->addGroupBy($this->m_groupbystmt);
    }
}
