<?php

  /**
   * The atkFlagAttribute class offers an way to use flags in an attribute.
   * @author  M. Roest <martin@ibuildings.nl>
   *
   * @version $Revision$
   *
   * $Id$
   */

  useattrib("atkmultiselectattribute");
  
  class atkFlagAttribute extends atkMultiSelectAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     * @param $optionArray Array with options
     * @param $valueArray Array with flags
     */
    function atkFlagAttribute($name, $optionArray, $valueArray="", $cols="", $flags=0, $size="")
    {
      $this->atkMultiSelectAttribute($name, $optionArray, $valueArray, $cols, $flags, $size);
      $this->m_dbfieldtype = "number";
    }

    function value2db($rec)
    {
      //atkdebug("atkFlagAttribute::value2db()");
      if(is_array($rec[$this->fieldName()]) && count($rec[$this->fieldName()]>=1))
      {
        $flags=0;
        foreach($rec[$this->fieldName()] as $flag)
        {
          $flags |= $flag;
        }
        return $flags;
      }
      else return 0;
    }
    
    function db2value($rec)
    {
      if($rec[$this->fieldName()]>0)
      {
        $newrec = array();
        foreach($this->m_values as $value)
        {
          if(hasFlag($rec[$this->fieldName()],$value))
          {
            $newrec[] = $value;
          }
        }
        return $newrec;
      }
      return array();
    }
    
    function dbFieldType()
    {
      return $this->m_dbfieldtype;
    }

    function searchCondition(&$query, $table, $value, $searchmode)
    {
      // not implemented
    }
  
    function getSearchModes()
    {
      // currently you can't search the flag attribute
    
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      // return array("exact");
    }  
  }
?>