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
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /** Flag(s) specific for atkRadioAttribute */
  define("AF_DISPLAY_VERT", AF_SPECIFIC_2); // erm, dislpays a set of radio buttons vertically

  /** @internal Include base class */
  include_once($config_atkroot."atk/attributes/class.atklistattribute.inc");

  /**
   * The atkRadioAttribute class represents an attribute of a node
   * that has a field with radio button  to select from predefined values.
   *
   * This attribute is almost identical to atkListAttribute, 
   * with some slight modification to show radiobuttons instead of a listbox
   *
   * @author Rene Bakx <rene@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkRadioAttribute extends atkListAttribute
  {  
    // Default number of cols / rows  
    var $m_amount = 1;
    var $m_cols=false;
    var $m_rows=false;
    
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     * @param $optionArray Array with options
     * @param $valueArray Array with values. If you don't use this parameter, 
     *                    values are assumed to be the same as the options.
     * @param $size database field size ($size[1] can be used for the amount of cols / rows to display, for example: 3c or 5r or just 4)
     *            
     */
    function atkRadioAttribute($name, $optionArray, $valueArray="", $flags=0, $size=0)
    {            
      if (is_array($size) and count($size)>1)
      {
        $lastchar = strtolower(substr($size[1],-1,1));
        atkdebug("Last: $lastchar");
        if($lastchar=="c")
        {
          $this->m_cols=true;
          $this->m_amount=substr($size[1],0,-1);
        }
        elseif($lastchar=="r")
        {
          $this->m_rows=true;
          $this->m_amount=substr($size[1],0,-1);
        }
        else
        {
          // Default options
          if($this->hasFlag(AF_DISPLAY_VERT)) { $this->m_rows=true; } else { $this->m_cols=true; } 
          $this->m_amount=$size[1];
        }
      }
      $this->atkListAttribute($name, $optionArray, $valueArray, $flags,$size); // base class constructor      
    }    

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with radioboxes
     */
    function edit($record="", $fieldprefix="")
    {
      
      $total_items = count($this->m_values);
      if($this->m_cols && !$this->hasFlag(AF_DISPLAY_VERT))
      {
        $items=$this->m_amount;
      }
      elseif($this->m_rows && !$this->hasFlag(AF_DISPLAY_VERT))
      {
        $items = ceil($total_items/$this->m_amount);
      }
      elseif($this->m_cols && $this->hasFlag(AF_DISPLAY_VERT))
      {
        $items=ceil($total_items/$this->m_amount);
        $total_items=$items;
      }
      elseif($this->m_rows && $this->hasFlag(AF_DISPLAY_VERT))
      {
        $items = $this->m_amount;
        $total_items=$this->m_amount;
      }
      
      $result ='<table class="table">';
      if(!$this->hasFlag(AF_DISPLAY_VERT)) $result.='<tr>';
      $item_count=0;
      for ($i=0;$i<$total_items;$i++)
      {
        if ($this->m_values[$i] == $record[$this->fieldName()] && $record[$this->fieldName()]!="")
        {
          $sel = "checked"; 
        }
        else
        {
          $sel = "";
        }
        if($record[$this->fieldName()]=="" && $i==0 && $this->hasFlag(AF_OBLIGATORY))
        {
          $sel = "checked";
        }
        

        $labelID = $fieldprefix.$this->formName()."_".$this->m_values[$i];
        if($this->hasFlag(AF_DISPLAY_VERT)) $result.='<tr>';
        $this->registerKeyListener($labelID, KB_CTRLCURSOR|KB_CURSOR);
        $result .= '<td><input id="'.$labelID.'" type="radio" name="'.$fieldprefix.$this->formName().'" value="'.$this->m_values[$i].'" '.$sel.'>
        <label for="'.$labelID.'">'.text($this->m_options[$i]).'</label></td>';
        if($this->hasflag(AF_DISPLAY_VERT))
        {
          $tmp_items=$items;
          if($this->hasFlag(AF_DISPLAY_VERT)&&$this->m_rows) 
          {
            $tmp_items=count($this->m_values);
          }
          else
          {
            $tmp_items = $items*$this->m_amount;
          }
          
          for($j=($items+$i);$j<$tmp_items;$j=$j+$items)
          {
            if ($this->m_values[$j] == $record[$this->fieldName()] && $record[$this->fieldName()]!="")
            {
              $sel = "checked"; 
            }
            else
            {
              $sel = "";
            }
            if($this->m_values[$j]!="")
            {
              $result .= '<td><input id="'.$labelID.'" type="radio" name="'.$fieldprefix.$this->formName().'" value="'.$this->m_values[$j].'" '.$sel.'>
              <label for="'.$labelID.'">'.text($this->m_options[$j]).'</label></td>';
            }
            else
            {
              $result .='<td>&nbsp;</td>';
            }
          }
          $result.='</tr>';
        }
       
        $item_count++;
        if($item_count==$items && !$this->hasFlag(AF_DISPLAY_VERT))
        {
          $result.='</tr><tr>';
          $item_count=0;
        }
        
      }
      // Fill with empty boxes when we have a horizontal display
      if(!$this->hasFlag(AF_DISPLAY_VERT))
      {      
        for($i=0;$i<($items-$item_count);$i++) $result.='<td>&nbsp;</td>';
        $result.='</tr>';
      }
      $result.='</table>';
      return $result;
    }
  }
?>
