<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Db;

/**
 * Attribute for editing email fields.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @author Maurice Maas
 */
class EmailAttribute extends Attribute
{
    /*
     * @var boolean Bool to set DNS search in validate function
     * @access private
     */
    public $m_dnsSearch = false;

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_STRING;

    /**
     * Constructor.
     *
     * <b>Example:</b>
     * <code>
     *   $this->add(new atkEmailAttribute("email", false, self::AF_OBLIGATORY));
     * </code>
     *
     * @param string $name Name of the attribute
     * @param int $flags Flags for the attribute
     * @param bool $search Search DNS for MX records in validate function
     */
    public function __construct($name, $flags = 0, $search = false)
    {
        $this->m_dnsSearch = $search;
        parent::__construct($name, $flags);
    }

    /**
     * Returns a displayable string for this value.
     *
     * @param array $record The record to display
     * @param string $mode The display mode ("view" for viewpages, or "list"
     *                       for displaying in recordlists, "edit" for
     *                       displaying in editscreens, "add" for displaying in
     *                       add screens. "csv" for csv files. Applications can
     *                       use additional modes.
     *
     * @return string
     */
    public function display($record, $mode)
    {
        if ($mode == 'csv') {
            return parent::display($record, $mode);
        }

        if (isset($record[$this->fieldName()]) && $record[$this->fieldName()] != '') {
            return '<a href="mailto:'.$record[$this->fieldName()].'">'.$record[$this->fieldName()].'</a>';
        }

        return '';
    }

    /**
     * Validates email address through regular expression and dns check.
     *
     * @param array $record Record that contains value to be validated.
     *                       Errors are saved in this record, in the 'atkerror'
     *                       field.
     * @param string $mode Validation mode. Can be either "add" or "update"
     */
    public function validate(&$record, $mode)
    {
        $email = $record[$this->fieldName()];
        //first check complete string
        if (!self::validateAddressSyntax($email)) {
            Tools::triggerError($record, $this, 'error_invalid_email');
        } else {
            if ($this->m_dnsSearch) {
                //now check if domain exists, searches DNS for MX records
                list(, $domain) = explode('@', $email, 2);
                if (!(self::validateAddressDomain($domain, false))) {
                    Tools::triggerError($record, $this->fieldName(), 'error_unkown_domain', Tools::atktext('error_unkown_domain').' '.$domain);
                }
            }
        }
    }

    /**
     * Checks e-mail address syntax against a regular expression.
     *
     * @param string $email e-mail address.
     *
     * @return bool e-mailaddress syntactically valid or not.
     */
    public static function validateAddressSyntax($email)
    {
        $email = strtolower($email); // to allow uppercase
        if (preg_match("/^[-_a-zA-Z0-9+]+(\.[-_a-zA-Z0-9+]+)*@([0-9a-z-]+\.)*([0-9a-z][0-9a-z-]*[0-9a-z]\.)+[a-z]{2,}$/", $email)) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Check if the domain is valid and exists.
     *
     * The strict parameter decides if the MX record gets checked.
     *
     * @param string $domain
     * @param bool $strict
     *
     * @return bool $result
     * @static
     */
    public function validateAddressDomain($domain, $strict = false)
    {
        if ($strict) {
            $rr = 'MX';
        } else {
            $rr = 'ANY';
        }
        //Check if this domain has an MX host.
        if (checkdnsrr($domain, $rr)) {
            return true;
        } else {
            return false;
        }
    }
}

if (!function_exists('checkdnsrr')) {

    /**
     * Check an e-mail do main in DNS using nslookup.
     *
     * This is only used on Windows as on Linux environments this function
     * is native in PHP.
     *
     * @param string $hostName
     * @param string $recType
     *
     * @return bool
     */
    function checkdnsrr($hostName, $recType = 'MX')
    {
        if (!empty($hostName)) {
            $recType = escapeshellarg($recType);
            $hostNameArg = escapeshellarg($hostName);
            exec("nslookup -type=$recType $hostNameArg", $result);
            // check each line to find the one that starts with the host
            // name. If it exists then the function succeeded.
            foreach ($result as $line) {
                if (preg_match("/^$hostName/i", $line)) {
                    return true;
                }
            }

            // otherwise there was no mail handler for the domain
            return false;
        }

        return false;
    }
}
