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

  /**
   * @internal Include base class
   */
  atkimport("atk.attributes.atklistattribute");

  /**
   * Flag(s) specific for atkRadioAttribute
   */

  /**
   * Displays the set of radio buttons vertically
   */
  define("AF_DISPLAY_VERT", AF_SPECIFIC_2);

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
    var $m_clickableLabel=true;
    
    /**
     * Array with comments per option
     *
     * @var array
     */
    var $m_comments = array();

    var $m_onchangehandler_init = "var newvalue = el.value;\n";

    /**
     * Constructor
     *
     * @param String $name Name of the attribute
     * @param array $optionArray Array with options
     * @param array $valueArray Array with values. If you don't use this parameter,
     *                    values are assumed to be the same as the options.
     * @param int $flags Flags for this attribute
     * @param int $size database field size ($size[1] can be used for the amount of cols / rows to display, for example: 3c or 5r or just 4)
     *
     */
    function atkRadioAttribute($name, $optionArray, $valueArray="", $flags=0, $size=0)
    {
      if (is_array($size) and count($size)>1)
      {
        $lastchar = strtolower(substr($size[1],-1,1));
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
     * Set comment for a specific option
     *
     * @param string $option The option the comment is for
     * @param string $comment The comment itself
     */
    function setComment($option, $comment)
    {
      $key = array_search($option, $this->m_options);
      $this->m_comments[$key] = $comment;
    }
    
    /**
     * Set clickablelabel for the radioattribute
     *
     * @param boolean $label
     */
    function setClickableLabel($label=true)
    {
      $this->m_clickableLabel=$label;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param array $record Array with fields
     * @return String piece of html code with radioboxes
     */
    function edit($record="", $fieldprefix="")
    {
      $values = $this->getValues($record);
      
      $total_items = count($values);
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
        if ($values[$i] == $record[$this->fieldName()] && $record[$this->fieldName()]!=="")
        {
          $sel = "checked";
        }
        else
        {
          $sel = "";
        }

        $labelID = $fieldprefix.$this->formName()."_".$values[$i];
        if($this->hasFlag(AF_DISPLAY_VERT)) $result.='<tr>';
        $this->registerKeyListener($labelID, KB_CTRLCURSOR|KB_CURSOR);
        $id = $this->getHtmlId($fieldprefix);

        $onchange='';
        if (count($this->m_onchangecode))
        {
          $onchange = 'onClick="'.$id.'_onChange(this);" ';
          $this->_renderChangeHandler($fieldprefix);
        }
        
        $commenthtml = '<br/><div class="atkradio_comment">'.$this->m_comments[$i].'</div>';

        $result .= '<td><input id="'.$labelID.'" type="radio" name="'.$fieldprefix.$this->formName().'" '.$this->getCSSClassAttribute("atkradio").' value="'.$values[$i].'" '.$onchange.$sel.'>
        '.$this->renderValue($labelID,$this->_translateValue($values[$i],$record)).($this->hasFlag(AF_DISPLAY_VERT) && $this->m_comments[$i] != '' ? $commenthtml : '').'</td>';
        
        if($this->hasflag(AF_DISPLAY_VERT))
        {
          $tmp_items=$items;
          if($this->hasFlag(AF_DISPLAY_VERT)&&$this->m_rows)
          {
            $tmp_items=count($values);
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
            if($values[$j]!="")
            {
              $result .= '<td><input id="'.$labelID.'" type="radio" name="'.$fieldprefix.$this->formName().'" '.$this->getCSSClassAttribute("atkradio").' value="'.$values[$j].'" '.$onchange.$sel.'>
              '.$this->renderValue($labelID,$this->_translateValue($values[$j],$record)).($this->m_comments[$i] != '' ? $commenthtml : '').'</td>';
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
    
    /**
     * Render value with or without a clickable label
     *
     * @param string $labelID Label ID
     * @param string $value Label value
     * @return string Label
     */
    function renderValue($labelID,$value)
    {
      if($this->m_clickableLabel)
      {
        return '<label for="'.$labelID.'">'.$value.'</label>';
      }
      return $value;
    }
    
  }
?>