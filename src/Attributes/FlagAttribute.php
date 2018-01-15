<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Query;

/**
 * The FlagAttribute class offers an way to edit bitmask flags.
 *
 * @author  M. Roest <martin@ibuildings.nl>
 */
class FlagAttribute extends MultiSelectAttribute
{
    /**
     * Constructor.
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for this attribute
     * @param array $optionArray Array with options
     * @param array $valueArray Array with values. If you don't use this parameter,
     *                            values are assumed to be the same as the options.
     */
    public function __construct($name, $flags = 0, $optionArray, $valueArray = null)
    {
        parent::__construct($name, $flags, $optionArray, $valueArray);
        $this->m_dbfieldtype = 'number';
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * @param array $rec The record that holds this attribute's value.
     *
     * @return string The database compatible value
     */
    public function value2db($rec)
    {
        if (is_array($rec[$this->fieldName()]) && Tools::count($rec[$this->fieldName()]) >= 1) {
            $flags = 0;
            foreach ($rec[$this->fieldName()] as $flag) {
                $flags |= $flag;
            }

            return $flags;
        } else {
            return 0;
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
        if ($rec[$this->fieldName()] > 0) {
            $newrec = [];
            foreach ($this->m_values as $value) {
                if (Tools::hasFlag($rec[$this->fieldName()], $value)) {
                    $newrec[] = $value;
                }
            }

            return $newrec;
        }

        return [];
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return string The 'generic' type of the database field for this
     *                attribute.
     */
    public function dbFieldType()
    {
        return $this->m_dbfieldtype;
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
        $searchcondition = '';
        if (is_array($value) && Tools::count($value) > 0 && $value[0] != '') { // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.
            $field = $table.'.'.$this->fieldName();
            if (Tools::count($value) == 1) { // exactly one value
                $query->addSearchCondition($field.' & '.$value[0]);
            } else {
                $mask = '('.implode('|', $value).')';
                $searchcondition = $field.'&'.$mask.'='.$mask;
            }
        }

        return $searchcondition;
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * For the regular Attribute, this means getting the field with the
     * same name as the attribute from the html posting.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     *
     * @return string The internal value
     */
    public function fetchValue($postvars)
    {
        $vars = Tools::atkArrayNvl($postvars, $this->fieldName());
        if (!is_array($vars)) {
            $result = [];
            foreach ($this->m_values as $value) {
                if (Tools::hasFlag($vars, $value)) {
                    $result[] = $value;
                }
            }

            return $result;
        } else {
            return $vars;
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
        $values = 0;
        $name = $this->fieldName();
        if (is_array($record[$name])) {
            foreach ($record[$name] as $var) {
                $values |= $var;
            }
        } else {
            $values = $record[$name];
        }

        return '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'" value="'.$values.'">';
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
}
