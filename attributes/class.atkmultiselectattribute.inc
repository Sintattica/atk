<?php

  /**
   * The atkMultiboolAttribute class represents an attribute of a node
   * that has a field with checkboxes, and stores the input seperated by a |
   *
   *
   * @author Rene Bakx (rene@ibuildings.nl)
   * @version $Revision$
   * @see atkListAttribute
   *
   */
  class atkMultiSelectAttribute extends atkListAttribute
  {    
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     * @param $optionArray Array with options
     * @param $valueArray Array with values. If you don't use this parameter, 
     *                    values are assumed to be the same as the options.
     */
    function atkMultiSelectAttribute($name, $optionArray, $valueArray="", $flags=0)
    {            
      $this->atkListAttribute($name, $optionArray, $valueArray, $flags); // base class constructor      
    }    


    /**
     * Substracts the postvars and returns a | seperated string 
     * @param $rec Array with values
     * @return String with slashes
     */
    function fetchValue($postvars)
    {
     
      //global $fieldprefix;
      //$form = $this->formName()."_nr";
      $nr = count($this->m_values);
      for ($i=0;$i<$nr;$i++)
      {  
        $field = $this->formName().$i;
        if ($postvars[$field] != "")
        {
          $string .=$postvars[$field]."|";
        } 
        
        
      }
      return substr($string,0,-1); // last | is stripped
    }
    
    function display($record)
    {
       $values = explode('|',$record[$this->fieldName()]);
       for ($i=0;$i<count($values);$i++)
       {
         $res[] = text($this->m_lookup[$values[$i]]);
       }
       return implode(', ',$res);
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
     
     $result .="\n<table><tr>\n";  
     $explode = explode("|",$record[$this->fieldName()]);
      for ($i=0;$i<count($this->m_values);$i++)
      {
        if (in_array($this->m_values[$i],$explode))
        {
          $sel = "checked"; 
        }
        else
        {
          $sel = "";
        }
        $result .= '<td class="table"><input type="checkbox" name="'.$fieldprefix.$this->formName().$i.'" value="'.$this->m_values[$i].'" '.$sel.'>'.text($this->m_options[$i]).'</td>';
        if ($i%3 == 2) $result .="</tr><tr>\n";
      }
      $result.="</tr></table>\n";
    
      return $result;
    }
    
    function searchCondition(&$query, $table, $value, $searchmode)
    {
      return $query->addCondition($query->substringCondition($table.".".$this->fieldName(),escapeSQL($value)));
    }

  }
?>
