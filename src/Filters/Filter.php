<?php namespace Sintattica\Atk\Filters;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Db\Query;

/**
 * Abstract base class for atkFilters.
 *
 * @author Ivo Jansch <ivo@ibuildings.nl>
 * @author Kees van Dieren <kees@ibuildings.nl>
 * @package atk
 * @subpackage filters
 * @abstract
 */
class Filter extends Attribute
{

    /**
     * Constructor
     *
     * @param string $name The name of the filter
     * @param int $flags The flags of the filter
     * @return Filter
     */
    public function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags | Attribute::AF_HIDE | Attribute::AF_FORCE_LOAD);
    }


    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
    }
}
