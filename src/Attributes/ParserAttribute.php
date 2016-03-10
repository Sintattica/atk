<?php namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\DataGrid\DataGrid;
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
    public $m_text;

    /**
     * Constructor
     * @param string $name Name of the attribute
     * @param string $text text field
     * @param int $flags Flags for this attribute
     */
    public function __construct($name, $text, $flags = 0)
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
    public function edit($record, $fieldprefix, $mode)
    {
        return $this->display($record, $fieldprefix, $mode);
    }

    /**
     * VOID implementation.. parserAttribute has no data associated with it, so you can't search it.
     * @param array $record Array with fields
     */
    public function search($record, $extended = false, $fieldprefix = "", DataGrid $grid = null)
    {
        return "&nbsp;";
    }

    /**
     * Parses a record
     * @param array $record Array with fields
     * @param string $mode
     * @return string Parsed string
     */
    public function display($record, $mode)
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
    public function store($db, $record, $mode)
    {
        return true;
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
    }

    /**
     * Dummy implementation
     *
     * @return string Empty string
     */
    public function dbFieldType()
    {
        return "";
    }
}
