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
   * The atkBoolAttribute class represents an attribute of a node
   * that can either be true or false.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkBoolAttribute extends atkAttribute
  {
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function atkBoolAttribute($name, $flags=0)
    {
      //Setting AF_OBLIGATORY has no use, so prevent setting it.
      if (($flags & AF_OBLIGATORY) == AF_OBLIGATORY) $flags &= (~ AF_OBLIGATORY);          
            
     // Call base class constructor. Size of boolean value is always 1.
      $this->atkAttribute($name,$flags, 1); 
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="", $fieldprefix="")
    {
      $checked = "";
      if (isset($record[$this->fieldName()]) && $record[$this->fieldName()]>0) $checked="checked";
      $id = $fieldprefix.$this->fieldName();
      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_CURSOR);  
      return '<input type="checkbox" id="'.$id.'" name="'.$id.'" value="1" '.$checked.'>';
    }
    

    /**
     * Get the value if it exits, otherwise return 0
     * @param $rec Array with values
     * @return String with slashes
     */
    function value2db($rec)
    {   
      (isset($rec[$this->fieldName()]) ? $ret =  $rec[$this->fieldName()]: $ret = 0);
      return $ret;
    }


    /**
     * Returns a piece of html code that can be used in a form to search for values   
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function search($record="")
    {
      $result = '<select name="atksearch['.$this->formName().']">';
      $result.= '<option value="">'.text('search_all');
      $result.= '<option value="0" ';
      if($record[$this->fieldName()]==0 && !empty($record)) $result.="selected";
      $result.= '>'.text('no');
      $result.= '<option value="1" ';
      if($record[$this->fieldName()]==1) $result.="selected";
      $result.= '>'.text('yes');
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
      $query->addSearchCondition($this->getSearchCondition($query, $table, $value, $searchmode));
    }     
    
    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      if (is_array($value)) $value = $value[$this->fieldName()];
      if ($value) return $query->exactCondition($table.".".$this->fieldName(),escapeSQL($value));
    }

    /**
     * Returns a displayable string for this value.
     * @param $record Array with boolean field
     * @return yes or no
     */
    function display($record)
    {
	    // We display true (1) as 'Yes' and false as 'No'
	    if ($record[$this->fieldName()]>0) return text("yes");
	    return text("no");
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
      return "number";
    }

    /**
     * Return the label of the field.
     */
    function label($record = array())
    { 
      $label_txt = parent::label($record);
      return '<label for="'.$this->m_name.'">'.$label_txt.'</label>';
    }
    
    /**
     * Convert a String representation into an internal value.
     *
     * This implementation converts 'y/j/yes/on/true/1/*' to 1
     * All other values are converted to 0
     *
     * @param String $stringvalue The value to parse.
     * @return boolean Internal value
     */
    function parseStringValue($stringvalue)
    {
      if (in_array(strtolower($stringvalue), array("y","j","yes","on","true","1","*"))) return 1;
      return 0;
    }

    
  }
?>
