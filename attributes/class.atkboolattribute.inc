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
   * Make bool attribute obligatory (normal AF_OBLIGATORY flag is always removed).
   */
  define("AF_BOOL_OBLIGATORY", AF_SPECIFIC_1);
  
  /**
   * Show an extra label right next to the checkbox. ATK searches the language 
   * file for the following variants <attribute>_label, <attribute> (next to 
   * the module/node prefixes). Don't forget to add the AF_BLANK_LABEL flag
   * if you don't want to show the normal label.
   */
  define("AF_BOOL_INLINE_LABEL", AF_SPECIFIC_2);

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
      // Call base class constructor. Size of boolean value is always 1.
      $this->atkAttribute($name,$flags, 1);
      if ($this->hasFlag(AF_BOOL_OBLIGATORY))
      {
        $this->addFlag(AF_OBLIGATORY);
      }
    }
    
    function addFlag($flags)
    {
      // setting AF_OBLIGATORY has no use, so prevent setting it.
      if (hasFlag($flags, AF_OBLIGATORY)) 
      {
        $flags &= (~AF_OBLIGATORY);     
      }

      // except if someone really really really wants to show this attribute is obligatory
      if (hasFlag($flags, AF_BOOL_OBLIGATORY))
      {
        $flags |= AF_OBLIGATORY;
      } 
      
      parent::addFlag($flags);
    }

    /**
     * Is empty?
     *
     * @param array $record
     * @return boolean empty?
     */
    function isEmpty($record)
    {
      $empty = parent::isEmpty($record);

      // if bool_obligatory flag is set the value must be true else we treat this record as empty
      if ($this->hasFlag(AF_BOOL_OBLIGATORY) && !$record[$this->fieldName()])
        $empty = true;

      return $empty;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="", $fieldprefix="")
    {
      $id = $this->getHtmlId($fieldprefix);
      $onchange='';
      if (count($this->m_onchangecode))
      {
        $onchange = 'onClick="'.$id.'_onChange(this);" ';
        $this->_renderChangeHandler($fieldprefix);
      }
      $checked = "";
      if (isset($record[$this->fieldName()]) && $record[$this->fieldName()]>0) $checked="checked";
      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_CURSOR);
      
      $result = '<input type="checkbox" id="'.$id.'" name="'.$id.'" value="1" '.$onchange.$checked.' '.$this->getCSSClassAttribute("atkcheckbox").' />';
      
      if ($this->hasFlag(AF_BOOL_INLINE_LABEL))
      {
        $result .= '&nbsp;<label for="'.$id.'">'.$this->text(array($this->fieldName().'_label', $this->fieldName())).'</label>';
      }
      
      return $result;
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
    function search($record="", $extended=false, $fieldprefix="")
    {
      $result = '<select name="atksearch'.$fieldprefix.'['.$this->formName().']">';
      $result.= '<option value="">'.atktext("search_all", "atk");
      $result.= '<option value="0" ';
      if($record[$this->fieldName()]==='0' && !empty($record)) $result.="selected";
      $result.= '>'.atktext("no", "atk");
      $result.= '<option value="1" ';
      if($record[$this->fieldName()]==='1') $result.="selected";
      $result.= '>'.atktext("yes", "atk");
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
      if (isset($value)) return $query->exactCondition($table.".".$this->fieldName(),$this->escapeSQL($value));
    }

    /**
     * Returns a displayable string for this value.
     * @param $record Array with boolean field
     * @return yes or no
     */
    function display($record)
    {
	    // We display true (1) as 'Yes' and false as 'No'
	    if ($record[$this->fieldName()]>0) return atktext("yes", "atk");
	    return atktext("no", "atk");
    }
    
    /**
     * Get the HTML label of the attribute.
     *
     * The difference with the label() method is that the label method always
     * returns the HTML label, while the getLabel() method is 'smart', by
     * taking the AF_NOLABEL and AF_BLANKLABEL flags into account.
     *
     * @param array $record The record holding the value for this attribute.
     * @return String The HTML compatible label for this attribute, or an
     *                empty string if the label should be blank, or NULL if no
     *                label at all should be displayed.
     */
    function getLabel($record = array(), $mode='')
    {
      if ($mode == 'view' && $this->hasFlag(AF_BLANK_LABEL|AF_BOOL_INLINE_LABEL))
      {
        return $this->label($record);
      }
      else 
      {
        return parent::getLabel($record);
      }
    }    

    function fetchValue($postvars)
    {
      if (is_array($postvars)&&isset($postvars[$this->fieldName()]))
      {
        return $postvars[$this->fieldName()];
      }
      else
      {
        return 0;
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


    function hide($record="", $fieldprefix="")
    {
      // the next if-statement is a workaround for derived attributes which do
      // not override the hide() method properly. This will not give them a
      // working hide() functionality but at least it will not give error messages.
      if ($record[$this->fieldName()]=="")$record[$this->fieldName()] = "0";
      if (!is_array($record[$this->fieldName()]))
      {
        $result = '<input type="hidden" name="'.$fieldprefix.$this->formName().
                  '" value="'.htmlspecialchars($record[$this->fieldName()]).'">';
        return $result;
      }
      else atkdebug("Warning attribute ".$this->m_name." has no proper hide method!");
    }
  }
?>
