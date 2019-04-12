<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Utils\StringParser;
use Sintattica\Atk\Db\Db;

/**
 * The atkParserAttribute can be used to create links or texts that
 * contain values, by supplying a template as parameter.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ParserAttribute extends Attribute
{
    public $m_text;

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_UNSUPPORTED;

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param string $text text field
     */
    public function __construct($name, $flags = 0, $text)
    {
        $flags = $flags | self::AF_HIDE_SEARCH | self::AF_NO_SORT;
        $this->m_text = $text;
        parent::__construct($name, $flags);
    }

    /**
     * Parses a record.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string Parsed string
     */
    public function edit($record, $fieldprefix, $mode)
    {
        return $this->display($record, $mode);
    }

    public function search($atksearch, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        // VOID implementation.. parserAttribute has no data associated with it, so you can't search it.
        return '';
    }

    /**
     * Parses a record.
     *
     * @param array $record Array with fields
     * @param string $mode
     *
     * @return string Parsed string
     */
    public function display($record, $mode)
    {
        $stringparser = new StringParser($this->m_text);

        return $stringparser->parse($record);
    }

    /**
     * No function, but is neccesary.
     *
     * @param Db $db The database object
     * @param array $record The record
     * @param string $mode
     *
     * @return bool
     */
    public function store($db, $record, $mode)
    {
        return true;
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
    }
}
