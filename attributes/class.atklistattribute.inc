<?php

  /**
   * The atkListAttribute class represents an attribute of a node
   * that has a selectbox to select from predefined values.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
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
      $result = '<select name="'.$fieldprefix.$this->formName().'">';
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
        $result.= '<option value="'.$this->m_values[$i].'" '.$sel.'>'.text($this->m_options[$i], $this->m_ownerInstance->m_type);
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
        if (atk_in_array($this->m_values[$i],$record[$this->fieldName()]) && $record[$this->fieldName()]!="")
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

  }
?>
