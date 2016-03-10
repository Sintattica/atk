<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;

/**
 * With the atkDummyAttribute class you can place comments between other
 * attributes.
 *
 * Use the flag self::AF_NOLABEL if you want to start at the beginning of the
 * line.
 *
 * @author Sandy Pleyte <sandy@ibuildings.nl>
 */
class DummyAttribute extends Attribute
{
    /**
     * Custom flags.
     */
    const AF_DUMMY_SHOW_LABEL = 33554432; // make the dummy label its fields

    public $m_text;

    /**
     * Constructor.
     *
     * @param string $name  The name of the attribute
     * @param string $text  The text to display
     * @param int    $flags The flags for this attribute
     */
    public function __construct($name, $text = '', $flags = 0)
    {
        // A Dummy attrikbute should not be searchable and sortable
        $flags |= self::AF_HIDE_SEARCH | self::AF_NO_SORT;

        // Add the self::AF_BLANKLABEL flag unless the self::AF_DUMMY_SHOW_LABEL flag wasn't present
        if (!Tools::hasFlag($flags, self::AF_DUMMY_SHOW_LABEL)) {
            $flags |= self::AF_BLANKLABEL;
        }

        parent::__construct($name, $flags); // base class constructor
        $this->m_text = $text;
    }

    /**
     * Add flag.
     *
     * @param int $flag flag
     */
    public function addFlag($flag)
    {
        parent::addFlag($flag);

        if ($this->hasFlag(self::AF_DUMMY_SHOW_LABEL)) {
            $this->removeFlag(self::AF_BLANK_LABEL);
        }
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * Here it will only return the text, no edit box.
     *
     * @param array  $record      The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode        The mode we're in ('add' or 'edit')
     *
     * @return string A piece of htmlcode for editing this attribute
     */
    public function edit($record, $fieldprefix, $mode)
    {
        return "<div ID=\"$this->m_name\">".$this->m_text.'</div>';
    }

    /**
     * Returns a piece of html code that can be used to get search terms input
     * from the user.
     * VOID implementation, dummy attributes cannot be searched.
     *
     * @param array  $record      Array with values
     * @param bool   $extended    if set to false, a simple search input is
     *                            returned for use in the searchbar of the
     *                            recordlist. If set to true, a more extended
     *                            search may be returned for the 'extended'
     *                            search page. The Attribute does not
     *                            make a difference for $extended is true, but
     *                            derived attributes may reimplement this.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     *
     * @return string A piece of html-code
     */
    public function search($record, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        return '&nbsp;';
    }

    /**
     * Display a record
     * Here it will only return the text.
     *
     * @param array $record Array with fields
     *
     * @return string Text
     */
    public function display($record, $mode)
    {
        return $this->m_text;
    }

    /**
     * Set the text of this attribute.
     *
     * @param string $text
     */
    public function setText($text)
    {
        $this->m_text = $text;
    }

    /**
     * Get the text of the attribute.
     *
     * @return string The text of the attribute
     */
    public function getText()
    {
        return $this->m_text;
    }

    /**
     * No function, but is neccesary.
     *
     * @param Db     $db     Database object
     * @param array  $record The record
     * @param string $mode   The mode
     *
     * @return bool to indicate if store went succesfully
     */
    public function store($db, $record, $mode)
    {
        return true;
    }

    /**
     * Convert the database value to an internally used value
     * Since dummyattrbiutes are not stored in the database this function returns NULL.
     *
     * @param array $record The record
     */
    public function db2value($record)
    {
        return;
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     * Since this attribute does not support searching it returns an empty array.
     *
     * @return empty array
     */
    public function getSearchModes()
    {
        // exact match and substring search should be supported by any database.
        // (the LIKE function is ANSI standard SQL, and both substring and wildcard
        // searches can be implemented using LIKE)
        // Possible values
        //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
        return array();
    }

    /**
     * Return the database field type of the attribute.
     * VOID implementation because dummy attributes are not stored in the database.
     *
     * @return string empty string
     */
    public function dbFieldType()
    {
        return '';
    }
}
