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
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * The atkListAttribute class represents an attribute of a node
   * that has a selectbox to select from predefined values. 
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkListAttribute extends atkAttribute
  {
    /**
     * Array with options for Listbox
     */
    var $m_options = array();

    /**
     * Array with values for Listbox
     */
    var $m_values = array();

    /**
     * Array for fast lookup of what value belongs to what option.
     */
    var $m_lookup = array();
    
    /**
     * Array for onchange code
     */
    var $m_onchangecode = array();
    
    /**
     * Attribute that is to be selected
     */
    var $m_selected;
    
    /**
     * Value that is used when list is empty, normally empty
     */
     var $m_emptyvalue;

    /**
     * Constructor.
     *
     * Warning: very old versions of this attribute supported passing the
     * parameters in a different order: $name, $flags, $optionArray.
     * This order used to be supported even when the new order was
     * implemented, but it has now been removed. Keep this in mind
     * when upgrading from a very old ATK version (pre ATK4).
     *
     * @param $name Name of the attribute
     * @param $optionArray Array with options
     * @param $valueArray Array with values. If you don't use this parameter,
     *                    values are assumed to be the same as the options.
     * @param $flags Flags for this attribute
     * @param $size  Size of the attribute.
     */
    function atkListAttribute($name, $optionArray, $valueArray="", $flags=0, $size=0)
    {
      if (!is_array($valueArray) || count($valueArray)==0)
      {
        $valueArray = $optionArray;
      }

      // If all values are numeric, we can use a numeric field to store the selected
      // value.
      $this->m_dbfieldtype = "number";
      for ($i=0, $_i=count($valueArray); $i<$_i && $this->m_dbfieldtype=="number"; $i++)
      {
        if (!is_numeric($valueArray[$i])) $this->m_dbfieldtype="string";
        // if one of the values is not a number, the fieldtype must be string, and
        // the loop is stopped.
      }

      // If no size is specified, the max size we have is equal to the biggest value.
      if ($size==0)
      {
        for ($i=0, $_i=count($valueArray); $i<$_i; $i++)
        {
          $size = max($size, strlen($valueArray[$i]));
        }
      }

      $this->atkAttribute($name, $flags, $size); // base class constructor
      $this->m_options = $optionArray;
      $this->m_values = $valueArray;

      for ($i=0, $_i=count($optionArray); $i<$_i; $i++)
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
      return text($this->m_lookup[$record[$this->fieldName()]], $this->m_ownerInstance->m_type);
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
      $id = $this->getHtmlId($fieldprefix);
      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_LEFTRIGHT);  
      
      $onchange='';
      if (count($this->m_onchangecode))
      {
        $onchange = 'onChange="'.$id.'_onChange(this);"';
        $this->renderChangeHandler($fieldprefix);
      }
      
      $result = '<select id="'.$id.'" name="'.$id.'" '.$onchange.'>';
      
      if(!$this->hasFlag(AF_OBLIGATORY))
      {
        $result.= '<option value="'.$this->m_emptyvalue.'">'.text('list_null_value', $this->m_ownerInstance->m_type);   
      }
      
      for ($i=0;$i<count($this->m_values);$i++)
      {
        if ($this->m_values[$i] == $this->m_selected || $this->m_values[$i] == $record[$this->fieldName()] && $record[$this->fieldName()]!="")
        {
          $sel = "selected";
        }
        else
        {
          $sel = "";
        }
        $result.= '<option value="'.$this->m_values[$i].'" '.$sel.'>'.$this->m_options[$i];
      }
      $result.='</select>';
      return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to search values
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function search($record="", $extended=false)
    {
      // todo: configurable rows
      $result = '<select ';
      if ($extended)
      {
        $result.='multiple size="'.min(5,count($this->m_values)+1).'"';
      }
      $result.='name="atksearch['.$this->formName().'][]">';
      $result.= '<option value="">'.text('search_all');

      for ($i=0,$_i=count($this->m_values);$i<$_i;$i++)
      {                
        if (atk_in_array(((string)$this->m_values[$i]),$record[$this->fieldName()], true) && $record[$this->fieldName()]!=="")
        {
          $sel = "selected";
        }
        else
        {
          $sel = "";
        }
        $result.= '<option value="'.$this->m_values[$i].'" '.$sel.'>'.text($this->m_options[$i], $this->m_ownerInstance->m_type);
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
      // We only support 'exact' matches.
      // But you can select more than one value, which we search using the IN() statement,
      // which should work in any ansi compatible database.
      if (is_array($value) && count($value)>0 && $value[0]!="") // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.
      {
        if (count($value)==1) // exactly one value
        {
          $query->addSearchCondition($query->exactCondition($table.".".$this->fieldName(),escapeSQL($value[0])));
        }
        else // search for more values using IN()
        {
          $query->addSearchCondition($table.".".$this->fieldName()." IN ('".implode("','",$value)."')");
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
      return array("exact");
    }

    function dbFieldType()
    {
      // Fieldtype was determined in the constructor.
      return $this->m_dbfieldtype;
    }
    
    /** 
     * Add a javascript onchange event handler.
     * @param $jscode A block of valid javascript code.
     */
    function addOnChangeHandler($jscode)
    {
      $this->m_onchangecode[] = $jscode;
    }
    
    function renderChangeHandler($fieldprefix)
    {
      if (count($this->m_onchangecode))
      {
        $page = &$this->m_ownerInstance->getPage();
        $page->register_scriptcode("function ".$this->getHtmlId($fieldprefix)."_onChange(el)
                                    {
                                      newvalue = el.options[el.selectedIndex].value;
                                      ".implode("\n", $this->m_onchangecode)."
                                    }\n");
      }
    }
    
    function setAutoHide($attrib, $valuearr)
    {
      $conditions = array();
      foreach ($valuearr as $value)
      {
        $conditions[] = "newvalue=='$value'";
      }
      $this->addOnChangeHandler("if (".implode('||', $conditions).") hideAttrib('$attrib'); else showAttrib('$attrib');");
    }
    
    /**
     * Sets the selected listitem
     * @param string $selected the listitem you want to have selected
     */
    function setSelected($selected)
    {
      $this->m_selected = $selected;
    }
    
    /**
     * Gets the selected listattribute
     * @return string the selected listitem
     */
    function getSelected()
    {
      return $this->m_selected;
    }
    
    /**
     * Sets the value for the empty entry in the list attribute
     * In normal cases you would just leave this empty, but certain cases
     * might demand you set a value.
     * @param string $value the value we set for empty value
     */
    function setEmptyValue($value)
    {
      $this->m_emptyvalue = $value;
    }
    
    /**
     * Gets the value for the empty entry in the list attribute
     * @return string 
     */
    function getEmptyValue()
    {
      return $this->m_emptyvalue;
    }
    
    /**
     * Convert a String representation into an internal value.
     *
     * This implementation search for the value first in the valueArray, then in the optionArray
     * All other values are converted to the first of the valueArray
     *
     * @param String $stringvalue The value to parse.
     * @return Internal value (from valueArray)
     */
    function parseStringValue($stringvalue)
    {
      foreach($this->m_values as $value)
        if(strtolower($stringvalue) == strtolower($value))
          return $value;

      $i=0;
      foreach($this->m_options as $option)
      {
        if(strtolower($stringvalue) == strtolower($option))
          return $this->m_values[$i];

        if(strtolower(text($stringvalue)) == strtolower($option))
          return $this->m_values[$i];
        $i++;
      }

      return $this->m_value[0];
    }
  }

?>
