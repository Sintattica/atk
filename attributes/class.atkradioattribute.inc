<?php

  include_once($config_atkroot."atk/attributes/class.atklistattribute.inc");

  /**
   * The atkRadioAttribute class represents an attribute of a node
   * that has a field with radio button  to select from predefined values.
   *
   * This attribute is almost identical to atkListAttribute, 
   * with some slight modification to show radiobuttons instead of a listbox
   *
   * @author Rene Bakx (rene@ibuildings.nl)
   * @version $Revision$
   * @see atkListAttribute
   *
   */
  class atkRadioAttribute extends atkListAttribute
  {    
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
      $this->atkListAttribute($name, $optionArray, $valueArray, $flags); // base class constructor      
    }    

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with radioboxes
     */
    function edit($record="", $fieldprefix="")
    {
      // todo: configurable rows
     //$result ="<p>";  
     $result ='';
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
        $result .= '<input type="radio" name="'.$fieldprefix.$this->formName().'" value="'.$this->m_values[$i].'" '.$sel.'>'.text($this->m_options[$i]).'&nbsp; ';
      }
      //$result.='</P>';
      return $result;
    }
  }
?>
