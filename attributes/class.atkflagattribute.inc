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

  /** @internal include base class. */
  useattrib("atkmultiselectattribute");

  /**
   * The atkFlagAttribute class offers an way to edit bitmask flags.
   * @author  M. Roest <martin@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkFlagAttribute extends atkMultiSelectAttribute
  {
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
    function atkFlagAttribute($name, $optionArray, $valueArray="", $cols="", $flags=0, $size="")
    {
      $this->atkMultiSelectAttribute($name, $optionArray, $valueArray, $cols, $flags, $size);
      $this->m_dbfieldtype = "number";
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
      if(is_array($rec[$this->fieldName()]) && count($rec[$this->fieldName()]>=1))
      {
        $flags=0;
        foreach($rec[$this->fieldName()] as $flag)
        {
          $flags |= $flag;
        }
        return $flags;
      }
      else return 0;
    }

    /**
     * Converts a database value to an internal value.
     *
     * @param array $rec The database record that holds this attribute's value
     * @return mixed The internal value
     */
    function db2value($rec)
    {
      if($rec[$this->fieldName()]>0)
      {
        $newrec = array();
        foreach($this->m_values as $value)
        {
          if(hasFlag($rec[$this->fieldName()],$value))
          {
            $newrec[] = $value;
          }
        }
        return $newrec;
      }
      return array();
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.
     */
    function dbFieldType()
    {
      return $this->m_dbfieldtype;
    }

    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * @param atkQuery $query     The query object where the search condition should be placed on
     * @param String $table       The name of the table in which this attribute
     *                              is stored
     * @param mixed $value        The value the user has entered in the searchbox
     * @param String $searchmode  The searchmode to use. This can be any one
     *                              of the supported modes, as returned by this
     *                              attribute's getSearchModes() method.
     * @return String The searchcondition to use.
     */
    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      if (is_array($value) && count($value)>0 && $value[0]!="") // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.
      {
        $field = $table.".".$this->fieldName();
        if (count($value)==1) // exactly one value
        {
          $query->addSearchCondition($field." & ".$value[0]);
        }
        else
        {
          $mask = "(".implode("|",$value).")";
          $searchcondition = $field."&".$mask."=".$mask;
        }
      }
      return $searchcondition;
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * For the regular atkAttribute, this means getting the field with the
     * same name as the attribute from the html posting.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     * @return String The internal value
     */
    function fetchValue($postvars)
    {
      $vars = atkArrayNvl($postvars, $this->fieldName());
      if(!is_array($vars))
      {
        $result = array();
        foreach($this->m_values as $value)
        {
          if(hasFlag($vars, $value))
          {
            $result[] = $value;
          }
        }
        return $result;
      }
      else return $vars;
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
      $values = 0;
      $name = $this->fieldName();
      if(is_array($record[$name]))
      {
        foreach ($record[$name] as $var) $values |=$var;
      }
      else $values = $record[$name];
      return '<input type="hidden" name="'.$fieldprefix.$name.'"
                      value="'.$values.'">';
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
      return array("exact");
    }
}
?>