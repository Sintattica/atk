<?php

  /**
   * The atkListAttribute class represents an attribute of a node
   * that has a selectbox to select from predefined values.
   * @author Ivo Jansch (ivo@achievo.com)
   * @version 1.3
   *
   * <b>@changes</b>
   *
   *  1.3   - fixed bug where listattrib would select item with value 0 if
   *          current value was "" (0=="" gives true in php for some 
   *          reason. (Ivo)
   *  1.2   - solved bug when no value array was specified (Ivo)
   *  1.1   - added an array to store values. options and values can now 
   *          differ (Ivo)
   *
   * $Id$
   * $Log$
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:08  ivo
   * Added atk4 to repository.
   *
   * Revision 1.3  2001/03/28 09:53:36  ivo
   * valueArray will now be ignored if it's an empty array.
   *
   * Revision 1.2  2001/02/22 22:46:18  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:57:58  sandy
   * Achievo Tool Kit
   *
   */
  class atkListAttribute extends atkAttribute
  {
    /**
     * Array with options for Listbox  
     */
    var $m_options = Array();
    
    /**
     * Array with values for Listbox  
     */    
    var $m_values = Array();

    /**
     * Array for fast lookup of what value belongs to what option.
     */
    var $m_lookup = Array();

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     * @param $optionArray Array with options
     * @param $valueArray Array with values. If you don't use this parameter, 
     *                    values are assumed to be the same as the options.
     */
    function atkListAttribute($name, $optionArray, $valueArray="", $flags=0)
    {
      
      // API CHANGE HACK
      // This function used to have the parameters: ($name, $flags=0, $optionArray, 
      // $valueArray="")
      // It was changed so atkListAttribute is more like the other attributes. 
      // However, to remain backward compatible with modules based on the old api
      // we detect if the parameters are in the wrong order, and if so, switch 'm.
      if (is_int($optionArray))
      {
        // Old api use detected...
        $tmp = $flags;
        $flags = $optionArray;
        $optionArray = $valueArray;
        $valueArray = $tmp; 
      }

      $this->atkAttribute($name,$flags); // base class constructor
      $this->m_options = $optionArray;
 
      if (is_array($valueArray) && count($valueArray)>0)
      {
        $this->m_values = $valueArray;
      }
      else
      {
        $this->m_values = $optionArray;
      }
      
      for ($i=0;$i<count($optionArray);$i++)
      {
        $this->m_lookup[$this->m_values[$i]] = $optionArray[$i];
      }
    }


    /**
     * Display's text version of Record
     * @param $record
     * @return text string of $record
     */
    function display($record)
    {     
      return text($this->m_lookup[$record[$this->fieldName()]]);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="")
    {
      // todo: configurable rows
      $result = '<select name="'.$this->formName().'">';
      for ($i=0;$i<count($this->m_values);$i++)
      {
        if ($this->m_values[$i] == $record[$this->fieldName()] && $record[$this->fieldName()]!="")
        {
          $sel = "selected"; 
        }
        else
        {
          $sel = "";
        }
        $result.= '<option value="'.$this->m_values[$i].'" '.$sel.'>'.text($this->m_options[$i]);
      }
      $result.='</select>';
      return $result;
    }
    
    /**
     * Returns a piece of html code that can be used in a form to search values   
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function search($record="")
    {      
      // todo: configurable rows
      $result = '<select name="atksearch['.$this->formName().']">';
      $result.= '<option value="">'.text('search_all');
      for ($i=0;$i<count($this->m_values);$i++)
      {
        if ($this->m_values[$i] == $record[$this->fieldName()] && $record[$this->fieldName()]!="")
        {         
          $sel = "selected"; 
        }
        else 
        {
          $sel = "";
        }
        $result.= '<option value="'.$this->m_values[$i].'" '.$sel.'>'.text($this->m_options[$i]);
      }
      $result.='</select>';
      return $result;
    }
  }
?>
