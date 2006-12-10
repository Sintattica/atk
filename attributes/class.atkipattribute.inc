<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  define("AF_IP_ALLOW_WILDCARDS", AF_SPECIFIC_1);
  define("AF_IP_STORENUMERIC", AF_SPECIFIC_2);
  define("AF_IP_SINGLEFIELD", AF_SPECIFIC_3);

  /**
   * The atkIpAttribute can be used to let the user enter IP(v4) addresses.
   * It's (optionally) possible to let the user enter wildcards.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @author Guido van Biemen <guido@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   */
  class atkIpAttribute extends atkAttribute
  {
    /**
     * Constructor.
     *
     * @param $name attribute name
     * @param $flags attribute flags.
     */
    function atkIpAttribute($name, $flags)
    {
      $this->atkAttribute($name, $flags, 15);
    }

    /**
     * Fetch value.
     *
     * @param array $postvars post vars
     *
     * @return fetched value
     */
    function fetchValue($postvars)
    {
      if ($this->hasFlag(AF_IP_SINGLEFIELD))
        return parent::fetchValue($postvars[$this->fieldName()]);
      if (!$this->isPosted($postvars))
        return NULL;

      $parts = array();
      for ($i = 0; $i < 4; $i++)
        $parts[$i] = $postvars[$this->fieldName()][$i];

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
    function edit($record, $fieldprefix="")
    {
      if ($this->hasFlag(AF_IP_SINGLEFIELD))
        return parent::edit($record, $fieldprefix);

      $inputs = array();
      $values = empty($record[$this->fieldName()]) ? NULL : explode('.', $record[$this->fieldName()]);

      for ($i = 0; $i < 4; $i++)
      {
        $name = $fieldprefix.$this->fieldName().'['.$i.']';
        $value = isset($values[$i]) ? $values[$i] : '';
        $inputs[] = '<input type="text" name="'.$name.'" value="'.$value.'" maxlength="3" size="3" />';
      }

      return implode('.', $inputs);
    }


    function validate(&$record, $mode)
    {
      // Check for valid ip string
      $strvalue = atkArrayNvl($record, $this->fieldName(), "");
      if (!empty($strvalue))
      {
        if ($this->hasFlag(AF_IP_ALLOW_WILDCARDS) && !$this->hasFlag(AF_IP_STORENUMERIC))
          $strvalue = str_replace("*", "0", $strvalue);
        $num='(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])';
        if (preg_match("/^$num\\.$num\\.$num\\.$num$/", $strvalue, $matches) <= 0)
          triggerError($record, $this->fieldName(), 'error_not_a_valid_ip');
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
      if (!$this->hasFlag(AF_IP_STORENUMERIC))
        return atkArrayNvl($rec, $this->fieldName());

      // But if the AF_IP_STORENUMERIC flag is set, we store it as long integer
      atkimport("atk.utils.atkiputils");
      return atkIpUtils::ipLongFormat(atkArrayNvl($rec, $this->fieldName()));
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
      if (!$this->hasFlag(AF_IP_STORENUMERIC))
        return atkArrayNvl($rec, $this->fieldName());

      // But if the AF_IP_STORENUMERIC flag is set, we load it as long integer
      atkimport("atk.utils.atkiputils");
      return atkIpUtils::ipStringFormat(atkArrayNvl($rec, $this->fieldName()));
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return String The 'generic' type of the database field for this attribute.
     */
    function dbFieldType()
    {
      return $this->hasFlag(AF_IP_STORENUMERIC) ? "int" : "string";
    }

    /**
     * Return the size of the field in the database.
     *
     * @return int The database field size
     */
    function dbFieldSize()
    {
      return $this->hasFlag(AF_IP_STORENUMERIC) ? 32 : 15;
    }
  }
?>
