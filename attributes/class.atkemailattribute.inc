<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * The file contains the default values for most configuration
   * settings.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Attribute for editing email fields.
   *
   * @author Maurice Maas <maurice@ibuildings.nl>
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkEmailAttribute extends atkAttribute
  {

    /**
     * @var boolean Bool to set DNS search in validate function
     * @access private
     */
    var $m_dnsSearch = false;

    /**
     * Constructor
     *
     * <b>Example:</b>
     * <code>
     *   $this->add(new atkEmailAttribute("email", false, AF_OBLIGATORY));
     * </code>
     *
     * @param String $name Name of the attribute
     * @param boolean $search Search DNS for MX records in validate function
     * @param int $flags Flags for the attribute
     * @param int $size The size of the field in characters
     */
    function atkEmailAttribute($name, $search = false, $flags = 0, $size=0)
    {
      $this->m_dnsSearch = $search;
      $this->atkAttribute($name, $flags, $size);
    }

    /**
     * Returns a displayable string for this value.
     * @param array $record The record to display
     * @return String
     */
    function display($record)
    {
      if ($record[$this->fieldName()]!="")
      {
        return '<a href="mailto:'.$record[$this->fieldName()].'">'.$record[$this->fieldName()].'</a>';
      }
      return '';
    }

    /**
     * Validates email address through regular expression and dns check
     * @param array $record Record that contains value to be validated.
     *                      Errors are saved in this record, in the 'atkerror'
     *                      field.
     * @param String $mode Validation mode. Can be either "add" or "update"
     */
    function validate(&$record, $mode)
    {
      $email = $record[$this->fieldName()];
      //first check complete string
      if (!(eregi("^[_a-z0-9-]+(\.[_a-z0-9-]+)*@([0-9a-z][0-9a-z-]*[0-9a-z]\.)+[a-z]{2,4}$", $email, $check)))
      {
        triggerError($record, $this->fieldName(), 'error_invalid_email');
      }
      else
      {
        if ($this->m_dnsSearch)
        {
          //now check if domain exists, searches DNS for MX records
          $domain = substr(strstr($check[0], '@'), 1);
          if (!( checkdnsrr( $domain, "ANY") ))
          {
            triggerError($record, $this->fieldName(), 'error_unkown_domain', text('error_unkown_domain')." ".$domain);
          }
        }
      }
    }

    /**
     * Called by the framework to determine the database field datatype.
     * @return String The databasefield datatype.
     */
    function dbFieldType()
    {
      return "string";
    }

  }

  if (!function_exists("checkdnsrr"))
  {
    /**
     * Check an e-mail do main in DNS using nslookup.
     *
     * This is only used on Windows as on Linux environments this function
     * is native in PHP.
     * @access private
     */
    function checkdnsrr($hostName, $recType = 'MX')
    {
      if(!empty($hostName))
      {
        $recType = escapeshellarg($recType);
        $hostName = escapeshellarg($hostName);
        exec("nslookup -type=$recType $hostName", $result);
        // check each line to find the one that starts with the host
        // name. If it exists then the function succeeded.
        foreach ($result as $line)
        {
          if(eregi("^$hostName",$line))
          {
            return true;
          }
        }
        // otherwise there was no mail handler for the domain
        return false;
      }
      return false;
    }
  }

?>
