<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\AdminLte\UIStateColors;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Query;

/**
 * Class MultiListAttribute
 *
 * Use this class instead of MultiSelectAttribute and MultiSelectListAttribute.
 */
class MultiListAttribute extends ListAttribute
{
    public const string FIELD_SEPARATOR_DEFAULT = '|';
    public const string DISPLAY_SEPARATOR_TAG = 'tag-separator';
    public const string DISPLAY_SEPARATOR_COMMA = ', ';
    public const string DISPLAY_SEPARATOR_DEFAULT = self::DISPLAY_SEPARATOR_TAG;
    public const string EXPORT_SEPARATOR_DEFAULT = '';

    protected string $m_fieldSeparator = self::FIELD_SEPARATOR_DEFAULT;
    private string $m_displaySeparator = self::DISPLAY_SEPARATOR_DEFAULT;
    private string $m_exportSeparator = self::EXPORT_SEPARATOR_DEFAULT;
    /** @var string The color of badge pills */
    private string $tagState = UIStateColors::STATE_SECONDARY;

    function __construct($name, $flags, $optionArray, $valueArray = null)
    {
        parent::__construct($name, $flags, $optionArray, $valueArray);

        // compute the attribute size
        $size = 0;
        $valueArray = $this->getValues();
        for ($i = 0, $_i = Tools::count($valueArray); $i < $_i; ++$i) {
            $size += (Tools::atk_strlen($valueArray[$i]) + 1); // 1 extra for the '|' symbol
        }
        if ($size > 0) {
            $this->setAttribSize($size);
        }

        $this->setSelect2Options(['close-on-select' => false], ['edit']);
    }

    public function display(array $record, string $mode): string
    {
        $values = $record[$this->fieldName()];
        $valuesTranslated = [];

        if ($values) {
            if (is_string($values)) {
                // $values is a string like |xxx|xxx|...| instead of an array
                $values = $this->db2value($record);
            }
            for ($i = 0; $i < Tools::count($values); $i++) {
                if ($r = $this->translateValue($values[$i], $record)) {
                    $valuesTranslated[] = $r;
                }
            }
        }

        if ($mode === 'csv') {
            // export separator
            $separator = $this->m_exportSeparator ?: $this->m_displaySeparator;

        } else {
            // badge pill display
            if ($this->m_displaySeparator === self::DISPLAY_SEPARATOR_TAG) {
                return Tools::formatTagList($valuesTranslated, $this->tagState);
            }

            // comma separator
            $separator = $this->m_displaySeparator;
        }

        return implode($separator, $valuesTranslated);
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
            $data .= ' data-' . $k . '="' . htmlspecialchars($v) . '"';
        }

        if ($this->getCssStyle($type, 'width') === null && $this->getCssStyle($type, 'min-width') === null) {
            $this->setCssStyle($type, 'min-width', '220px');
        }

        $style = $styles = '';
        foreach ($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }
        if ($style != '') {
            $styles = ' style="' . $style . '"';
        }

        $onchange = '';
        if (Tools::count($this->m_onchangecode)) {
            $onchange = ' onChange="' . $this->getHtmlId($fieldprefix) . '_onChange(this)"';
            $this->_renderChangeHandler($fieldprefix);
        }

        $result = '<select multiple id="' . $id . '" name="' . $name . '[]" ' . $this->getCSSClassAttribute() . '" ' . $onchange . $data . $styles . '>';

        $values = $this->getValues();
        if (isset($record[$this->fieldName()])) {
            if (!is_array($record[$this->fieldName()])) {
                $recordvalue = $this->db2value($record);
            } else {
                $recordvalue = $record[$this->fieldName()];
            }
        } else {
            $recordvalue = null;
        }

        for ($i = 0; $i < Tools::count($values); ++$i) {
            // If the current value is selected or occurs in the record
            $sel = (Tools::atk_in_array($values[$i], $recordvalue)) ? 'selected' : '';

            $result .= '<option value="' . $values[$i] . '" ' . $sel . '>' . $this->translateValue($values[$i], $record);
        }

