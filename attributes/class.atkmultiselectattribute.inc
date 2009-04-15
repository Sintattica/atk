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
   * @version $Revision: 6305 $
   * $Id$
   */

  /** @internal Include base class */
  useattrib("atklistattribute");

  /** Defines */
  define("AF_NO_TOGGLELINKS", AF_SPECIFIC_2);
  define("AF_CHECK_ALL", AF_SPECIFIC_3);
  define("AF_LINKS_BOTTOM", AF_SPECIFIC_4);

  /**
   * The atkMultiboolAttribute class represents an attribute of a node
   * that has a field with checkboxes, and stores the input seperated by a '|'
   *
   * @author Rene Bakx <rene@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkMultiSelectAttribute extends atkListAttribute
  {
    // number of cols
    public $m_cols;

    /**
     * Default field separator
     *
     * @var string separator
     */
    protected $m_fieldSeparator = "|";

    /**
     * Constructor
     * @param string $name Name of the attribute
     * @param array $optionArray Array with options
     * @param array $valueArray Array with values. If you don't use this parameter,
     *                    values are assumed to be the same as the options.
     * @param int $cols Number of columns
     * @param int $flags Flags for this attribute
     * @param int $size  Size of the attribute.
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
          $size+=(atk_strlen($valueArray[$i])+1); // 1 extra for the '|' symbol
        }
      }
      $this->atkListAttribute($name, $optionArray, $valueArray, $flags, $size); // base class constructor
      ($cols<1)?$this->m_cols=3:$this->m_cols=$cols;
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">)
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode with hidden form elements that post
     *                this attribute's value without showing it.
     */
    function hide($record="", $fieldprefix="")
    {
      if(is_array($record[$this->fieldName()]))
      {
        $values = $this->getValues($record);
        for ($i=0;$i<count($values);$i++)
        {
          if(in_array($values[$i],$record[$this->fieldName()]))
          $result .= '<input type="hidden" name="'.$fieldprefix.$this->fieldName().'[]"
                      value="'.$values[$i].'">';
        }
      } else parent::hide($record,$fieldprefix);
      return $result;
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * @param array $rec The record that holds this attribute's value.
     * @return String The database compatible value
     */
    function value2db($rec)
    {
      //atkdebug("multiselectattribute::value2db()");
      if(is_array($rec[$this->fieldName()]) && count($rec[$this->fieldName()]>=1))
      {
        return $this->escapeSQL(implode($this->m_fieldSeparator,$rec[$this->fieldName()]));
      }
      else return "";
    }

    /**
     * Converts a database value to an internal value.
     *
     * @param array $rec The database record that holds this attribute's value
     * @return mixed The internal value
     */
    function db2value($rec)
    {
      if (isset($rec[$this->fieldName()]) && $rec[$this->fieldName()]!=='')
        return explode($this->m_fieldSeparator,$rec[$this->fieldName()]);
      else return array();
    }

    /**
     * Allows you to set the field separator
     * which is used to separate values in the
     * database. i.e use a comma separator for
     * mysql set datatype
     *
     * @param string $separator
     * @return void
     */
    public function setFieldSeparator($separator)
    {
      $this->m_fieldSeparator = $separator;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists, "edit" for
     *                     displaying in editscreens, "add" for displaying in
     *                     add screens. "csv" for csv files. Applications can
     *                     use additional modes.
     * @return String HTML String
     */
    function display($record, $mode="")
    {
       $values = $record[$this->fieldName()];
       $res=array();
       for ($i=0;$i<count($values);$i++)
       {
         $res[] = $this->_translateValue($values[$i],$record);
       }
       return implode(', ',$res);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return piece of html code with radioboxes
     */
    function edit($record="", $fieldprefix="", $mode="")
    {
      global $config_atkroot;
      $this->m_record = $record;
      $cols = $this->m_cols;
      $modcols =$cols-1;

      $id = $fieldprefix.$this->fieldName();

      $page = &atkPage::getInstance();
      $page->register_script($config_atkroot."atk/javascript/class.atkprofileattribute.js.php");

      $result = "";
      if (!$this->hasFlag(AF_LINKS_BOTTOM)) $result.=$this->_addLinks($fieldprefix);

      $result .="\n<table><tr>\n";

      $values = $this->getValues($record);
      if(!is_array($record[$this->fieldname()]))
        $recordvalue = $this->db2value($record);
      else
        $recordvalue = $record[$this->fieldName()];

      for ($i=0;$i<count($values);$i++)
      {
        if (!$this->hasFlag(AF_CHECK_ALL))
        {
          (atk_in_array($values[$i],$recordvalue))?$sel = "checked":$sel = "";
        }
        else
        {
          $sel = "checked";
        }

        $result .= '<td class="table" valign="top"><input type="checkbox" id="'.$id.'_'.$i.'" '.$this->getCSSClassAttribute("atkcheckbox").' name="'.$fieldprefix.$this->fieldName().'[]" value="'.$values[$i].'" '.$sel.'>'.$this->_translateValue($values[$i],$record).'</td>';
        $this->registerKeyListener($id.'_'.$i, KB_CTRLCURSOR|KB_UPDOWN);

        if ($i%$cols == $modcols) $result .="</tr><tr>\n";
      }
      $result.="</tr></table>\n";
      if ($this->hasFlag(AF_LINKS_BOTTOM))$result.=$this->_addLinks($fieldprefix);

      return $result;
    }

    /**
     * @todo code below can't possibly work.
     *  really needs to be fixed.
     *
     * @param atkquery $query
     * @param string $table
     * @param mixed $value
     * @param string $searchmode
     * @return string condition to use in a where clause
     */
    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      // Multiselect attribute has only 1 searchmode, and that is substring.

      if(is_array($value) && $value[0]!="" && count($value)>0)
      {
        if (count($value)==1)
        {
          $searchcondition = $query->substringCondition($table.".".$this->fieldName(),$this->escapeSQL($value[0]));
        }
        else
        {
        foreach($value as $str)
        {
          $searchcondition = $query->substringCondition($table.".".$this->fieldName(),$this->escapeSQL($str));
        }
      }
      }
      return $searchcondition;
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.
     */
    function dbFieldType()
    {
      if (empty($this->m_dbfieldtype))
      {
        return 'text';
      }
      return $this->m_dbfieldtype;
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * @return array List of supported searchmodes
     */
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array("substring");
    }

    /**
     * Add the checkall, checknone and checkinvert links
     *
     * @param string $fieldprefix The fieldprefix
     * @return a piece of htmlcode with the links
     */
    function _addLinks($fieldprefix)
    {
      if (count($this->m_values)>4 && !hasFlag($this->m_flags,AF_NO_TOGGLELINKS))
      {
        return '<div align="left"><font size="-2">
                  [<a href="javascript:void(0)" onclick="profile_checkAll(\''.$fieldprefix.$this->fieldName().'\'); return false;">'.
                       atktext("check_all").
                  '</a> <a href="javascript:void(0)" onclick="profile_checkNone(\''.$fieldprefix.$this->fieldName().'\'); return false;">'.
                       atktext("check_none").
                  '</a> <a href="javascript:void(0)" onclick="profile_checkInvert(\''.$fieldprefix.$this->fieldName().'\'); return false;">'.
                       atktext("invert_selection").'</a>]</font></div>';
      }
    }

    /**
     * Check if a record has an empty value for this attribute.
     * @param array $record The record that holds this attribute's value.
     * @return boolean
     */
    function isEmpty($record)
    {
      return (!isset($record[$this->fieldName()])||(!is_array($record[$this->fieldName()]))||(is_array($record[$this->fieldName()]) && count($record[$this->fieldName()])===0));
    }
  }
?>
