<?php

  /**
   * The atkFlagAttribute class offers an way to use flags in an attribute.
   * @author  Lineke Willems <lineke@ibuildings.nl>
   *
   * @version $Revision$
   *
   * $Id$
   */

  // TODO: Test this attribute and bring back the parameters to one array in stead of two arrays

  class atkFlagAttribute extends atkAttribute
  {
    /**
     * Array with checkbox options
     */
    var $m_options = Array();

    /**
     * Array with checkbox values
     */
    var $m_values = Array();

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     * @param $optionArray Array with options
     * @param $valueArray Array with flags
     */
    function atkFlagAttribute($name, $optionArray, $valueArray="", $flags=0, $size=0)
    {
      // For flags, we use a default size of 10. (PHP can only handle 32 bit flags,
      // so 10 positions is enough to store the bits of a PHP flag set).
      if ($size==0) $size=10;
      
      $this->atkAttribute($name,$flags, $size); // base class constructor
      $this->m_options = $optionArray;
      $this->m_values = $valueArray;
    }

    function edit($record="", $fieldprefix="")
    {
      $result = "";
      for ($i=0;$i<count($this->m_values);$i++)
      {
        if ((hasFlag($record[$this->fieldName()], $this->m_values[$i])) && $record[$this->fieldName()]!="")
        {
          $sel = "checked";
        }
        else
        {
          $sel = "";
        }
        $result.= '<input type=checkbox name="'.$fieldprefix.$this->formName().'[]" value="'.$this->m_values[$i].'" '.$sel.'>'.text($this->m_options[$i], $this->m_ownerInstance->m_type);
      }
      return $result;
    }

    /**
     * Convert checkbox array to database value
     * @param $rec array with x fields (for each checkbox a field)
     * @return database value for role
     */
    function fetchValue($rec)
    {
      $rolenumber = 0;

      for ($i=0;$i<count($rec[$this->fieldName()]);$i++)
      {
        $rolenumber += $rec[$this->fieldName()][$i];
      }
      return $rolenumber;
    }

    /**
     * Function display's the role in text
     * @param $record array with roles
     * @return formatted role string array
     */
    function display($record)
    {
      $value = $record[$this->fieldName()];
      for ($j=0;$j<count($this->m_values);$j++)
      {
        if (hasFlag($record[$this->fieldName()], $this->m_values[$j]))
          $rolestring .= text($this->m_options[$j])." ";
      }
      return $rolestring;
    }
    
    function dbFieldType()
    {
      return "number";
    }
  }

?>