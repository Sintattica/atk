<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Ui\Page;
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
    const AF_NO_TRANSLATION = 33554432;

    /**
     * Do not add a default null option.
     */
    const AF_LIST_NO_OBLIGATORY_NULL_ITEM = 67108864;

    /**
     * Do not add null option ever.
     */
    const AF_LIST_NO_NULL_ITEM = 134217728;

    /**
     * Add a default null option to obligatory items.
     */
    const AF_LIST_OBLIGATORY_NULL_ITEM = 268435456;

    /*
     * Array with options for Listbox
     */
    public $m_options = array();

    /*
     * Array with values for Listbox
     */
    public $m_values = array();

    /*
     * Array for fast lookup of what value belongs to what option.
     */
    public $m_lookup = array();

    /*
     * Array which holds the options,values and lookup array in cache
     */
    public $m_types = array();

    /*
     * Attribute that is to be selected
     */
    public $m_selected;

    /*
     * Value that is used when list is empty, normally empty
     */
    public $m_emptyvalue;

    /*
     * The width of the dropdown list in pixels
     * @var int
     */
    public $m_width;
    public $m_onchangehandler_init = "newvalue = el.options[el.selectedIndex].value;\n";

    /**
     * When autosearch is set to true, this attribute will automatically submit
     * the search form onchange. This will only happen in the admin action.
     *
     * @var bool
     */
    protected $m_autoSearch = false;


    /**
     * Use Quick[select] plugin (http://eggboxio.github.io/quick-select/) to expand the selection as a series of buttons.
     * Comes handy when there are only a few options.
     * (see expandAsButtons function).
     *
     * @var bool|array False (disabled), True (enabled with default options), Array of options
     */
    private $m_expandAsButtons = false;

    protected $m_multipleSearch = [
        'normal' => false,
        'extended' => true,
    ];

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
    public function __construct($name, $flags = 0, $optionArray, $valueArray = null)
    {
        if (!is_array($valueArray) || count($valueArray) == 0) {
            $valueArray = $optionArray;
        }

        // If all values are numeric, we can use a numeric field to store the selected
        // value.
        $this->m_dbfieldtype = 'number';
        for ($i = 0, $_i = count($valueArray); $i < $_i && $this->m_dbfieldtype == 'number'; ++$i) {
            if (!is_numeric($valueArray[$i])) {
                $this->m_dbfieldtype = 'string';
            }
            // if one of the values is not a number, the fieldtype must be string, and
            // the loop is stopped.
        }

        // the max size we have is equal to the biggest value.
        $size = 0;
        for ($i = 0, $_i = count($valueArray); $i < $_i; ++$i) {
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
     *
     * @param array $optionArray
     * @param array $valueArray
     */
    public function createLookupArray($optionArray, $valueArray)
    {
        foreach ($optionArray as $id => $option) {
            $this->m_lookup[$valueArray[$id]] = $option;
        }

        $this->_set('lookup', $this->m_lookup);
    }

    /**
     * Get function to access the member variable for options.
     * For backwards compatibility we also check the old member variable m_options.
     *
     * @param array $rec The record
     */
    public function getOptions($rec = null)
    {
        if (!isset($this->m_types['options']) || count($this->m_types['options']) == 0) {
            return $this->m_options;
        }

        return $this->_get('options', $rec);
    }

    /**
     * Get functions to access the member variable for values
     * For backwards compatibility we also check the old member variable m_values.
     *
     * @param array $rec The record
     */
    public function getValues($rec = null)
    {
        if (!isset($this->m_types['values']) || count($this->m_types['values']) == 0) {
            return $this->m_values;
        }

        return $this->_get('values', $rec);
    }

    /**
     * Get functions to access the member variable for lookup.
     * For backwards compatibility we also check the old member variable m_lookup.
     *
     * @param array $rec The record
     */
    public function getLookup($rec = null)
    {
        if (!isset($this->m_types['lookup']) || count($this->m_types['lookup']) == 0) {
            return $this->m_lookup;
        }

        return $this->_get('lookup', $rec);
    }

    /**
     * Returns one of the following arrays
     * options => optionarray
     * values => valuearray
     * lookup => lookuparray.
     *
     * @param string $type ("options", "values" or "lookup")
     * @param array $rec The record
     *
     * @return array with options, values or lookup
     */
    public function _get($type, $rec = null)
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
     *
     * @return true
     */
    public function _set($type, $value)
    {
        $this->m_types[$type] = $value;

        return true;
    }

    /**
     * Display's text version of Record.
     *
     * @param array $record
     * @param string $mode
     *
     * @return string of $record
     */
    public function display($record, $mode)
    {
        return $this->_translateValue($record[$this->fieldName()], $record);
    }

    /**
     * Translates the database value.
     *
     * @param string $value
     * @param array $rec The record
     *
     * @return string
     */
    public function _translateValue($value, $rec = null)
    {
        $lookup = $this->getLookup($rec);
        $res = '';
        if (isset($lookup[$value])) {
            if ($this->hasFlag(self::AF_NO_TRANSLATION)) {
                $res = $lookup[$value];
            } else {
                $res = $this->text(array($this->fieldName().'_'.$lookup[$value], $lookup[$value]));
            }
        }

        return $res;
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
        $this->registerJavaScriptObservers($id);

        $selectOptions = [];

        $onchange = '';
        if (count($this->m_onchangecode)) {
            $onchange = $id.'_onChange(this);';
            $this->_renderChangeHandler($fieldprefix);
        }

        $result = '<select id="'.$id.'" name="'.$id.'" '.$this->getCSSClassAttribute('form-control').'" '.$onchange.'>';

        $nullLabel = '';
        if ($hasNullOption = $this->hasNullOption()) {
            $nullLabel = $this->getNullLabel();
            $result .= '<option value="'.$this->m_emptyvalue.'">'.$nullLabel.'</option>';
        }

        $values = $this->getValues($record);
        $recvalue = Tools::atkArrayNvl($record, $this->fieldName());

        for ($i = 0; $i < count($values); ++$i) {
            $sel = '';
            // If the current value is selected or occurs in the record
            if ((!is_null($this->m_selected) && $values[$i] == $this->m_selected) || (is_null($this->m_selected) && $values[$i] == $recvalue && $recvalue !== '')) {
                $sel = 'selected';
            }

            $result .= '<option value="'.$values[$i].'" '.$sel.'>'.$this->_translateValue($values[$i], $record);
        }

        $result .= '</select>';

        if ($this->m_expandAsButtons) {
            // use Quick[select] plugin to expand the selection as a series of buttons
            $page = $this->m_ownerInstance ? $this->m_ownerInstance->getPage() : Page::getInstance();
            $page->register_script(Config::getGlobal('assets_url').'javascript/quickselect/jquery.quickselect.min.js');
            $page->register_style(Config::getGlobal('assets_url').'javascript/quickselect/quickselect.css');
            $options = json_encode($this->m_expandAsButtons);
            $result .= "<script>jQuery('#$id').quickselect($options);</script>";
        } else {

            if ($hasNullOption) {
                $selectOptions['allowClear'] = true;
                $selectOptions['placeholder'] = $nullLabel;
            }

            $selectOptions['dropdownAutoWidth'] = 'true';
            $selectOptions['minimumResultsForSearch'] = 10;

            if($this->m_width){
                $selectOptions['width'] = $this->m_width;
            }else{
                $selectOptions['width'] = 'auto';
            }


            $script = "jQuery('#$id').select2(".json_encode($selectOptions).")";
            if ($onchange != '') {
                $script .= '.on("change", function(){'.$onchange.'})';
            }
            $result .= '<script>'.$script.';</script>';
        }

        return $result;
    }

    /**
     * Enable Quick[select] plugin (http://eggboxio.github.io/quick-select/) to expand the selection as a series of buttons.
     *
     * @param array $options Quick[select] Options (or null for default options)
     */
    public function expandAsButtons($options = null)
    {
        if (!$options || !is_array($options)) {
            $options = array();
        }
        $defaultOptions = array(
            'activeButtonClass' => 'btn-primary atkdefaultbutton active',
            'buttonClass' => 'btn btn-default',
            'breakOutAll' => true,
            'wrapperClass' => 'btn-group',
        );

        $this->m_expandAsButtons = array_merge($defaultOptions, $options);
    }


    public function getNullLabel()
    {
        if ($this->hasNullOption()) {
            // use a different (more descriptive) text for obligatory items
            $text_key = $this->hasFlag(self::AF_OBLIGATORY) ? 'list_null_value_obligatory' : 'list_null_value';

            return htmlentities($this->text([$this->fieldName().'_'.$text_key, $text_key,]));
        }

        return '';
    }

    public function hasNullOption()
    {
        if (!$this->hasFlag(self::AF_LIST_NO_NULL_ITEM)) {
            if (!$this->hasFlag(self::AF_OBLIGATORY) || ($this->hasFlag(self::AF_LIST_OBLIGATORY_NULL_ITEM) || (Config::getGlobal('list_obligatory_null_item') && !$this->hasFlag(self::AF_LIST_NO_OBLIGATORY_NULL_ITEM)))) {
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
     * @todo Configurable rows
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
     *
     * @return string A piece of html-code with a checkbox
     *
     *
     */
    public function search($record = '', $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        $values = $this->getValues($record);
        $id = $this->getSearchFieldName($fieldprefix);

        $isMultiple = $this->isMultipleSearch($extended);

        $class = $this->getCSSClassAttribute(['form-control']);
        $result = '<select '.($isMultiple ? 'multiple' : '').' '.$class.' id="'.$id.'" name="'.$id.'[]">';


        $selValues = $record[$this->fieldName()];
        if (!is_array($selValues)) {
            $selValues = [$selValues];
        }

        if (in_array('', $selValues)) {
            $selValues = [''];
        }

        $notSelectFirst = false;
        $selected = (!$notSelectFirst && $selValues[0] == '') ? ' selected' : '';
        $option = Tools::atktext('search_all');
        $result .= sprintf('<option value=""%s>%s</option>', $selected, $option);

        // "none" option
        if (!$this->hasFlag(self::AF_OBLIGATORY) && !$this->hasFlag(self::AF_LIST_NO_NULL_ITEM)) {
            $selected = Tools::atk_in_array('__NONE__', $selValues) ? ' selected' : '';
            $option = Tools::atktext('search_none');
            $result .= sprintf('<option value="__NONE__"%s>%s</option>', $selected, $option);
        }

        // normal options
        foreach ($values as $value) {
            $selected = Tools::atk_in_array(((string)$value), $selValues, true) ? ' selected' : '';
            $option = $this->_translateValue($value, $record);
            $result .= sprintf('<option value="%s"%s>%s</option>', $value, $selected, $option);
        }

        $result .= '</select>';

        $selectOptions = [];
        $selectOptions['width'] = '100%';

        $script = "jQuery('#$id').select2(".json_encode($selectOptions).")";


        // if we use autosearch, register an onchange event that submits the grid
        if (!is_null($grid) && !$extended && $this->m_autoSearch) {
            $onchange = $grid->getUpdateCall(array('atkstartat' => 0), array(), 'ATK.DataGrid.extractSearchOverrides');
            $script .= '.on("change", function(){'.$onchange.'})';
        }

        $result .= '<script>'.$script.';</script>';

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
     *
     * @return string The searchcondition to use.
     */
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        // We only support 'exact' matches.
        // But you can select more than one value, which we search using the IN() statement,
        // which should work in any ansi compatible database.
        $searchcondition = '';
        if (is_array($value) && count($value) > 0 && $value[0] != '') { // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.

            if (count($value) == 1 && $value[0] != '') { // exactly one value
                if ($value[0] == '__NONE__') {
                    return $query->nullCondition($table.'.'.$this->fieldName(), true);
                } else {
                    return $query->exactCondition($table.'.'.$this->fieldName(), $this->escapeSQL($value[0]));
                }
            } elseif (count($value) > 1) { // search for more values
                if (in_array('__NONE__', $value)) {
                    unset($value[array_search('__NONE__', $value)]);

                    return sprintf('(%s OR %s)', $query->nullCondition($table.'.'.$this->fieldName(), true),
                        $table.'.'.$this->fieldName()." IN ('".implode("','", $value)."')");
                } else {
                    return $table.'.'.$this->fieldName()." IN ('".implode("','", $value)."')";
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
        return array('exact');
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
     *
     * @param string $attrib
     * @param array $valuearr
     */
    public function setAutoHide($attrib, $valuearr)
    {
        $conditions = array();
        foreach ($valuearr as $value) {
            $conditions[] = "newvalue=='$value'";
        }
        $this->addOnChangeHandler('if ('.implode('||', $conditions).") hideAttrib('$attrib'); else showAttrib('$attrib');");
    }

    /**
     * When autosearch is set to true, this attribute will automatically submit
     * the search form onchange. This will only happen in the admin action.
     *
     * @param bool $auto
     */
    public function setAutoSearch($auto = false)
    {
        $this->m_autoSearch = $auto;
    }

    /**
     * Sets the value for the empty entry in the list attribute
     * In normal cases you would just leave this empty, but certain cases
     * might demand you set a value.
     *
     * @param string $value the value we set for empty value
     */
    public function setEmptyValue($value)
    {
        $this->m_emptyvalue = $value;
    }

    /**
     * Gets the value for the empty entry in the list attribute.
     *
     * @return string
     */
    public function getEmptyValue()
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
     * Set the css width of the dropdown list
     *
     * @param string $width The width of the dropdown list in pixels
     */
    public function setWidth($width)
    {
        $this->m_width = $width;
    }

    /**
     * Gets the css width of the dropdown list in pixels.
     *
     * @return string The width of the dropdown list in pixels
     */
    public function getWidth()
    {
        return $this->m_width;
    }

    /**
     * Add option/value to dropdown.
     *
     * @param string $option
     * @param string $value
     */
    public function addOption($option, $value = '')
    {
        if ($value != 0 && empty($value)) {
            $value = $option;
        }
        $currentOptions = $this->_get('options');
        $currentOptions[] = $option;
        $this->_set('options', $currentOptions);

        $currentValues = $this->_get('values');
        $currentValues[] = $value;
        $this->_set('values', $currentValues);

        $this->createLookupArray($currentOptions, $currentValues);

        return $this;
    }

    /**
     * Remove option from dropdown.
     *
     * @param string $option
     */
    public function removeOption($option)
    {
        $currentOptions = $this->_get('options');
        $currentValues = $this->_get('values');

        $index = array_search($option, $currentOptions);

        array_splice($currentOptions, $index, 1); // remove option
        array_splice($currentValues, $index, 1);  // remove value

        $this->_set('options', $currentOptions);
        $this->_set('values', $currentValues);

        return $this;
    }

    /**
     * Set the option and value array.
     *
     * @param array $optionArray array with options
     * @param array $valueArray array with values
     *
     * @return object reference to this attribute
     */
    public function setOptions($optionArray, $valueArray)
    {
        // m_options and m_values array are still here for backwardscompatibility
        $this->m_options = $optionArray;
        $this->_set('options', $optionArray);
        $this->m_values = $valueArray;
        $this->_set('values', $valueArray);

        $this->createLookupArray($optionArray, $valueArray);

        return $this;
    }

    /**
     * Remove value from dropdown.
     *
     * @param string $value
     */
    public function removeValue($value)
    {
        $currentOptions = $this->_get('options');
        $currentValues = $this->_get('values');

        $v = array_search($value, $currentValues);

        array_splice($currentOptions, $v, 1); // remove option
        array_splice($currentValues, $v, 1);  // remove value

        $this->_set('options', $currentOptions);
        $this->_set('values', $currentValues);

        return $this;
    }

    /**
     * @param bool $normal
     * @param bool $extended
     * @return array $m_multipleSearch
     */
    public function setMultipleSearch($normal = true, $extended = true)
    {
        $this->m_multipleSearch = [
            'normal' => $normal,
            'extended' => $extended,
        ];

        return $this->m_multipleSearch;
    }

    public function getMultipleSearch()
    {
        return $this->m_multipleSearch;
    }

    /**
     * @param bool $extended
     * @return bool
     */
    public function isMultipleSearch($extended)
    {
        $ms = $this->getMultipleSearch();

        return $ms[$extended ? 'extended' : 'normal'];
    }
}
