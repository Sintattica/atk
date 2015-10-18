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
    function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags | Attribute::AF_HIDE | Attribute::AF_FORCE_LOAD);
    }

    /**
     * Adds this attribute to database queries.
     *
     * @param Query $query The SQL query object
     * @param String $tablename The name of the table of this attribute
     * @param String $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
     * @param Array $rec The record that contains the value of this attribute.
     * @param int $level Recursion level if relations point to eachother, an
     *                   endless loop could occur if they keep loading
     *                   eachothers data. The $level is used to detect this
     *                   loop. If overriden in a derived class, any subcall to
     *                   an addToQuery method should pass the $level+1.
     * @param String $mode Indicates what kind of query is being processing:
     *                     This can be any action performed on a node (edit,
     *                     add, etc) Mind you that "add" and "update" are the
     *                     actions that store something in the database,
     *                     whereas the rest are probably select queries.
     */
    function addToQuery(&$query, $tablename = "", $fieldaliasprefix = "", $rec = "", $level, $mode)
    {

    }

}

