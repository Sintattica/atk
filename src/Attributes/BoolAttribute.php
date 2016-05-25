<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Query;

/**
 * The atkBoolAttribute class represents an attribute of a node
 * that can either be true or false.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class BoolAttribute extends Attribute
{
    /**
     * Make bool attribute obligatory (normal self::AF_OBLIGATORY flag is always removed).
     */
    const AF_BOOL_OBLIGATORY = 33554432;

    /**
     * Show an extra label right next to the checkbox. ATK searches the language
     * file for the following variants <attribute>_label, <attribute> (next to
     * the module/node prefixes). Don't forget to add the self::AF_BLANK_LABEL flag
     * if you don't want to show the normal label.
     */
    const AF_BOOL_INLINE_LABEL = 67108864;

    /**
     * Display checkbox in view / list mode instead of "yes" or "no".
     */
    const AF_BOOL_DISPLAY_CHECKBOX = 134217728;

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param int $size Size for this attribute
     */
    public function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags);
        $this->setAttribSize(1);
        if ($this->hasFlag(self::AF_BOOL_OBLIGATORY)) {
            $this->addFlag(self::AF_OBLIGATORY);
        }
    }

    /**
     * Adds the self::AF_OBLIGATORY flag to the attribute.
     *
     * @param int $flags The flag to add to the attribute
     *
     * @return Attribute The instance of this Attribute
     */
    public function addFlag($flags)
    {
        // setting self::AF_OBLIGATORY has no use, so prevent setting it.
        if (Tools::hasFlag($flags, self::AF_OBLIGATORY)) {
            $flags &= (~self::AF_OBLIGATORY);
        }

        // except if someone really really really wants to show this attribute is obligatory
        if (Tools::hasFlag($flags, self::AF_BOOL_OBLIGATORY)) {
            $flags |= self::AF_OBLIGATORY;
        }

        return parent::addFlag($flags);
    }

    /**
     * Is empty?
     *
     * @param array $record
     *
     * @return bool empty?
     */
    public function isEmpty($record)
    {
        $empty = parent::isEmpty($record);

        // if bool_obligatory flag is set the value must be true else we treat this record as empty
        if ($this->hasFlag(self::AF_BOOL_OBLIGATORY) && !$record[$this->fieldName()]) {
            $empty = true;
        }

        return $empty;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string piece of html code with a checkbox
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $id = $this->getHtmlId($fieldprefix);
        $onchange = '';
        if (count($this->m_onchangecode)) {
            $onchange = 'onClick="'.$id.'_onChange(this);" ';
            $this->_renderChangeHandler($fieldprefix);
        }
        $checked = '';
        if (isset($record[$this->fieldName()]) && $record[$this->fieldName()] > 0) {
            $checked = 'checked';
        }

        $result = '<input type="checkbox" id="'.$id.'" name="'.$id.'" value="1" '.$onchange.$checked.' '.$this->getCSSClassAttribute('atkcheckbox').' />';

        if ($this->hasFlag(self::AF_BOOL_INLINE_LABEL)) {
            $result .= '&nbsp;<label for="'.$id.'">'.$this->text(array(
                    $this->fieldName().'_label',
                    parent::label(),
                )).'</label>';
        }
        
        return $result;
    }

    /**
     * Get the value if it exits, otherwise return 0.
     *
     * @param array $rec Array with values
     *
     * @return int
     */
    public function value2db($rec)
    {
        return isset($rec[$this->fieldName()]) ? (int)$rec[$this->fieldName()] : 0;
    }

    /**
     * Returns a piece of html code that can be used in a form to search for values.
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
     * @return string piece of html code with a checkbox
     */
    public function search($record, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        $result = '<select name="'.$this->getSearchFieldName($fieldprefix).'" class="form-control">';
        $result .= '<option value="">'.Tools::atktext('search_all', 'atk').'</option>';
        $result .= '<option value="0" ';
        if (!empty($record[$this->fieldName()]) && $record[$this->fieldName()] === '0') {
            $result .= 'selected';
        }
        $result .= '>'.Tools::atktext('no', 'atk').'</option>';
        $result .= '<option value="1" ';
        if (!empty($record[$this->fieldName()]) && $record[$this->fieldName()] === '1') {
            $result .= 'selected';
        }
        $result .= '>'.Tools::atktext('yes', 'atk').'</option>';
        $result .= '</select>';

        return $result;
    }

    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        if (is_array($value)) {
            $value = $value[$this->fieldName()];
        }
        if (isset($value)) {
            return $query->exactCondition($table.'.'.$this->fieldName(), $this->escapeSQL($value));
        }
    }

    /**
     * Returns a displayable string for this value.
     *
     * @param array $record Array with boolean field
     *
     * @return yes or no
     */
    public function display($record, $mode)
    {
        if ($this->hasFlag(self::AF_BOOL_DISPLAY_CHECKBOX)) {
            return '
    		  <div align="center">
    		    <input type="checkbox" disabled="disabled" '.($record[$this->fieldName()] ? 'checked="checked"' : '').' />
    		  </div>
    		';
        } else {
            return $this->text($record[$this->fieldName()] ? 'yes' : 'no');
        }
    }

    /**
     * Get the HTML label of the attribute.
     *
     * The difference with the label() method is that the label method always
     * returns the HTML label, while the getLabel() method is 'smart', by
     * taking the self::AF_NOLABEL and self::AF_BLANKLABEL flags into account.
     *
     * @param array $record The record holding the value for this attribute.
     * @param string $mode The mode ("add", "edit" or "view")
     *
     * @return string The HTML compatible label for this attribute, or an
     *                empty string if the label should be blank, or NULL if no
     *                label at all should be displayed.
     */
    public function getLabel($record = array(), $mode = '')
    {
        if ($mode == 'view' && $this->hasFlag(self::AF_BLANK_LABEL | self::AF_BOOL_INLINE_LABEL)) {
            return $this->label();
        } else {
            return parent::getLabel($record);
        }
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     *
     * @return string The internal value
     */
    public function fetchValue($postvars)
    {
        if (is_array($postvars) && isset($postvars[$this->fieldName()])) {
            return $postvars[$this->fieldName()];
        } else {
            return 0;
        }
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
        return 'number';
    }

    /**
     * Return the label of the field.
     *
     * @param array $record The record that holds the value of this attribute
     *
     * @return The label for this attribute
     */
    /*
    function label($record = array())
    {
        $label_txt = parent::label($record);
        return '<label for="' . $this->m_name . '">' . $label_txt . '</label>';
    }
    */

    /**
     * Convert a String representation into an internal value.
     *
     * This implementation converts 'y/j/yes/on/true/1/*' to 1
     * All other values are converted to 0
     *
     * @param string $stringvalue The value to parse.
     *
     * @return bool Internal value
     */
    public function parseStringValue($stringvalue)
    {
        if (in_array(strtolower($stringvalue), array('y', 'j', 'yes', 'on', 'true', '1', '*'))) {
            return 1;
        }

        return 0;
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
        // the next if-statement is a workaround for derived attributes which do
        // not override the hide() method properly. This will not give them a
        // working hide() functionality but at least it will not give error messages.
        if ($record[$this->fieldName()] == '') {
            $record[$this->fieldName()] = '0';
        }
        if (!is_array($record[$this->fieldName()])) {
            $result = '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'" value="'.htmlspecialchars($record[$this->fieldName()]).'">';

            return $result;
        } else {
            Tools::atkdebug('Warning attribute '.$this->m_name.' has no proper hide method!');
        }
    }
}
