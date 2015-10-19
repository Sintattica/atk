<?php namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\IpUtils;

/**
 * The IpAttribute can be used to let the user enter IP(v4) addresses.
 * It's (optionally) possible to let the user enter wildcards.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @author Guido van Biemen <guido@ibuildings.nl>
 * @package atk
 * @subpackage attributes
 */
class IpAttribute extends Attribute
{
    /**
     * Flags for the atkIpAttribute
     */
    const AF_IP_ALLOW_WILDCARDS = self::AF_SPECIFIC_1;
    const AF_IP_STORENUMERIC = self::AF_SPECIFIC_2;
    const AF_IP_SINGLEFIELD = self::AF_SPECIFIC_3;

    /**
     * Constructor.
     *
     * @param string $name attribute name
     * @param int $flags attribute flags.
     */
    function __construct($name, $flags)
    {
        parent::__construct($name, $flags, 15);
    }

    /**
     * Fetch value.
     *
     * @param array $postvars post vars
     *
     * @return string fetched value
     */
    function fetchValue($postvars)
    {
        if ($this->hasFlag(self::AF_IP_SINGLEFIELD)) {
            return parent::fetchValue($postvars[$this->fieldName()]);
        }
        if (!$this->isPosted($postvars)) {
            return null;
        }

        $parts = array();
        for ($i = 0; $i < 4; $i++) {
            $parts[$i] = $postvars[$this->fieldName()][$i];
        }

        return implode('.', $parts);
    }

    /**
     * Returns form fields to edit the ip address.
     *
     * @param array $record the record
     * @param string $fieldprefix the field prefix
     *
     * @return string html string
     */
    function edit($record, $fieldprefix = "")
    {
        if ($this->hasFlag(self::AF_IP_SINGLEFIELD)) {
            return parent::edit($record, $fieldprefix);
        }

        $inputs = array();
        $values = empty($record[$this->fieldName()]) ? null : explode('.', $record[$this->fieldName()]);

        for ($i = 0; $i < 4; $i++) {
            $name = $fieldprefix . $this->fieldName() . '[' . $i . ']';
            $value = isset($values[$i]) ? $values[$i] : '';
            $inputs[] = '<input type="text" name="' . $name . '" value="' . $value . '" maxlength="3" size="3" />';
        }

        return implode('.', $inputs);
    }

    /**
     * Checks if the value is a valid IP address
     *
     * @param array $record The record that holds the value for this
     *                      attribute. If an error occurs, the error will
     *                      be stored in the 'atkerror' field of the record.
     * @param String $mode The mode for which should be validated ("add" or
     *                     "update")
     */
    function validate(&$record, $mode)
    {
        // Check for valid ip string
        $strvalue = Tools::atkArrayNvl($record, $this->fieldName(), "");
        if (!empty($strvalue)) {
            if ($this->hasFlag(self::AF_IP_ALLOW_WILDCARDS) && !$this->hasFlag(self::AF_IP_STORENUMERIC)) {
                $strvalue = str_replace("*", "0", $strvalue);
            }
            $num = '(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])';
            if (preg_match("/^$num\\.$num\\.$num\\.$num$/", $strvalue, $matches) <= 0) {
                Tools::triggerError($record, $this->fieldName(), 'error_not_a_valid_ip');
            }
        }
        parent::validate($record, $mode);
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * @param array $rec The record that holds this attribute's value.
     * @return String The database compatible value
     */
    function value2db($rec)
    {
        // By default, return the plain ip number
        if (!$this->hasFlag(self::AF_IP_STORENUMERIC)) {
            return Tools::atkArrayNvl($rec, $this->fieldName());
        }

        // But if the self::AF_IP_STORENUMERIC flag is set, we store it as long integer
        return IpUtils::ipLongFormat(Tools::atkArrayNvl($rec, $this->fieldName()));
    }

    /**
     * Converts a database value to an internal value.
     *
     * @param array $rec The database record that holds this attribute's value
     * @return mixed The internal value
     */
    function db2value($rec)
    {
        // By default, return the plain ip number
        if (!$this->hasFlag(self::AF_IP_STORENUMERIC)) {
            return Tools::atkArrayNvl($rec, $this->fieldName());
        }

        // But if the self::AF_IP_STORENUMERIC flag is set, we load it as long integer
        return IpUtils::ipStringFormat(Tools::atkArrayNvl($rec, $this->fieldName()));
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return String The 'generic' type of the database field for this attribute.
     */
    function dbFieldType()
    {
        return $this->hasFlag(self::AF_IP_STORENUMERIC) ? "int" : "string";
    }

    /**
     * Return the size of the field in the database.
     *
     * @return int The database field size
     */
    function dbFieldSize()
    {
        return $this->hasFlag(self::AF_IP_STORENUMERIC) ? 32 : 15;
    }

}


