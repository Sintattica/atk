<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Query;

class MultiSelectListAttribute extends ListAttribute
{

    const AF_NO_TOGGLELINKS = 536870912;

    /**
     * Default field separator.
     *
     * @var string separator
     */
    protected $m_fieldSeparator = '|';

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


    public function value2db(array $rec)
    {
        if (is_array($rec[$this->fieldName()]) && Tools::count($rec[$this->fieldName()]) >= 1) {
            return $this->escapeSQL(implode($this->m_fieldSeparator, $rec[$this->fieldName()]));
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

    public function getFieldSeparator()
    {
        return $this->m_fieldSeparator;
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

        return $res ? '<span class="badge-sm badge-pill badge-secondary mr-1">' . implode('</span><span class="badge-sm badge-pill badge-secondary mr-1">', $res) . "</span>": "";
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
        $type = 'edit';

        $selectOptions = [];
        $selectOptions['enable-select2'] = true;
        $selectOptions['dropdown-auto-width'] = true;
        $selectOptions['minimum-results-for-search'] = 10;
        $selectOptions['multiple'] = true;
        $selectOptions['placeholder'] = $this->getNullLabel();
        $selectOptions = array_merge($selectOptions, $this->m_select2Options['edit']);

        $data = '';
        foreach ($selectOptions as $k => $v) {
            $data .= ' data-'.$k.'="'.htmlspecialchars($v).'"';
        }

        if($this->getCssStyle($type, 'width') === null && $this->getCssStyle($type, 'min-width') === null) {
            $this->setCssStyle($type, 'min-width', '220px');
        }

        $style = $styles = '';
        foreach($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }
        if($style != ''){
            $styles = ' style="'.$style.'"';
        }

        $onchange = '';
        if (Tools::count($this->m_onchangecode)) {
            $onchange = ' onChange="'.$this->getHtmlId($fieldprefix).'_onChange(this)"';
            $this->_renderChangeHandler($fieldprefix);
        }

        $result = '<select multiple id="'.$id.'" name="'.$name.'[]" '.$this->getCSSClassAttribute().'" '.$onchange.$data.$styles.'>';

        $values = $this->getValues();
        if (!is_array($record[$this->fieldName()])) {
            $recordvalue = $this->db2value($record);
        } else {
            $recordvalue = $record[$this->fieldName()];
        }

        for ($i = 0; $i < Tools::count($values); ++$i) {
            // If the current value is selected or occurs in the record
            $sel = (Tools::atk_in_array($values[$i], $recordvalue)) ? 'selected' : '';

            $result .= '<option value="'.$values[$i].'" '.$sel.'>'.$this->_translateValue($values[$i], $record);
        }

        $result .= '</select>';
        $result .= "<script>ATK.Tools.enableSelect2ForSelect('#$id');</script>";

        return $result;
    }


    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        $searchcondition = '';
        if (is_array($value) && $value[0] != '' && Tools::count($value) > 0) {
            $searchcondition = [];

            if (in_array('__NONE__', $value)) {
                 unset($value[array_search('__NONE__', $value)]);
                $searchcondition[] = $query->nullCondition($table.'.'.$this->fieldName(), true);
            }

            foreach ($value as $str) {
                $searchcondition[] = $query->substringCondition($table.'.'.$this->fieldName(), $this->escapeSQL($str));
            }
            $searchcondition = implode(' OR ', $searchcondition);
        }

        return $searchcondition;
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return string The 'text' type of the database field for this
     *                attribute.
     */
    public function dbFieldType()
    {
        return 'text';
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
        return ['substring'];
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
