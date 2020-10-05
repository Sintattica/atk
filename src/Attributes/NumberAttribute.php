<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Ui\Page;

/**
 * The NumberAttribute can be used for numeric values.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class NumberAttribute extends Attribute
{
    // N.B. $m_size, $m_maxsize and $m_searchsize are relative to only the integral part of number (before decimal separator)

    public $m_decimals = null; // The number of decimals of the number.
    public $m_minvalue = false; // The minimum value of the number.
    public $m_maxvalue = false; // The maximum value of the number.
    public $m_use_thousands_separator = false; // use the thousands separator when formatting a number
    public $m_decimalseparator;
    public $m_thousandsseparator;
    public $m_trailingzeros = false; // Show trailing zeros
    public $m_dbfieldtype = Db::FT_NUMBER; // By default, without decimals, store as an NUMBER

    protected $touchspin;

    // ids of separators in atk language file
    const SEPARATOR_DECIMAL = 'decimal_separator';
    const SEPARATOR_THOUSAND = 'thousands_separator';
    // default separator
    const DEFAULT_SEPARATOR = '.';

    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param int $decimals The number of decimals to use.
     */
    public function __construct($name, $flags = 0, $decimals = null)
    {
        parent::__construct($name, $flags);

        $this->m_decimals = $decimals;
        $this->m_decimalseparator = Tools::atktext(self::SEPARATOR_DECIMAL, 'atk');
        $this->m_thousandsseparator = Tools::atktext(self::SEPARATOR_THOUSAND, 'atk');
    }

    /**
     * Returns the number of decimals.
     *
     * @return int decimals
     */
    public function getDecimals()
    {
        return (int)$this->m_decimals;
    }

    /**
     * Sets the number of decimals.
     *
     * @param int $decimals number of decimals
     */
    public function setDecimals($decimals)
    {
        $this->m_decimals = $decimals;
        $this->m_dbfieldtype = $this->getDecimals() > 0 ? Db::FT_DECIMAL : Db::FT_NUMBER;
    }

    /**
     * Set the minimum and maximum value of the number. Violations of this range.
     *
     * @param int $minvalue Minimum value of the number.
     * @param int $maxvalue Maximum value of the number.
     */
    public function setRange($minvalue, $maxvalue)
    {
        $this->m_minvalue = $minvalue;
        $this->m_maxvalue = $maxvalue;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * The regular Attribute uses PHP's nl2br() and htmlspecialchars()
     * methods to prepare a value for display, unless $mode is "cvs".
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
        if (isset($record[$this->fieldName()]) && $record[$this->fieldName()] !== '') {
            return $this->formatNumber($record[$this->fieldName()]);
        }

        return '';
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">).
     *
     * We have to format the value, so it matches the display value
     * Otherwise the value will be corrupted by removeSeparators
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
        if (!is_array($record[$this->fieldName()])) {
            $value = $this->formatNumber($record[$this->fieldName()]);
            return '<input type="hidden" id="'.$this->getHtmlId($fieldprefix).'" name="'.$this->getHtmlName($fieldprefix).'" value="'.htmlspecialchars($value).'">';
        } else {
            Tools::atkdebug('Warning attribute '.$this->m_name.' has no proper hide method!');
        }
    }

    /**
     * convert a formatted number to a real number.
     *
     * @param string $number The number that needs to be converted
     * @param string $decimal_separator override decimal separator
     * @param string $thousands_separator override thousands separator
     *
     * @return string The converted number
     */
    public function removeSeparators($number, $decimal_separator = '', $thousands_separator = '')
    {
        if (is_null($number)) {
            return null;
        }
        if (empty($decimal_separator)) {
            $decimal_separator = $this->m_decimalseparator;
        }
        if (empty($thousands_separator)) {
            $thousands_separator = $this->m_thousandsseparator;
        }

        if ($decimal_separator == $thousands_separator) {
            Tools::atkwarning('invalid thousandsseparator. identical to the decimal_separator');
            $thousands_separator = '';
        }

        if (strstr($number, $decimal_separator) !== false) {
            // check invalid input
            if (substr_count($number, $decimal_separator) > 2) {
                return $number;
            }

            $number = str_replace($thousands_separator, '', $number);
            $number = str_replace($decimal_separator, self::DEFAULT_SEPARATOR, $number);

            if (substr_count($number, self::DEFAULT_SEPARATOR) > 1) {
                $parts = explode(self::DEFAULT_SEPARATOR, $number);
                $decimals = array_pop($parts);
                $number = implode('', $parts).self::DEFAULT_SEPARATOR.$decimals;
            }
        } else {
            $number = str_replace($thousands_separator, '', $number);
        }

        return $number;
    }

    /**
     * Use the thousands separator when formatting a number.
     *
     * @param bool $use_separator
     *
     * @return bool
     */
    public function setUseThousandsSeparator($use_separator)
    {
        $this->m_use_thousands_separator = (bool)$use_separator;
    }

    /**
     * Returns true if we 're using the thousands separator
     * when formatting the number.
     *
     * @return bool
     */
    public function getUseThousandsSeparator()
    {
        return $this->m_use_thousands_separator;
    }

    /**
     * Get the thousands separator.
     *
     * @return string with the thousands separator
     */
    public function getThousandsSeparator()
    {
        return $this->m_thousandsseparator;
    }

    /**
     * Set the thousands separator.
     *
     * @param string $separator The thousands separator
     */
    public function setThousandsSeparator($separator)
    {
        $this->m_thousandsseparator = $separator;
    }

    /**
     * Get the decimal separator.
     *
     * @return string with the decimal separator
     */
    public function getDecimalSeparator()
    {
        return $this->m_decimalseparator;
    }

    /**
     * Set the decimal separator.
     *
     * @param string $separator The decimal separator
     */
    public function setDecimalSeparator($separator)
    {
        $this->m_decimalseparator = $separator;
    }

    /**
     * Set showing/hiding of trailing zeros.
     *
     * @param bool $value
     */
    public function setTrailingZeros($value)
    {
        $this->m_trailingzeros = $value;
    }

    /**
     * Formats the number based on setting in the language file.
     *
     * @param float $number number
     * @param string $decimalSeparator override decimal separator
     * @param string $thousandsSeparator override thousands separator
     * @param string $mode
     *
     * @return string nicely formatted number
     */
    protected function formatNumber($number, $decimalSeparator = '', $thousandsSeparator = '', $mode = '')
    {
        if ($number === null || $number === '') {
            return '';
        }

        $decimalSeparator = $decimalSeparator == null ? $this->m_decimalseparator : $decimalSeparator;
        $thousandsSeparator = $thousandsSeparator == null ? $this->m_thousandsseparator : $thousandsSeparator;
        // (never shows the thousands separator in add/edit mode)
        $thousandsSeparator = ($this->m_use_thousands_separator && !in_array($mode, array('add', 'edit'))) ? $thousandsSeparator : '';

        if ($decimalSeparator == $thousandsSeparator) {
            Tools::atkwarning('invalid thousandsseparator. identical to the decimal_separator');
            $thousandsSeparator = '';
        }

        // NOTE: we don't use number_format because this sometimes causes rounding issues
        //       if a float can not be properly represented (see http://nl.php.net/manual/en/function.number-format.php#93893)

        $tmp1 = abs(round((float)$number, $this->getDecimals()));
        $tmp1 .= $this->getDecimals() > 0 && strpos($tmp1, '.') === false ? '.' : '';
        $tmp1 .= str_repeat('0', max($this->getDecimals() - strlen(substr($tmp1, strpos($tmp1, '.') + 1)), 0));

        while (($tmp2 = preg_replace('/(?<!.)(\d+)(\d\d\d)/', '\1 \2', $tmp1)) != $tmp1) {
            $tmp1 = $tmp2;
        }

        $r = strtr($tmp1, array(' ' => $thousandsSeparator, '.' => $decimalSeparator));
        if ($number < 0) {
            $r = '-'.$r;
        }

        if (!$this->m_trailingzeros) {
            // remove trailing zeros
            if (strpos($r, $decimalSeparator)) {
                $r = rtrim($r, '0');
                $r = rtrim($r, $decimalSeparator);
            }
        }

        return $r;
    }

    /**
     * Validates if value is numeric.
     *
     * @param array $record Record that contains value to be validated.
     *                       Errors are saved in this record
     * @param string $mode can be either "add" or "update"
     */
    public function validate(&$record, $mode)
    {
        if (!is_numeric($record[$this->fieldName()]) && $record[$this->fieldName()] != '') {
            Tools::triggerError($record, $this->fieldName(), 'error_notnumeric');
        }
        if (($this->m_maxvalue !== false) && ($record[$this->fieldName()] > $this->m_maxvalue)) {
            Tools::triggerError($record, $this->fieldName(), 'above_maximum_value');
        }
        if (($this->m_minvalue !== false) && ($record[$this->fieldName()] < $this->m_minvalue)) {
            Tools::triggerError($record, $this->fieldName(), 'below_minimum_value');
        }
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * If the user entered a number in his native language, he may have used
     * a different decimal separator, which we first convert to the '.'
     * standard separator (ATK uses the regular dot notation internally)
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     *
     * @return string The internal value
     */
    public function fetchValue($postvars)
    {
        return $this->removeSeparators(parent::fetchValue($postvars));
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * For the regular Attribute, this means escaping things like
     * quotes and slashes. Derived attributes may reimplement this for their
     * own conversion.
     * This is the exact opposite of the db2value method.
     *
     * @param array $rec The record that holds this attribute's value.
     *
     * @return string The database compatible value
     */
    public function value2db($rec)
    {
        if ((!isset($rec[$this->fieldName()]) || strlen($rec[$this->fieldName()]) == 0) && !$this->hasFlag(self::AF_OBLIGATORY)) {
            return;
        }
        if ($this->getDecimals() > 0) {
            return round((float)$rec[$this->fieldName()], $this->getDecimals());
        } else {
            return isset($rec[$this->fieldName()]) ? $rec[$this->fieldName()] : null;
        }
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * Note that not all modes may be supported by the database driver.
     * Compare this list to the one returned by the databasedriver, to
     * determine which searchmodes may be used.
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
        return self::getStaticSearchModes();
    }

    public static function getStaticSearchModes()
    {
        return array('exact', 'between', 'greaterthan', 'greaterthanequal', 'lessthan', 'lessthanequal');
    }

    /**
     * Return the size of the field in the database.
     *
     * If 0 is returned, the size is unknown. In this case, the
     * return value should not be used to create table columns.
     *
     * Ofcourse, the size does not make sense for every field type.
     * So only interpret the result if a size has meaning for
     * the field type of this attribute. (For example, if the
     * database field is of type 'date', the size has no meaning)
     *
     * Note that derived attributes might set a dot separated size,
     * for example to store decimal numbers. The number after the dot
     * should be interpreted as the number of decimals.
     *
     * @return int The database field size
     */
    public function dbFieldSize()
    {
        return $this->m_maxsize.($this->getDecimals() > 0 ? ','.$this->getDecimals() : '');
    }

    /**
     * Apply database metadata for setting the attribute size.
     * @param array $metadata
     */
    public function fetchMeta($metadata)
    {
        // N.B. size, maxsize and searchsize are relative to only the integral part of number (before decimal separator)

        $attribname = strtolower($this->fieldName());

        // maxsize and decimals
        // (if the value is explicitly set, but the database simply can't handle it, we use the smallest one)
        if (isset($metadata[$attribname])) {
            if (strpos($metadata[$attribname]['len'], ',') !== false) {
                list($metaSize, $metaDecimals) = explode(',', $metadata[$attribname]['len']);
                $metaSize = (int)$metaSize;
                $metaDecimals = (int)$metaDecimals;

                // decimals
                if ($this->m_decimals === null) {
                    $this->m_decimals = $metaDecimals;
                } else {
                    $this->m_decimals = min($this->m_decimals, $metaDecimals);
                }
            } else {
                $metaSize = $metadata[$attribname]['len'];
            }

            // maxsize
            if ($this->m_maxsize > 0) {
                $this->m_maxsize = min($this->m_maxsize, $metaSize);
            } else {
                $this->m_maxsize = $metaSize;
            }
        }

        // size
        if (!$this->m_size) {
            $this->m_size = min($this->m_maxsize, $this->maxInputSize());
        }

        // searchsize
        if (!$this->m_searchsize) {
            $this->m_searchsize = min($this->m_maxsize, $this->maxSearchInputSize());
        }
    }

    /**
     * See http://www.virtuosoft.eu/code/bootstrap-touchspin/
     * @param array $options
     */
    public function enableTouchspin($options = [])
    {
        if ($this->m_decimals) {
            $options['decimals'] = $this->m_decimals;
        }
        if ($this->m_minvalue) {
            $options['min'] = $this->m_minvalue;
        }
        if ($this->m_maxvalue) {
            $options['max'] = $this->m_maxvalue;
        }
        $this->touchspin = $options;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record Array with values
     * @param string $fieldprefix The attribute must use this to prefix its form elements (used for
     *                            embedded forms)
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string Piece of htmlcode
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);

        $style = '';
        foreach($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }
        
        if (Tools::count($this->m_onchangecode)) {
            $onchange = 'onChange="'.$id.'_onChange(this);"';
            $this->_renderChangeHandler($fieldprefix);
        } else {
            $onchange = '';
        }

        $size = $this->m_size;
        $maxsize = $this->m_maxsize;
        if ($this->getDecimals() > 0) {
            $size += ($this->getDecimals() + 1);
            $maxsize += ($this->getDecimals() + 1); // make room for the number of decimals
            // TODO we should also consider the sign symbol (for signed type)
        }

        $value = '';
        if (isset($record[$this->fieldName()]) && strlen($record[$this->fieldName()]) > 0) {
            $value = $this->formatNumber($record[$this->fieldName()], '', '', $mode);
        }

        $result = '';
        $result .= '<input type="text" id="'.$id.'"';
        $result .= ' name="'.$name.'"';
        $result .= ' '.$this->getCSSClassAttribute(array('form-control'));
        $result .= ' value="'.$value.'"';
        if($size > 0){
            $result .= ' size="'.$size.'"';
        }
        if($maxsize > 0){
            $result .= ' maxlength="'.$maxsize.'"';
        }
        if($onchange){
            $result .= ' '.$onchange;
        }
        if($placeholder = $this->getPlaceholder()){
            $result .= ' placeholder="'.htmlspecialchars($placeholder).'"';
        }
        if($style != ''){
            $result .= ' style="'.$style.'"';
        }
        $result .= ' />';

        if (is_array($this->touchspin)) {
            $page = Page::getInstance();
            $base = Config::getGlobal('assets_url') . 'lib/bootstrap-touchspin/';
            $page->register_script($base . 'jquery.bootstrap-touchspin.min.js');
            $page->register_style($base . 'jquery.bootstrap-touchspin.min.css');
            $opts = json_encode($this->touchspin);
            $page->register_loadscript("
                jQuery(function($){
                    $('#$id').TouchSpin($opts);
                });");
        }

        return $result;
    }

    public function search($atksearch, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        $value = $atksearch[$this->getHtmlName()] ?? '';

        $searchsize = $this->m_searchsize;
        if ($this->m_decimals > 0) { // (don't use getDecimals to avoid fatal error when called from ExpressionAttribute)
            $searchsize += ($this->m_decimals + 1); // make room for the number of decimals
            // TODO we should also consider the sign symbol (for signed type)
        }

        $class = $this->getCSSClassAttribute(['form-control']);
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getSearchFieldName($fieldprefix);
        $style = '';
        $type = $extended ? 'extended_search':'search';
        foreach($this->getCssStyles($type) as $k => $v) {
            $style .= "$k:$v;";
        }

        if (!$extended) {
            if (is_array($value)) { // values entered in the extended search
                // TODO we would need to know the searchmode for better handling...
                if ($value['from'] != '' && $value['to'] != '') {
                    $value = $value['from'].'/'.$value['to'];
                } elseif ($value['from'] != '') {
                    $value = $value['from'];
                } elseif ($value['to'] != '') {
                    $value = $value['to'];
                } else {
                    $value = '';
                }
            }

            $result = '<input type="text" id="'.$id.'" '.$class.' name="'.$name.'"';
            $result .= ' value="'.htmlentities($value).'"'.($searchsize > 0 ? ' size="'.$searchsize.'"' : '');
            if($style != ''){
                $result .= ' style="'.$style.'"';
            }
            $result .= '>';
        } else {

            if (is_array($value)) {
                $valueFrom = $value['from'];
                $valueTo = $value['to'];
            } else {
                $valueFrom = $valueTo = $value;
            }

            $result = '<div class="form-inline"';
            if($style != ''){
                $result .= ' style="'.$style.'"';
            }
            $result .= '>';
            $result .= '<input type="text" id="'.$id.'" '.$class.' name="'.$name.'[from]" value="'.htmlentities($valueFrom).'"'.($searchsize > 0 ? ' size="'.$searchsize.'"' : '').'>';


            $result .= ' ('.Tools::atktext('until').' <input type="text" id="'.$id.'" class="form-control '.get_class($this).'" name="'.$name.'[to]" value="'.htmlentities($valueTo).'"'.($searchsize > 0 ? ' size="'.$searchsize.'"' : '').'>)';
            $result .= '</div>';
        }

        return $result;
    }

    /**
     * Process the search value.
     *
     * @param string $value The search value
     * @param string $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
     *
     * @return string with the processed search value
     */
    public function processSearchValue($value, &$searchmode)
    {
        $processed = null;
        if (!is_array($value)) {
            // quicksearch
            $value = trim($value);
            if (strpos($value, '/') !== false) { // from/to searches
                list($from, $to) = explode('/', $value);
                $from = $this->removeSeparators(trim($from));
                $to = $this->removeSeparators(trim($to));
                $from = is_numeric($from) ? $from : '';
                $to = is_numeric($to) ? $to : '';
                $processed = array('from' => $from, 'to' => $to);
                $searchmode = 'between';
            } else { // single value
                $value = $this->removeSeparators($value);
                if (is_numeric($value)) {
                    $processed['from'] = $value;
                } else {
                    $processed = [];
                }
            }
        } else {
            // assumes array('from'=><intval>, 'to'=><intval>)
            foreach ($value as $key => $search) {
                $v = $this->removeSeparators($search);
                $processed[$key] = is_numeric($v) ? $v : '';
            }
        }

        return $processed;
    }

    /**
     * Get the between search condition
     *
     * This function checks the nullity of one attribute (which leads to greaterthan or
     * lessthan condition) and reorder the values if needed.
     *
     * @param Query $query The query object where the search condition should be placed on
     * @param string $fieldname The name of the field in the database (quoted)
     * @param array $value The processed search values indexed by 'from' and 'to'
     *
     * @return QueryPart where clause for searching
     */
    public function getBetweenCondition($query, $fieldname, $value)
    {
        if ($value['from'] != '' && $value['to'] != '') {
            if ($value['from'] > $value['to']) {
                // User entered fields in wrong order. Let's fix that.
                $tmp = $value['from'];
                $value['from'] = $value['to'];
                $value['to'] = $tmp;
            }

            return $query->betweenCondition($fieldname, $value['from'], $value['to']);
        } elseif ($value['from'] != '' && $value['to'] == '') {
            return $query->greaterthanequalCondition($fieldname, $value['from']);
        } elseif ($value['from'] == '' && $value['to'] != '') {
            return $query->lessthanequalCondition($fieldname, $value['to']);
        }

        return null;
    }

    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        $value = $this->processSearchValue($value, $searchmode);

        if ($searchmode != 'between') {
            if ($value['from'] != '') {
                $value = $value['from'];
            } elseif ($value['to'] != '') {
                $value = $value['to'];
            } else {
                return null;
            }

            return parent::getSearchCondition($query, $table, $value, $searchmode);
        }

        return $this->getBetweenCondition($query, Db::quoteIdentifier($table, $this->fieldName()), $value);
    }
}
