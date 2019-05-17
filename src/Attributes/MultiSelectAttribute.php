<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Db\QueryPart;

/**
 * The MultiSelectAttribute class represents an attribute of a node
 * that has a field with checkboxes, and stores the input seperated by a '|'.
 *
 * @author Rene Bakx <rene@ibuildings.nl>
 */
class MultiSelectAttribute extends ListAttribute
{
    /** Defines */
    const AF_NO_TOGGLELINKS = 67108864;
    const AF_CHECK_ALL = 134217728;
    const AF_LINKS_BOTTOM = 268435456;

    /**
     * Default field separator.
     *
     * @var string separator
     */
    protected $m_fieldSeparator = '|';

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_STRING;

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param array $optionArray Array with options
     * @param $valueArray $value Array with values. If you don't use this parameter,
     *                    values are assumed to be the same as the options.
     */
    public function __construct($name, $flags = 0, $optionArray, $valueArray = null)
    {
        parent::__construct($name, $flags, $optionArray, $valueArray);

        $size = 0;
        $valueArray = $this->getValues();
        for ($i = 0, $_i = Tools::count($valueArray); $i < $_i; ++$i) {
            $size += (Tools::atk_strlen($valueArray[$i]) + 1); // 1 extra for the '|' symbol
        }
        if ($size > 0) {
            $this->setAttribSize($size);
        }
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">).
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        $result = '';
        if (is_array($record[$this->fieldName()])) {
            $values = $this->getValues();
            for ($i = 0; $i < Tools::count($values); ++$i) {
                if (in_array($values[$i], $record[$this->fieldName()])) {
                    $result .= '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'[]" value="'.$values[$i].'">';
                }
            }
        } else {
            parent::hide($record, $fieldprefix, $mode);
        }

        return $result;
    }


    public function value2db($rec)
    {
        if (is_array($rec[$this->fieldName()]) && Tools::count($rec[$this->fieldName()]) >= 1) {
            return implode($this->m_fieldSeparator, $rec[$this->fieldName()]);
        } else {
            return '';
        }
    }

    /**
     * Converts a database value to an internal value.
     *
     * @param array $rec The database record that holds this attribute's value
     *
     * @return mixed The internal value
     */
    public function db2value($rec)
    {
        if (isset($rec[$this->fieldName()]) && $rec[$this->fieldName()] !== '') {
            return explode($this->m_fieldSeparator, $rec[$this->fieldName()]);
        } else {
            return [];
        }
    }

    /**
     * Allows you to set the field separator
     * which is used to separate values in the
     * database. i.e use a comma separator for
     * mysql set datatype.
     *
     * @param string $separator
     */
    public function setFieldSeparator($separator)
    {
        $this->m_fieldSeparator = $separator;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * @param array $record The record that holds the value for this attribute
     * @param string $mode The display mode ("view" for viewpages, or "list"
     *                       for displaying in recordlists, "edit" for
     *                       displaying in editscreens, "add" for displaying in
     *                       add screens. "csv" for csv files. Applications can
     *                       use additional modes.
     *
     * @return string HTML String
     */
    public function display($record, $mode)
    {
        $values = $record[$this->fieldName()];
        $res = [];
        for ($i = 0; $i < Tools::count($values); ++$i) {
            $res[] = $this->_translateValue($values[$i], $record);
        }

        return implode(', ', $res);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record Array with fields
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string piece of html code with radioboxes
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);

        $page = Page::getInstance();
        $page->register_script(Config::getGlobal('assets_url').'javascript/profileattribute.js');

        $result = '';
        if (!$this->hasFlag(self::AF_LINKS_BOTTOM)) {
            $result .= $this->_addLinks($fieldprefix);
        }

        $css = $this->getCSSClassAttribute('');
        $result .= '<div '.$css.'>';

        $values = $this->getValues();
        if (!is_array($record[$this->fieldName()])) {
            $recordvalue = $this->db2value($record);
        } else {
            $recordvalue = $record[$this->fieldName()];
        }

        for ($i = 0; $i < Tools::count($values); ++$i) {
            $checkId = $id.'_'.$i;
            $checkName = $name.'[]';

            if (!$this->hasFlag(self::AF_CHECK_ALL)) {
                $sel = (Tools::atk_in_array($values[$i], $recordvalue)) ? 'checked' : '';
            } else {
                $sel = 'checked';
            }

            $result .= '<div>';
            $result .= '<input type="checkbox" id="'.$checkId.'" name="'.$checkName.'" value="'.$values[$i].'" '.$sel.'> ';
            $result .= $this->_translateValue($values[$i], $record);
            $result .= '</div>';
        }
        $result .= '</div>';
        if ($this->hasFlag(self::AF_LINKS_BOTTOM)) {
            $result .= $this->_addLinks($fieldprefix);
        }

        return $result;
    }

    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        // Multiselect attribute has only 1 searchmode, and that is substring.
        $searchcondition = '';
        if (!is_array($value) || $value[0] == '' || empty($value)) {
            return null;
        }

        $searchconditions = [];

        $keyNone = array_search('__NONE__', $value);
        if ($keyNone !== FALSE) {
            $searchconditions[] = $query->nullCondition(Db::quoteIdentifier($table, $this->fieldName()), true);
            // Removing '__NONE__' and reindexing $value :
            unset($value[$keyNone]);
            $value = array_values($value);
        }

        foreach ($value as $str) {
            $searchconditions[] = $query->substringCondition(Db::quoteIdentifier($table, $this->fieldName()), $str);
        }
        return QueryPart::implode('OR', $searchconditions, true);
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * @return array List of supported searchmodes
     */
    public function getSearchModes()
    {
        // exact match and substring search should be supported by any database.
        // (the LIKE function is ANSI standard SQL, and both substring and wildcard
        // searches can be implemented using LIKE)
        // Possible values
        //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
        return array('substring');
    }

    /**
     * Add the checkall, checknone and checkinvert links.
     *
     * @param string $fieldprefix The fieldprefix
     *
     * @return string a piece of htmlcode with the links
     */
    public function _addLinks($fieldprefix)
    {
        if (Tools::count($this->m_values) > 4 && !Tools::hasFlag($this->m_flags, self::AF_NO_TOGGLELINKS)) {
            return '<div align="left">[<a href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkAll(\''.$fieldprefix.$this->fieldName().'\'); return false;">'.Tools::atktext('check_all').'</a> <a href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkNone(\''.$fieldprefix.$this->fieldName().'\'); return false;">'.Tools::atktext('check_none').'</a> <a href="javascript:void(0)" onclick="ATK.ProfileAttribute.profile_checkInvert(\''.$fieldprefix.$this->fieldName().'\'); return false;">'.Tools::atktext('invert_selection').'</a>]</div>';
        }

        return '';
    }

    /**
     * Check if a record has an empty value for this attribute.
     *
     * @param array $record The record that holds this attribute's value.
     *
     * @return bool
     */
    public function isEmpty($record)
    {
        return !isset($record[$this->fieldName()]) || (!is_array($record[$this->fieldName()])) || (is_array($record[$this->fieldName()]) && Tools::count($record[$this->fieldName()]) === 0);
    }
}
