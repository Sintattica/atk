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
   */

  /** @internal includes **/
  atkimport("atk.atklanguage");
  useattrib("atklistattribute");

  /**
   * The atkTimeZone class represents an attribute to handle timezones in a listbox.
   *
   * @author Dennis Luitwieler <dennis@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   */
  class atkTimeZoneAttribute extends atkListAttribute
  {
     var $m_defaulttocurrent = true;

   /**
    * Constructor
    *
    * <b>Example:</b>
    *        $this->add(new atkTimeZoneAttribute("timezone",AF_OBLIGATORY));
    * @param $name Name of the attribute
    * @param $flags Flags for the attribute
    */
    function atkTimeZoneAttribute($name, $flags = 0)
    {
      $optionsArray = array(
        'timezone_utc_-1200',
        'timezone_utc_-1100',
        'timezone_utc_-1000',
        'timezone_utc_-0900',
        'timezone_utc_-0800',
        'timezone_utc_-0700',
        'timezone_utc_-0600',
        'timezone_utc_-0500',
        'timezone_utc_-0400',
        'timezone_utc_-0300',
        'timezone_utc_-0200',
        'timezone_utc_-0100',
        'timezone_utc_+0000',
        'timezone_utc_+0100',
        'timezone_utc_+0200',
        'timezone_utc_+0300',
        'timezone_utc_+0400',
        'timezone_utc_+0500',
        'timezone_utc_+0600',
        'timezone_utc_+0700',
        'timezone_utc_+0800',
        'timezone_utc_+0900',
        'timezone_utc_+1000',
        'timezone_utc_+1100',
        'timezone_utc_+1200',
        'timezone_utc_+1300',
      );

      $valuesArray = array(
        '-1200',
        '-1100',
        '-1000',
        '-0900',
        '-0800',
        '-0700',
        '-0600',
        '-0500',
        '-0400',
        '-0300',
        '-0200',
        '-0100',
        '+0000',
        '+0100',
        '+0200',
        '+0300',
        '+0400',
        '+0500',
        '+0600',
        '+0700',
        '+0800',
        '+0900',
        '+1000',
        '+1100',
        '+1200',
        '+1300',
      );

      $this->atkListAttribute($name,$optionsArray,$valuesArray,$flags, 0);
    }

    /**
     * Returns the UTC offset in seconds for a value of the timezone attribute.
     *
     * @param string $value
     * @param timestamp $timestamp
     * @return int UTC offset in seconds
     *
     * @static
     */
    function getUTCOffset($value, $timestamp=null)
    {
      if ($value === null)
      {
        return 0;
      }
      else
      {
        list($sign, $hours, $minutes) = sscanf($value, '%1s%2d%2d');
        return ($sign == '+' ? 1 : -1) * ($hours * 60 * 60) + ($minutes * 60);
      }
    }
  }
?>
