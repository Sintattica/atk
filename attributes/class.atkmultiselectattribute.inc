<?php

  define(AF_NO_TOGGLELINKS, AF_SPECIFIC_1);
  
  useattrib("atklistattribute");
  
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
   
    // number of cols
    var $m_cols;

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $optionArray Array with options
     * @param $valueArray Array with values. If you don't use this parameter, 
     *                    values are assumed to be the same as the options.
     * @param $flags Flags for this attribute
     * @param $size  Size of the attribute.
     */
    function atkMultiSelectAttribute($name, $optionArray, $valueArray="", $cols="", $flags=0, $size="")
    {            
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
      ($cols<1)?$this->m_cols=3:$this->m_cols=$cols;
    }    

    function hide($record="", $fieldprefix="")
    {
      if(is_array($record[$this->fieldName()]))
      {
        for ($i=0;$i<count($this->m_values);$i++)
        {
          if(in_array($this->m_values[$i],$record[$this->fieldName()]))
          $result .= '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'[]"
                      value="'.$this->m_values[$i].'">';
        }
      } else parent::hide($record,$fieldprefix);
      return $result;
    }

    function value2db($rec)
    {
      //atkdebug("multiselectattribute::value2db()");
      if(is_array($rec[$this->fieldName()]) && count($rec[$this->fieldName()]>=1))
      {
        return escapeSQL(implode("|",$rec[$this->fieldName()]));
      }
      else return "";
    }
    
    function db2value($rec)
    {
      if($rec[$this->fieldName()]!="")
        return explode("|",$rec[$this->fieldName()]);
      else return array();
    }
    
    function display($record)
    {
       //$values = explode('|',$record[$this->fieldName()]);
       $values = $record[$this->fieldName()];
       $res=array();
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
      global $config_atkroot;
      $this->m_record = $record;
      $cols = $this->m_cols;
      $modcols =$cols-1;
   
      $id = $fieldprefix.$this->fieldName();   
      
      $page = &atkPage::getInstance();
      $page->register_script($config_atkroot."atk/javascript/class.atkprofileattribute.js.php");      

      if (count($this->m_values)>4 && !hasFlag($this->m_flags,AF_NO_TOGGLELINKS))
      {
        $result.='<div align="left"><font size="-2">
                  [<a href="javascript:profile_checkAll(\''.$fieldprefix.$this->fieldName().'\')">'.
                       text("check_all").
                  '</a> <a href="javascript:profile_checkNone(\''.$fieldprefix.$this->fieldName().'\')">'.
                       text("check_none").
                  '</a> <a href="javascript:profile_checkInvert(\''.$fieldprefix.$this->fieldName().'\')">'.
                       text("invert_selection").'</a>]</font></div>';
      } 
      $result .="\n<table><tr>\n";  
            
      for ($i=0;$i<count($this->m_values);$i++)
      {
        (atk_in_array($this->m_values[$i],$record[$this->fieldName()]))?$sel = "checked":$sel = "";
        $result .= '<td class="table" valign="top"><input type="checkbox" id="'.$id.'_'.$i.'" name="'.$fieldprefix.$this->fieldName().'[]" value="'.$this->m_values[$i].'" '.$sel.'>'.text($this->m_options[$i]).'</td>';
        $this->registerKeyListener($id.'_'.$i, KB_CTRLCURSOR|KB_UPDOWN);  

        if ($i%$cols == $modcols) $result .="</tr><tr>\n";
        
      }
      $result.="</tr></table>\n";
    
      return $result;
    }
    
    function searchCondition(&$query, $table, $value, $searchmode)
    {
      // Multiselect attribute has only 1 searchmode, and that is substring.
      
      if(is_array($value) && $value[0]!="" && count($value)>0)
      {
        if (count($value)==1)
        {
          return $query->addSearchCondition($query->substringCondition($table.".".$this->fieldName(),escapeSQL($value[0])));
        }
        else
        {
          foreach($value as $str)
          {
            $query->addSearchCondition($query->substringCondition($table.".".$this->fieldName(),escapeSQL($str)));
          }
        }
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