        $result .= '</select>';
        $result .= "<script>ATK.Tools.enableSelect2ForSelect('#$id');</script>";

        return $result;
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
        $valuesRecord = $record[$this->fieldName()];
        if (is_string($valuesRecord)) {
            // $valuesRecord is a string like |xxx|xxx|...| instead of an array
            $valuesRecord = $this->db2value($record);
        }
        if (is_array($valuesRecord)) {
            $valuesAll = $this->getValues();
            for ($i = 0; $i < Tools::count($valuesAll); ++$i) {
                if (in_array($valuesAll[$i], $valuesRecord)) {
                    $result .= '<input type="hidden" name="' . $this->getHtmlName($fieldprefix) . '[]" value="' . $valuesAll[$i] . '">';
                }
            }
        } else {
            parent::hide($record, $fieldprefix, $mode);
        }

        return $result;
    }

    /**
     * Stores the values like |value1|value2|...
     * @param array $record
     * @return string
     */
    function value2db($record)
    {
        if (is_array($record[$this->fieldName()]) && Tools::count($record[$this->fieldName()]) >= 1) {
            return $this->escapeSQL($this->m_fieldSeparator . implode($this->m_fieldSeparator, $record[$this->fieldName()]) . $this->m_fieldSeparator);
        }

        return '';
    }

    /**
     * Converts a database value to an internal value.
     *
     * @param array $record The database record that holds this attribute's value
     *
     * @return array The internal value
     */
    function db2value($record)
    {
        if (isset($record[$this->fieldName()]) && $record[$this->fieldName()] !== '') {
            // remove initial and final m_fieldSeparator
            $value = substr($record[$this->fieldName()], 1, strlen($record[$this->fieldName()]) - 2);
            // transform in array
            return explode($this->m_fieldSeparator, $value);
        }

        return [];
    }

    function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        /**
         * MultiListAttribute has only 1 searchmode and that is "substring".
         * @see getSearchModes()
         */
        $searchconditions = [];

        if (is_array($value) && $value[0] != "" && count($value) > 0) {
            // includes the separators in the value to search, in this way the search is more secure
            if (in_array('__NONE__', $value)) {
                return $query->nullCondition($table . '.' . $this->fieldName(), true);
            }

            foreach ($value as $str) {
                $searchconditions[] = $query->substringCondition($table . '.' . $this->fieldName(), $this->escapeSQL($this->m_fieldSeparator . $str . $this->m_fieldSeparator));
            }
        }

        return count($searchconditions) ? '(' . implode(' OR ', $searchconditions) . ')' : '';
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
     * Check if a record has an empty value for this attribute.
     *
     * @param array $record The record that holds this attribute's value.
     *
     * @return bool
     */
    public function isEmpty($record)
    {
        if (!isset($record[$this->fieldName()])) {
            return true;
        }

        $values = $record[$this->fieldName()];

        if (is_string($values)) {
            // $values is a string like |xxx|xxx|...| instead of an array
            $values = $this->db2value($record);
        }
        return !is_array($values) || Tools::count($values) === 0;
    }

    public function getFieldSeparator(): string
    {
        return $this->m_fieldSeparator;
    }

    public function setFieldSeparator(string $m_fieldSeparator): static
    {
        $this->m_fieldSeparator = $m_fieldSeparator;
        return $this;
    }

    public function getDisplaySeparator(): string
    {
        return $this->m_displaySeparator;
    }

    public function setDisplaySeparator($separator): static
    {
        $this->m_displaySeparator = $separator;
        return $this;
    }

    public function getExportSeparator(): string
    {
        return $this->m_exportSeparator;
    }

    public function setExportSeparator($separator): static
    {
        $this->m_exportSeparator = $separator;
        return $this;
    }

    public function getTagState(): string
    {
        return $this->tagState;
    }

    public function setTagState(string $tagState): static
    {
        $this->tagState = $tagState;
        return $this;
    }
}
