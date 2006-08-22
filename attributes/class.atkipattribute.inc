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
   * @copyright (c)2000-2006 Ivo Jansch
   * @copyright (c)2000-2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
  
  define("AF_IP_ALLOW_WILDCARDS", AF_SPECIFIC_1);

  /**
   * The atkIpAttribute can be used to let the user enter IP(v4) addresses.
   * It's (optionally) possible to let the user enter wildcards.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
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
      $this->atkAttribute($name, $flags, 16);
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
      $value = $postvars[$this->fieldName()];
      
      if (empty($value[0]))
        return NULL;
      
      $parts = array();
      for ($i = 0; $i < 4; $i++)  
      {
        if ($this->hasFlag(AF_IP_ALLOW_WILDCARDS) && $value[$i] == '*')
          $parts[$i] = '*';
        else $parts[$i] = max(min((int)$value[$i], 255), 0);
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
    function edit($record, $fieldprefix)
    {
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
  }
?>