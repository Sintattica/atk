<?php namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Utils\StringParser;

/**
 * The atkParserAttribute can be used to create links or texts that
 * contain values, by supplying a template as parameter.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage attributes
 *
 */
class ParserAttribute extends Attribute
{
    var $m_text;

    /**
     * Constructor
     * @param string $name Name of the attribute
     * @param string $text text field
     * @param int $flags Flags for this attribute
     */
    function __construct($name, $text, $flags = 0)
    {
        parent::__construct($name, $flags | self::AF_HIDE_SEARCH | self::AF_NO_SORT); // base class constructor
        $this->m_text = $text;
    }

    /**
     * Parses a record
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     * @return string Parsed string
     */
    function edit($record = "", $fieldprefix = "", $mode = "")
    {
        return $this->display($record);
    }

    /**
     * VOID implementation.. parserAttribute has no data associated with it, so you can't search it.
     * @param array $record Array with fields
     */
    function search($record = "")
    {
        return "&nbsp;";
    }

    /**
     * Parses a record
     * @param array $record Array with fields
     * @param string $mode
     * @return string Parsed string
     */
    function display($record, $mode = '')
    {
        $stringparser = new StringParser($this->m_text);
        return $stringparser->parse($record);
    }

    /**
     * No function, but is neccesary
     *
     * @param Db $db The database object
     * @param array $record The record
     * @param string $mode
     */
    function store($db, $record, $mode)
    {
        return true;
    }

    /**
     * No function, but is neccesary
     *
     * @param Query $query The SQL query object
     * @param string $tablename The name of the table of this attribute
     * @param string $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
     * @param array $rec The record that contains the value of this attribute.
     * @param int $level Recursion level if relations point to eachother, an
     *                   endless loop could occur if they keep loading
     *                   eachothers data. The $level is used to detect this
     *                   loop. If overriden in a derived class, any subcall to
     *                   an addToQuery method should pass the $level+1.
     * @param string $mode Indicates what kind of query is being processing:
     *                     This can be any action performed on a node (edit,
     *                     add, etc) Mind you that "add" and "update" are the
     *                     actions that store something in the database,
     *                     whereas the rest are probably select queries.
     */
    function addToQuery(&$query, $tablename = "", $fieldaliasprefix = "", $rec, $level, $mode)
    {

    }

    /**
     * Dummy implementation
     *
     * @return string Empty string
     */
    function dbFieldType()
    {
        return "";
    }

}


