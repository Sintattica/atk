<?php

  /**
   * The atkRadioAttribute class represents an attribute of a node
   * that has a field with radio button  to select from predefined values.
   * @author Rene Bakx (rene@ibuildings.nl)
   * @version 1.0
   *
  * This function is allmost a 100% copy from atkListAttribute, with some slight modification to show radiobuttons 
  * instead of a listbox
   */
  class atkRadioAttribute extends atkAttribute
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
    function atkRadioAttribute($name, $optionArray, $valueArray="", $flags=0)
    {
      
      // API CHANGE HACK
      // This function used to have the parameters: ($name, $flags=0, $optionArray, 
      // $valueArray="")
      // It was changed so atkRadioAttribute is more like the other attributes. 
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
    function edit($record="", $fieldprefix="")
    {
      // todo: configurable rows
     $result ="<p>";  
      for ($i=0;$i<count($this->m_values);$i++)
      {
        if ($this->m_values[$i] == $record[$this->fieldName()] && $record[$this->fieldName()]!="")
        {
          $sel = "checked"; 
        }
        else
        {
          $sel = "";
        }
        $result .= '<input type="radio" name="'.$fieldprefix.$this->formName().'" value="'.$this->m_values[$i].'" '.$sel.'>'.text($this->m_options[$i]);
      }
      $result.='</P>';
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
    
    /**
     * Creates an search condition for a given search value
     * @param $table the table name
     * @param $value the search value
     * @return a piece of the SQL where clause
     */
    function searchCondition(&$query, $table, $value, $searchmode)
    {
      return $query->addCondition($query->exactCondition($table.".".$this->fieldName(),escapeSQL($value)));
    }    
  }
?>
