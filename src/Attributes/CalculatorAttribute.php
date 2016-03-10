<?php namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Utils\StringParser;

/**
 * a simple but flexible value calculator.
 *
 * This attribute adds a column that contains a calculated value.
 * Calculations can range from simple "3*4" fixed calculations to
 * calculations based on other attributes. The examples are pretty
 * self-explanatory:
 *
 * <i>Example usages</i>
 *
 * A simple calculation:
 *   $this->add(new atkCalculatorAttribute("simple", "10*5"));
 *
 * A calculation using an value from the current record:
 *   $this->add(new atkCalculatorAttribute("vat", "[price]*0.19"));
 *
 * A calculation using multiple values, bracketed calculations, and
 * a previous calculation:
 *   new atkCalculatorAttribute("total", "([price]*[quantity])+[vat]");
 *
 * A calculation using values from a relation:
 *   new atkCalculatorAttribute("vat", "[vattype.percentage]*[total]");
 *
 * <b>A word of caution</b>
 *
 * The PHP function eval() is used to perform the calculation, so use this
 * only in a trusted environment, and make sure that the user cannot put
 * arbitrary php code in a value (for example, by only performing
 * calculations on atkNumberAttribute values, which are validated to
 * contain only numeric values).
 *
 * Note that if the calculation contains invalid php code, for example
 * unmatched brackets, a php parse error will be thrown.
 *
 * You can use self::AF_TOTAL to totalize the values in recordlists.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage attributes
 *
 */
class CalculatorAttribute extends Attribute
{
    /**
     * The calculation to perform.
     * @access private
     * @var String
     */
    public $m_calculation = null;

    /**
     * Constructor
     *
     * @param string $name Unique name of this attribute within the node.
     * @param string $calculation The calculation to perform. Must be a valid php expression.
     * @param int $flags Flags of the attribute.
     */
    public function __construct($name, $calculation, $flags = 0)
    {
        parent::__construct($name, $flags | self::AF_NO_SORT | self::AF_HIDE_SEARCH | self::AF_READONLY);

        $this->m_calculation = $calculation;
    }

    /**
     * Make sure the value is not stored. (always calculated on the fly)
     * @access private
     * @return int
     */
    public function storageType()
    {
        return self::NOSTORE;
    }

    /**
     * Make sure the value is loaded *after* the main record is loaded.
     * @access private
     * @return int
     */
    public function loadType()
    {
        return self::POSTLOAD;
    }

    /**
     * The load method performs the calculation.
     *
     * @access private
     * @param Db $db
     * @param array $record
     * @return String result of the calculation
     */
    public function load(&$db, $record)
    {
        $parser = new StringParser($this->m_calculation);
        eval("\$result = ".$parser->parse($record).";");

        return $result;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * @access public
     * @param array $record The record that holds the value for this attribute
     * @param string $mode The display mode.
     * @return String HTML String
     */
    public function display($record, $mode)
    {
        if ($this->m_ownerInstance->m_partial == "attribute.".$this->fieldName().".refresh") {
            $record[$this->fieldName()] = $this->load($db, $record);
        }

        return parent::display($record, $mode);
    }
}
