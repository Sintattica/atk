<?php

  include_once($config_atkroot."atk/attributes/class.atklistattribute.inc");

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
   
  // number of rows
  var $m_rows;

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $optionArray Array with options
     * @param $valueArray Array with values. If you don't use this parameter, 
     *                    values are assumed to be the same as the options.
     * @param $flags Flags for this attribute
     * @param $size  Size of the attribute.
     */
    function atkMultiSelectAttribute($name, $optionArray, $valueArray="",$rows, $flags=0, $size="")
    {            
      // compatiblity with old versions
      if (func_num_args()>5) $this->m_rows=$rows;
      else                   
      {
        $flags = $rows;
        $this->m_rows=3;
      }      
      
      if (!is_array($valueArray) || count($valueArray)==0)
      {
        $valueArray = $optionArray;
      }
      
      // size must be large enough to store a combination of all values.
      if ($size == "")
      {
        $size=0;
        for ($i=0, $_i=count($valueArray); $i<$_i; $i++)
        {
          $size+=(strlen($valueArray[$i])+1); // 1 extra for the '|' symbol
        }        
      }
      $this->atkListAttribute($name, $optionArray, $valueArray, $flags, $size); // base class constructor      
      
      $this->m_dbfieldtype = "string"; // since we store values separated by '|', we always store as string.
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
    global $g_layout,$config_atkroot;
    $rows = $this->m_rows;
    $modRows =$rows-1;
    $g_layout->register_script($config_atkroot."atk/javascript/class.atkprofileattribute.js.php");
     // todo: configurable rows
     if (count($this->m_values)>4)
     {
       $result.='<div align="left"><font size="-2">
                 [<a href="javascript:profile_checkAll(\''.$this->fieldName().'\')">'.
                      text("check_all").
               '</a> <a href="javascript:profile_checkNone(\''.$this->fieldName().'\')">'.
                      text("check_none").
               '</a> <a href="javascript:profile_checkInvert(\''.$this->fieldName().'\')">'.
                      text("invert_selection").'</a>]</font></div>';
     } 
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
        $result .= '<td class="table" valign="top"><input type="checkbox" name="'.$fieldprefix.$this->formName().$i.'" value="'.$this->m_values[$i].'" '.$sel.'>'.text($this->m_options[$i]).'</td>';
        if ($i%$rows == $modRows) $result .="</tr><tr>\n";
      }
      $result.="</tr></table>\n";
    
      return $result;
    }
    
   
    function searchCondition(&$query, $table, $value, $searchmode)
    {
      // Multiselect attribute has only 1 searchmode, and that is substring.
      
      if(is_array($value) && $value[0]!="" && count($value)>0)
      {
        return $query->addSearchCondition($query->substringCondition($table.".".$this->fieldName(),escapeSQL($value[0])));
      }
    }
    
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array("substring");
    }  
    
  }
?>
