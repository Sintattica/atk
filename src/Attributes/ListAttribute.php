<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Query;

/**
 * The ListAttribute class represents an attribute of a node
 * that has a selectbox to select from predefined values.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ListAttribute extends Attribute
{
    /**
     * Do not translate the options.
     */
    const int AF_NO_TRANSLATION = 33554432;

    /**
     * Do not add a default null option.
     */
    const int AF_LIST_NO_OBLIGATORY_NULL_ITEM = 67108864;

    /**
     * Do not add null option ever.
     */
    const int AF_LIST_NO_NULL_ITEM = 134217728;

    /**
     * Add a default null option to obligatory items.
     */
    const int AF_LIST_OBLIGATORY_NULL_ITEM = 268435456;

    /*
     * Array with options for Listbox
     */
    public array $m_options = [];

    /*
     * Array with values for Listbox
     */
    public array $m_values = [];

    /*
     * Array for fast lookup of what value belongs to what option.
     */
    public array $m_lookup = [];

    /*
     * Array which holds the options, values and lookup array in cache
     */
    public array $m_types = [];

    /*
     * Attribute that is to be selected
     */
    public $m_selected;

    /*
     * Value that is used when list is empty, normally empty
     */
    public string $m_emptyvalue = '';

    public $m_onchangehandler_init = "newvalue = el.options[el.selectedIndex] ? el.options[el.selectedIndex].value : null;\n";

    /**
     * When autosearch is set to true, this attribute will automatically submit
     * the search form onchange. This will only happen in the admin action.
     */
    protected bool $m_autoSearch = false;

    /** @var array{normal: bool, extended: bool} */
    protected array $m_multipleSearch = [
        'normal' => false,
        'extended' => true,
    ];

    protected string $nullLabel = '';

    /**
     * Constructor.
     *
     * Warning: very old versions of this attribute supported passing the
     * parameters in a different order: $name, $flags, $optionArray.
     * This order used to be supported even when the new order was
     * implemented, but it has now been removed. Keep this in mind
     * when upgrading from a very old ATK version (pre ATK4).
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param array $optionArray Array with options
     * @param array $valueArray Array with values. If you don't use this parameter,
     *                            values are assumed to be the same as the options.
     */
    public function __construct($name, $flags, $optionArray, $valueArray = null)
    {
        if (!is_array($valueArray) || Tools::count($valueArray) === 0) {
            $valueArray = $optionArray;
        }

        // If all values are numeric, we can use a numeric field to store the selected
        // value.
        $this->m_dbfieldtype = 'number';
        for ($i = 0, $_i = Tools::count($valueArray); $i < $_i && $this->m_dbfieldtype == 'number'; ++$i) {
            if (!is_numeric($valueArray[$i])) {
                $this->m_dbfieldtype = 'string';
            }
            // if one of the values is not a number, the fieldtype must be string, and
            // the loop is stopped.
        }

        // the max size we have is equal to the biggest value.
        $size = 0;
        for ($i = 0, $_i = Tools::count($valueArray); $i < $_i; ++$i) {
            $size = max($size, Tools::atk_strlen($valueArray[$i]));
        }
        if ($size > 0) {
            $this->setAttribSize($size);
        }

        parent::__construct($name, $flags);

        $this->setOptions($optionArray, $valueArray);
    }

    /**
     * Creates a lookup array to speedup translations.
     */
    public function createLookupArray(array $optionArray, array $valueArray): static
    {
        foreach ($optionArray as $id => $option) {
            $this->m_lookup[$valueArray[$id]] = $option;
        }

        return $this->setSingleType('lookup', $this->m_lookup);
    }

    /**
     * Get function to access the member variable for options.
     * For backwards compatibility we also check the old member variable m_options.
     *
     */
    public function getOptions(): array
    {
        if (!isset($this->m_types['options']) || Tools::count($this->m_types['options']) === 0) {
            return $this->m_options;
        }

        return $this->getSingleType('options');
    }

    /**
     * Get functions to access the member variable for values
     * For backwards compatibility we also check the old member variable m_values.
     *
     */
    public function getValues(): array
    {
        if (!isset($this->m_types['values']) || Tools::count($this->m_types['values']) === 0) {
            return $this->m_values;
        }

        return $this->getSingleType('values');
    }

    /**
     * Get functions to access the member variable for lookup.
     * For backwards compatibility we also check the old member variable m_lookup.
     *
     */
    public function getLookup(): array
    {
        if (!isset($this->m_types['lookup']) || Tools::count($this->m_types['lookup']) === 0) {
            return $this->m_lookup;
        }

        return $this->getSingleType('lookup');
    }

    /**
     * Returns one of the following arrays
     * options => optionarray
     * values => valuearray
     * lookup => lookuparray.
     *
     * @param string $type ("options", "values" or "lookup")
     *
     * @return array with options, values or lookup
     */
    private function getSingleType(string $type): array
    {
        return $this->m_types[$type];
    }

    /**
     * Set's one of the following arrays
     * options => optionarray
     * values => valuearray
     * lookup => lookuparray.
     *
     * @param string $type ("options", "values" or "lookup)
     * @param array $value
     * @return ListAttribute
     */
    private function setSingleType(string $type, array $value): static
    {
        $this->m_types[$type] = $value;
        return $this;
    }

    /**
     * Display's text version of Record.
     *
     * @param array<string, mixed> $record
     * @param string $mode
     *
     * @return string
     */
    public function display(array $record, string $mode): string
    {
        return isset($record[$this->fieldName()]) ? $this->translateValue($record[$this->fieldName()], $record) : '';
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
     * @return string piece of html code with a checkbox
     */
    public function edit($record, $fieldprefix, $mode)
    {

        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);
        $hasNullOption = $this->hasNullOption();

        $selectOptions = [];
        $selectOptions['enable-select2'] = true;
        $selectOptions['dropdown-auto-width'] = true;
        $selectOptions['minimum-results-for-search'] = 10;

        $nullLabel = '';
        if ($hasNullOption) {
            $nullLabel = $this->getNullLabel();
            $selectOptions['with-empty-value'] = $this->getEmptyValue();
        }

        $selectOptions = array_merge($selectOptions, $this->m_select2Options['edit']);

        $onchange = '';
        if (Tools::count($this->m_onchangecode)) {
            $onchange = ' onChange="' . $this->getHtmlId($fieldprefix) . '_onChange(this)"';
            $this->_renderChangeHandler($fieldprefix);
        }

        $data = '';
        foreach ($selectOptions as $k => $v) {
            $data .= ' data-' . $k . '="' . htmlspecialchars($v) . '"';
        }

        $style = $styles = '';
        foreach ($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }
        if ($style != '') {
            $styles = 'style="' . $style . '"';
        }

        $result = '<select id="' . $id . '" name="' . $name . '" ' . $this->getCSSClassAttribute() . $onchange . $data . $styles . '>';

        if ($hasNullOption) {
            $result .= '<option value="' . $this->getEmptyValue() . '">' . $nullLabel . '</option>';
        }

        $values = $this->getValues();
        $recvalue = Tools::atkArrayNvl($record, $this->fieldName());

        for ($i = 0; $i < Tools::count($values); ++$i) {
            $sel = '';
            // If the current value is selected or occurs in the record
            if ((!is_null($this->m_selected) && $values[$i] == $this->m_selected) || (is_null($this->m_selected) && $values[$i] == $recvalue && $recvalue !== '')) {
                $sel = 'selected';
            }

            $result .= '<option value="' . $values[$i] . '" ' . $sel . '>' . $this->translateValue($values[$i], $record);
        }

        $result .= '</select>';
        $result .= "<script>ATK.Tools.enableSelect2ForSelect('#$id');</script>";

        return $result;
    }

    /**
     * Translates the database value.
     */
    protected function translateValue(mixed $value, ?array $record = null): string
    {
        $lookup = $this->getLookup();
        $res = '';
        if (isset($lookup[$value])) {
            if ($this->hasFlag(self::AF_NO_TRANSLATION)) {
                $res = $lookup[$value];
            } else {
                $res = $this->text(array($this->fieldName() . '_' . $lookup[$value], $lookup[$value]));
            }
        }

        return $res;
    }

    public function getNullLabel(): string
    {
        if ($this->hasNullOption()) {
            if ($this->nullLabel) {
                // specific null label
                return $this->nullLabel;
            }

            // use a different (more descriptive) text for obligatory items
            $textKey = $this->hasFlag(self::AF_OBLIGATORY) ? 'list_null_value_obligatory' : 'list_null_value';

            return htmlentities($this->text([$this->fieldName() . '_' . $textKey, $textKey]));
        }

        return '';
    }

    public function setNullLabel(string $nullLabel): static
    {
        $this->nullLabel = $nullLabel;
        return $this;
    }

    public function hasNullOption(): bool
    {
        if (!$this->hasFlag(self::AF_LIST_NO_NULL_ITEM)) {
            if (!$this->hasFlag(self::AF_OBLIGATORY)
                || ($this->hasFlag(self::AF_LIST_OBLIGATORY_NULL_ITEM) || (Config::getGlobal('list_obligatory_null_item') && !$this->hasFlag(self::AF_LIST_NO_OBLIGATORY_NULL_ITEM)))
            ) {
                return true;
            }
        }

        return false;
    }

    /**
     * Returns a piece of html code that can be used to get search terms input
     * from the user.
     *
     * The framework calls this method to display the searchbox
     * in the search bar of the recordlist, and to display a more extensive
     * search in the 'extended' search screen.
     *
     * @param array $record Array with values
     * @param bool $extended if set to false, a simple search input is
     *                            returned for use in the searchbar of the
     *                            recordlist. If set to true, a more extended
     *                            search may be returned for the 'extended'
     *                            search page. The Attribute does not
     *                            make a difference for $extended is true, but
     *                            derived attributes may reimplement this.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     * @param DataGrid|null $grid
     *
     * @return string A piece of html-code with a checkbox
     * @todo Configurable rows
     */
    public function search($record, $extended = false, $fieldprefix = '', DataGrid $grid = null): string
    {
        $values = $this->getValues();
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getSearchFieldName($fieldprefix);
        $isMultiple = $this->isMultipleSearch($extended);
        $class = $this->getCSSClassAttribute();
        $style = '';
        $type = $extended ? 'extended_search' : 'search';

        $selectOptions = [];
        $selectOptions['enable-select2'] = true;
        $selectOptions['dropdown-auto-width'] = true;
        $selectOptions['minimum-results-for-search'] = 10;
        $selectOptions['with-empty-value'] = '';
        if ($isMultiple) {
            $selectOptions['allow-clear'] = true;
            $selectOptions['placeholder'] = Tools::atktext('search_all');
        }
        $selectOptions = array_merge($selectOptions, $this->m_select2Options['search']);

        foreach ($this->getCssStyles($type) as $k => $v) {
            $style .= "$k:$v;";
        }

        $result = '<select ' . ($isMultiple ? 'multiple' : '') . ' ' . $class . ' id="' . $id . '" name="' . $name . '[]"';
        foreach ($selectOptions as $k => $v) {
            $result .= ' data-' . $k . '="' . htmlspecialchars($v) . '"';
        }
        $result .= $style != '' ? ' style="' . $style . '"' : '';
        $result .= ' >';

        $selValues = $record[$this->fieldName()] ?? null;
        if (!is_array($selValues)) {
            $selValues = [$selValues];
        }

        if (in_array('', $selValues)) {
            $selValues = [''];
        }

        $selected = (!$isMultiple && $selValues[0] == '') ? ' selected' : '';
        $option = Tools::atktext('search_all');
        $result .= sprintf('<option value=""%s>%s</option>', $selected, $option);

        // "none" option
        if (!$this->hasFlag(self::AF_OBLIGATORY) && !$this->hasFlag(self::AF_LIST_NO_NULL_ITEM)) {
            $selected = Tools::atk_in_array('__NONE__', $selValues) ? ' selected' : '';
            $option = $this->nullLabel ?: Tools::atktext('search_none');
            $result .= sprintf('<option value="__NONE__"%s>%s</option>', $selected, $option);
        }

        // normal options
        foreach ($values as $value) {
            $selected = Tools::atk_in_array(((string)$value), $selValues, true) ? ' selected' : '';
            $option = $this->translateValue($value, $record);
            $result .= sprintf('<option value="%s"%s>%s</option>', $value, $selected, $option);
        }

        $result .= '</select>';
        $result .= "<script>ATK.Tools.enableSelect2ForSelect('#$id');</script>";

        $onchange = '';

        // if is multiple, replace null selection with empty string
        if ($isMultiple) {
            $onchange .= <<<EOF
var s=jQuery(this), v = s.val();
if (v != null && v.length > 0) {
    var nv = jQuery.grep(v, function(value) {
        return value != '';
    });
    s.val(nv);
}
if(s.val() === null){
   s.val('');
};
s.trigger('change.select2');
EOF;
        }

        // if we use autosearch, register an onchange event that submits the grid
        if (!is_null($grid) && !$extended && $this->m_autoSearch) {
            $onchange .= $grid->getUpdateCall(array('atkstartat' => 0), [], 'ATK.DataGrid.extractSearchOverrides');
        }

        if ($onchange != '') {
            $this->getOwnerInstance()->getPage()->register_loadscript('jQuery("#' . $id . '").on("change", function(){' . $onchange . '})');
        }

        return $result;
    }

    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * @param Query $query The query object where the search condition should be placed on
     * @param string $table The name of the table in which this attribute
     *                           is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
     * @param string $fieldname
     *
     * @return string The searchcondition to use.
     */
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        // We only support 'exact' matches.
        // But you can select more than one value, which we search using the IN() statement,
        // which should work in any ansi compatible database.
        $searchcondition = '';
        if (is_array($value) && Tools::count($value) > 0 && $value[0] != '') { // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.

            if (Tools::count($value) == 1 && $value[0] != '') { // exactly one value
                if ($value[0] == '__NONE__') {
                    return $query->nullCondition($table . '.' . $this->fieldName(), true);
                } else {
                    return $query->exactCondition($table . '.' . $this->fieldName(), $this->escapeSQL($value[0]), $this->dbFieldType());
                }
            } elseif (Tools::count($value) > 1) { // search for more values
                if (in_array('__NONE__', $value)) {
                    unset($value[array_search('__NONE__', $value)]);

                    return sprintf('(%s OR %s)', $query->nullCondition($table . '.' . $this->fieldName(), true),
                        $table . '.' . $this->fieldName() . " IN ('" . implode("','", $value) . "')");
                } else {
                    return $table . '.' . $this->fieldName() . " IN ('" . implode("','", $value) . "')";
                }
            }
        }

        return $searchcondition;
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
        return ['exact'];
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return string The 'generic' type of the database field for this
     *                attribute.
     */
    public function dbFieldType()
    {
        // Fieldtype was determined in the constructor.
        return $this->m_dbfieldtype;
    }

    /**
     * Set autohide for the given attribute.
     */
    public function setAutoHide(string $attributeName, array $valuesToHide): static
    {
        $conditions = [];
        foreach ($valuesToHide as $value) {
            $conditions[] = "newvalue=='$value'";
        }
        $this->addOnChangeHandler('if (' . implode('||', $conditions) . ") ATK.Tools.hideAttrib('$attributeName'); else ATK.Tools.showAttrib('$attributeName');");
        return $this;
    }

    /**
     * When autosearch is set to true, this attribute will automatically submit
     * the search form onchange. This will only happen in the admin action.
     */
    public function setAutoSearch(bool $auto = false): static
    {
        $this->m_autoSearch = $auto;
        return $this;
    }

    /**
     * Sets the value for the empty entry in the list attribute
     * In normal cases you would just leave this empty, but certain cases
     * might demand you set a value.
     *
     * @param string $value the value we set for empty value
     */
    public function setEmptyValue(string $value): static
    {
        $this->m_emptyvalue = $value;
        return $this;
    }

    /**
     * Gets the value for the empty entry in the list attribute.
     */
    public function getEmptyValue(): string
    {
        return $this->m_emptyvalue;
    }

    /**
     * Convert a String representation into an internal value.
     *
     * This implementation search for the value first in the valueArray, then in the optionArray
     * All other values are converted to the first of the valueArray
     *
     * @param string $stringvalue The value to parse.
     *
     * @return mixed Internal value (from valueArray)
     */
    public function parseStringValue($stringvalue)
    {
        $values = $this->getValues();
        foreach ($values as $value) {
            if (strtolower($stringvalue) == strtolower($value)) {
                return $value;
            }
        }

        $i = 0;
        $options = $this->getOptions();
        foreach ($options as $option) {
            if (strtolower($stringvalue) == strtolower($option)) {
                return $values[$i];
            }

            if (strtolower(Tools::atktext($stringvalue)) == strtolower($option)) {
                return $values[$i];
            }
            ++$i;
        }

        return $values[0];
    }

    /**
     * Add option/value to dropdown.
     */
    public function addOption(string $option, mixed $value = ''): static
    {
        if ($value != 0 && empty($value)) {
            $value = $option;
        }
        $currentOptions = $this->getSingleType('options');
        $currentOptions[] = $option;
        $this->setSingleType('options', $currentOptions);

        $currentValues = $this->getSingleType('values');
        $currentValues[] = $value;
        $this->setSingleType('values', $currentValues);

        return $this->createLookupArray($currentOptions, $currentValues);
    }

    /**
     * Remove option from dropdown.
     */
    public function removeOption(string $option): static
    {
        $currentOptions = $this->getSingleType('options');
        $currentValues = $this->getSingleType('values');

        $index = array_search($option, $currentOptions);

        array_splice($currentOptions, $index, 1); // remove option
        array_splice($currentValues, $index, 1);  // remove value

        $this->setSingleType('options', $currentOptions);
        $this->setSingleType('values', $currentValues);

        return $this;
    }

    /**
     * Set the option and value array.
     */
    public function setOptions(array $optionArray, array $valueArray = null): static
    {
        // m_options and m_values array are still here for backwardscompatibility
        $this->m_options = $optionArray;
        $this->setSingleType('options', $optionArray);

        if (!is_array($valueArray) || Tools::count($valueArray) === 0) {
            $valueArray = $optionArray;
        }

        $this->m_values = $valueArray;
        $this->setSingleType('values', $valueArray);

        return $this->createLookupArray($optionArray, $valueArray);
    }

    /**
     * Remove value from dropdown.
     */
    public function removeValue(mixed $value): static
    {
        $currentOptions = $this->getSingleType('options');
        $currentValues = $this->getSingleType('values');

        $v = array_search($value, $currentValues);

        array_splice($currentOptions, $v, 1); // remove option
        array_splice($currentValues, $v, 1);  // remove value

        $this->setSingleType('options', $currentOptions);
        $this->setSingleType('values', $currentValues);

        return $this;
    }

    public function setMultipleSearch(bool $normal = true, bool $extended = true): static
    {
        $this->m_multipleSearch = [
            'normal' => $normal,
            'extended' => $extended,
        ];

        return $this;
    }

    /**
     * @return array{normal: bool, extended: bool}
     */
    public function getMultipleSearch(): array
    {
        return $this->m_multipleSearch;
    }

    public function isMultipleSearch(bool $extended): bool
    {
        $ms = $this->getMultipleSearch();

        return $ms[$extended ? 'extended' : 'normal'];
    }
}
