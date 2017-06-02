<?php
useattrib("atknumberattribute");

/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage attributes
 *
 * @copyright (c)2000-2004 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 */

/**
 * The atkCurrencyAttribute can be used for money values.
 *
 * @author Mark Baaijens <mark@ibuildings.nl>
 * @package atk
 * @subpackage attributes
 *
 */
class atkCurrencyAttribute extends atkNumberAttribute
{
    var $m_currencysymbol;

    /**
     * Constructor
     * @param string $name  Name of the attribute
     * @param int    $flags Flags for this attribute
     * @param int    $size  The size(s) for this attribute (default 10)
     * @param string $currencysymbol The symbol which is printed in front of the value.
     * @param int    $decimals The number of decimals (default 2)
     * @param string $decimalseparator The separator which is printed for the decimals.
     * @param string $thousandsseparator The separator which is printed for the thousands.
     *
     */
    function __construct($name, $flags = 0, $size = 10, $currencysymbol = "", $decimals = 2, $decimalseparator = "", $thousandsseparator = "")
    {
	    parent::__construct($name, $flags, $size, $decimals); // base class constructor

        if ($currencysymbol == "")
            $currencysymbol = atktext("currencysymbol", "atk", "", "", "", true);

        $this->m_currencysymbol = $currencysymbol;
        $this->m_decimalseparator = ($decimalseparator != "" ? $decimalseparator
                    : ".");
        $this->m_thousandsseparator = ($thousandsseparator != "" ? $thousandsseparator
                    : ",");

        $this->setUseThousandsSeparator(true);
        $this->setTrailingZeros(true);
    }

    /**
     * overrides the edit function to put the currencysymbol in front of the input field
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record = "", $fieldprefix = "", $mode = '')
    {
        return $this->getCurrencySymbolDisplay() . parent::edit($record, $fieldprefix, $mode);
    }

    /**
     * overrides the display function to put the currencysymbol in front of the input field
     *
     * The regular atkAttribute uses PHP's nl2br() and htmlspecialchars()
     * methods to prepare a value for display, unless $mode is "cvs".
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists, "edit" for
     *                     displaying in editscreens, "add" for displaying in
     *                     add screens. "csv" for csv files. Applications can
     *                     use additional modes.
     * @return String HTML String
     */
    function display($record, $mode = "")
    {
        $result = empty($this->m_currencysymbol) ? "" : $this->getCurrencySymbolDisplay();
        $result .= parent::display($record, $mode);
        return $result;
    }

    /**
     * Get currency symbol display
     * @return string
     */
    function getCurrencySymbolDisplay()
    {
        return '<span class="currencysymbol">' . $this->m_currencysymbol . '</span> ';
    }

}
