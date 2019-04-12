<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Utils\IpUtils;
use Sintattica\Atk\Db\Db;

/**
 * The IpAttribute can be used to let the user enter IP(v4) addresses.
 * It's (optionally) possible to let the user enter wildcards.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @author Guido van Biemen <guido@ibuildings.nl>
 */
class IpAttribute extends Attribute
{
    /**
     * Flags for the atkIpAttribute.
     */
    const AF_IP_ALLOW_WILDCARDS = 33554432;
    const AF_IP_STORENUMERIC = 67108864;
    const AF_IP_SINGLEFIELD = 134217728;

    /**
     * Constructor.
     *
     * @param string $name attribute name
     * @param int $flags attribute flags.
     */
    public function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags);
        $this->setAttribSize(15);
    }

    /**
     * Fetch value.
     *
     * @param array $postvars post vars
     *
     * @return string fetched value
     */
    public function fetchValue($postvars)
    {
        if ($this->hasFlag(self::AF_IP_SINGLEFIELD)) {
            return parent::fetchValue($postvars[$this->fieldName()]);
        }
        if (!$this->isPosted($postvars)) {
            return;
        }

        $parts = [];
        for ($i = 0; $i < 4; ++$i) {
            $parts[$i] = $postvars[$this->getHtmlName()][$i];
        }

        return implode('.', $parts);
    }

    public function edit($record, $fieldprefix, $mode)
    {
        if ($this->hasFlag(self::AF_IP_SINGLEFIELD)) {
            return parent::edit($record, $fieldprefix, $mode);
        }

        $inputs = [];
        $values = empty($record[$this->fieldName()]) ? null : explode('.', $record[$this->fieldName()]);

        for ($i = 0; $i < 4; ++$i) {
            $name = $this->getHtmlName($fieldprefix).'['.$i.']';
            $value = isset($values[$i]) ? $values[$i] : '';
            $inputs[] = '<input type="text" name="'.$name.'" value="'.$value.'" maxlength="3" size="3" />';
        }

        return implode('.', $inputs);
    }

    /**
     * Checks if the value is a valid IP address.
     *
     * @param array $record The record that holds the value for this
     *                       attribute. If an error occurs, the error will
     *                       be stored in the 'atkerror' field of the record.
     * @param string $mode The mode for which should be validated ("add" or
     *                       "update")
     */
    public function validate(&$record, $mode)
    {
        // Check for valid ip string
        $strvalue = Tools::atkArrayNvl($record, $this->fieldName(), '');
        if ($strvalue != '' && $strvalue != '...') {
            if ($this->hasFlag(self::AF_IP_ALLOW_WILDCARDS) && !$this->hasFlag(self::AF_IP_STORENUMERIC)) {
                $strvalue = str_replace('*', '0', $strvalue);
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
     *
     * @return string The database compatible value
     */
    public function value2db($rec)
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
     *
     * @return mixed The internal value
     */
    public function db2value($rec)
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
     * @return string The 'generic' type of the database field for this attribute.
     */
    public function dbFieldType()
    {
        return $this->hasFlag(self::AF_IP_STORENUMERIC) ? Db::FT_NUMBER : Db::FT_STRING;
    }

    /**
     * Return the size of the field in the database.
     *
     * @return int The database field size
     */
    public function dbFieldSize()
    {
        return $this->hasFlag(self::AF_IP_STORENUMERIC) ? 32 : 15;
    }
}
