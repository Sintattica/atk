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
   * @version $Revision: 6185 $
   * $Id$
   */

  /**
   * Flags for atkTimeAttribute
   */
  define("AF_TIME_SECONDS", AF_SPECIFIC_1);

  /**
   * The atkTimeAttribute class represents an attribute of a node
   * that has a selectbox to select from predefined time values.
   *
   * @author Wim Kosten <wim@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkTimeAttribute extends atkAttribute
  {
    var $m_beginTime = 0;
    var $m_endTime = 23;
    var $m_steps = array("0","30");
    var $m_default = "";

    /**
     * Constructor
     * @param string $name      Name of the attribute
     * @param int $beginTime    Time to start with (eg 8)
     * @param int $endTime      Time to end with (eg 24)
     * @param int|array $steps  containing possible minute or seconds values (eg array("00","15","30","45"))
     *                          or the interval (eg 5 for 00,05,10,15, etc.)
     *                          if the flag AF_TIME_SECONDS is set, this is for seconds, the minutes will be range(0, 59)
     *                          else this is for the minutes and the seconds will not be displayed
	   * @param string $default   Start Time (exp: 20:30)
     * @param int $flags        Flags for this attribute
     */
    function atkTimeAttribute($name, $beginTime=0, $endTime=23, $steps=array("00","15","30","45"), $default="", $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
      $this->m_beginTime = $beginTime;
      $this->m_endTime   = $endTime;
      if (is_array($steps))  $this->m_steps     = $steps;
      else                   $this->m_steps     = $this->intervalToSteps($steps);
      $this->m_default   = $default;
    }

    /**
     * Convert an interval (integer) to an array with steps.
     *
     * @param int $interval The interval to convert
     * @return array The array with steps.
     */
    function intervalToSteps($interval)
    {
      $steps=array();
      for ($i=0;$i<=59;$i++)
      {
        if ($i%$interval===0)
        {
          $steps[] = $i;
        }
      }
      return $steps;
    }

    /**
     * Converts a date string (HHMISS) to an
     * array with 2 fields (hours, minutes, seconds).
     * @param string $time the time string
     * @return array with 3 fields (hours, minutes, seconds)
     */
    function timeArray($time)
    {
      return array("hours" => substr($time, 0, 2),
                   "minutes" => substr($time, 2, 2),
                   "seconds" => substr($time, 4, 2));
    }


    /**
     * Display's text version of Record
     * @param array $record
     * @return text string of $record
     */
    function display($record)
    {
	  	$value = $record[$this->fieldName()];
      if ($value["hours"] === "") return "";
      $tmp_time = sprintf("%02d:%02d", $value["hours"], $value["minutes"]);
      if ($value["seconds"] && $this->hasFlag(AF_TIME_SECONDS)) $tmp_time.=sprintf(":%02d", $value["seconds"]);
      return $tmp_time;
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
      $result = $postvars[$this->fieldName()];

      if(!is_array($result))
      {
        $exploded = explode(":",$postvars[$this->fieldName()]);
        if (count($exploded)<=1) return "";
        $result= array();
        $result["hours"]  = $exploded[0];
        $result["minutes"]= $exploded[1];
        if ($exploded[2]) $result["seconds"]= $exploded[2];
      }
      else if (strlen($result['hours']) == 0 || strlen($result['minutes']) == 0)
      {
        return NULL;
      }
      else
      {
        $result = array(
          'hours' => $result['hours'],
          'minutes' => $result['minutes'],
          'seconds' => $result['seconds']
        );
      }

      return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record="", $fieldprefix="", $mode="")
    {
      if(($this->m_default=="NOW" && $this->m_ownerInstance->m_action=="add") ||
         ($this->m_default=="" && $this->hasFlag(AF_OBLIGATORY)))
      {
	     $this->m_default=date("H:i:s");
      }
      $default = split(":",$this->m_default);

      $id = $fieldprefix.$this->fieldName();
      $field = $record[$this->fieldName()];

      // set vars for hour / minutes dropdowns
      $this->registerKeyListener($id.'[hours]', KB_CTRLCURSOR|KB_LEFTRIGHT);
      $this->registerKeyListener($id.'[minutes]', KB_CTRLCURSOR|KB_LEFTRIGHT);
      $this->registerKeyListener($id.'[seconds]', KB_CTRLCURSOR|KB_LEFTRIGHT);
      $m_hourBox = '<select id="'.$id.'[hours]" name="'.$id."[hours]\" class=\"atktimeattribute\">\n";
      $m_minBox  = '<select id="'.$id.'[minutes]" name="'.$id."[minutes]\" class=\"atktimeattribute\">\n";
      $m_secBox  = '<select id="'.$id.'[seconds]" name="'.$id."[seconds]\" class=\"atktimeattribute\">\n";
      // set default values for both boxes
      // depends upon atkaction
      // if add/admin, use $default param, else use time in $record
      if (is_array($field))
      {
        $m_defHour = $field["hours"];
        $m_defMin  = $field["minutes"];
        $m_defSec  = $field["seconds"];
      }
      else
      {
        $m_defHour = $default[0];
        $m_defMin  = $default[1];
        $m_defSec  = $default[2];
      }

      atkdebug("defhour=$m_defHour   defmin=$m_defMin");
      // generate hour dropdown
      if (!$this->hasflag(AF_OBLIGATORY))
        $m_hourBox .= '<option value=""'.($m_defHour === "" ? ' selected' : '').'></option>';
      for ($i=$this->m_beginTime;$i<=$this->m_endTime;$i++)
      {
        if ($m_defHour!=="" && ($i == $m_defHour))
        {
          $sel = " selected";
        }
        else
        {
          $sel = "";
        }
        $m_hourBox .= sprintf("<option value='%02d'%s>%02d</option>\n",$i,$sel,$i);
      }

      // generate minute dropdown
      if (!$this->hasflag(AF_OBLIGATORY))
        $m_minBox .= '<option value=""'.($m_defMin === "" ? ' selected' : '').'></option>';

      if ($this->hasFlag(AF_TIME_SECONDS))
      {
        $minute_steps = range(00, 59);
      }
      else
      {
        $minute_steps = $this->m_steps;
      }

      for ($i=0;$i<=count($minute_steps)-1;$i++)
      {
       if($i!=0) $prev = $minute_steps[$i-1];
       else $prev= -1;
       if ($minute_steps[$i] >= $m_defMin && $prev < $m_defMin&&($m_defMin!=""))
       {
        $sel = " selected";
       }
       else
       {
        $sel = "";
       }

       $m_minBox .= sprintf("<option value='%02d'%s>%02d</option>\n",$minute_steps[$i],$sel,$minute_steps[$i]);
      }

      // generate second dropdown
      if (!$this->hasFlag(AF_OBLIGATORY))
        $m_secBox .= '<option value""'.($m_defSec === "" ? ' selected' : '').'></option>';
      for ($i=0;$i<=count($this->m_steps)-1;$i++)
      {
        if($i!=0) $prev= $this->m_steps[$i-1];
        else $prev= -1;
        if ($this->m_steps[$i] >= $m_defSec && $prev < $m_defSec&&($m_defSec!=""))
        {
          $sel = " selected";
        }
        else
        {
          $sel = "";
        }

        $m_secBox .= sprintf("<option value='%02d' %s>%02d</option>\n",$this->m_steps[$i],$sel,$this->m_steps[$i]);
      }



      // close dropdown structures
      $m_hourBox .= "</select>";
      $m_minBox  .= "</select>";
      if($this->hasFlag(AF_TIME_SECONDS))
      {
        $m_secBox  .= "</select>";
        $m_secBox = ":" . $m_secBox;
      }
      else
      {
        $m_secBox = "<input type=\"hidden\" name=\"".$fieldprefix.$this->fieldName()."[seconds]\" value=\"00\">\n";
      }

      // assemble display version
      return $m_hourBox.":".$m_minBox.$m_secBox;
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
      $hours = $rec[$this->fieldName()]["hours"];
      $minutes = $rec[$this->fieldName()]["minutes"];
      $seconds = $rec[$this->fieldName()]["seconds"];
      
      if ($hours=="" || $minutes=="" || ($this->hasFlag(AF_TIME_SECONDS) && $seconds=="")) return NULL;

      $result = sprintf("%02d",$hours).":".sprintf("%02d",$minutes).":".sprintf("%02d",$seconds);
      
      return $result;
    }

		/**
     * Convert database value to time array
     * @param array $rec database record with date field
     * @return array with 3 fields (hours:minutes:seconds)
     */
    function db2value($rec)
    {
      if (strlen($rec[$this->fieldName()]) == 0 )
      {
        $retval=NULL;
      }
      else
      {
         $retval = array("hours"=>substr($rec[$this->fieldName()], 0, 2),
                   "minutes"=>substr($rec[$this->fieldName()], 3, 2),
                   "seconds"=>substr($rec[$this->fieldName()], 6, 2));
      }
      return $retval;
    }

    /**
     * Returns a piece of html code that can be used in a form to search values
     * @param array $record Array with fields
     * @param boolean $extended if set to false, a simple search input is
     *                          returned for use in the searchbar of the
     *                          recordlist. If set to true, a more extended
     *                          search may be returned for the 'extended'
     *                          search page. The atkAttribute does not
     *                          make a difference for $extended is true, but
     *                          derived attributes may reimplement this.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     * @return piece of html code with a checkbox
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
     return parent::search($record, $extended, $fieldprefix);
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

    /**
     * Checks if a value is valid.
     *
     * @param array $rec    The record that holds the value for this
     *                      attribute. If an error occurs, the error will
     *                      be stored in the 'atkerror' field of the record.
     * @param String $mode The mode for which should be validated ("add" or
     *                     "update")
     */
    function validate(&$rec, $mode)
    {
      $value = $rec[$this->fieldName()];
      if ($this->hasFlag(AF_OBLIGATORY) && ($value["hours"]==-1 || $value['minutes']==-1))
      {
        triggerError($rec, $this->fieldName(), 'error_obligatoryfield');
      }

    }

    /**
     * Adds this attribute to database queries.
     *
     * Database queries (select, insert and update) are passed to this method
     * so the attribute can 'hook' itself into the query.
     * 
     * @param atkQuery $query The SQL query object
     * @param String $tablename The name of the table of this attribute
     * @param String $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
     * @param Array $rec The record that contains the value of this attribute.
     * @param int $level Recursion level if relations point to eachother, an
     *                   endless loop could occur if they keep loading
     *                   eachothers data. The $level is used to detect this
     *                   loop. If overriden in a derived class, any subcall to
     *                   an addToQuery method should pass the $level+1.
     * @param String $mode Indicates what kind of query is being processing:
     *                     This can be any action performed on a node (edit,
     *                     add, etc) Mind you that "add" and "update" are the
     *                     actions that store something in the database,
     *                     whereas the rest are probably select queries.
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode == "add" || $mode == "update")
      {
        $value = $this->value2db($rec);
        if($value == NULL)
          $query->addField($this->fieldName(),'NULL',"","",false);
        else
          $query->addField($this->fieldName(),$value,"","",!$this->hasFlag(AF_NO_QUOTES));
      }
      else
        $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
    }

    /**
     * Return the database field type of the attribute.
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.
     */
    function dbFieldType()
    {
      return "time";
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param array $record Array with values
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return Piece of htmlcode
     */
    function hide($record="", $fieldprefix)
    {
      $field = $record[$this->fieldName()];

      if (is_array($field))
        foreach ($field as $key => $value)
          $result .= '<input type="hidden" name="'.$fieldprefix.$this->formName().'['.$key.']" '.'value="'.$value.'">';

      else
        $result = '<input type="hidden" name="'.$fieldprefix.$this->formName().'" value="'.$field.'">';

      return $result;
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
      // When we get $value as a substring, we autocomplete the time
      // So 9 becomes 09:00:00 and 11:15 becomes 11:15:00
      if (!is_array($value))
      {
        $retval = array("hours"=>substr($value, 0, 2),
                   "minutes"=>substr($value, 3, 2),
                   "seconds"=>substr($value, 6, 2));

        if (!$retval["seconds"]) $retval["seconds"]="00";
        if (!$retval["minutes"]) $retval["minutes"]="00";

        if (strlen($retval["hours"])==1)   $retval["hours"] =   "0".$retval["hours"];
        if (strlen($retval["minutes"])==1) $retval["minutes"] = "0".$retval["minutes"];
        if (strlen($retval["seconds"])==1) $retval["seconds"] = "0".$retval["seconds"];

        $value = implode(":",$retval);
      }
      return parent::getSearchCondition($query, $table, $value, $searchmode);
    }

    /**
     * Parse a timestring to an array
     *
     * @param string $stringvalue The time to parse
     * @return array array with hours, minutes and seconds
     */
    function parseTime($stringvalue)
    {
  		//Assuming hh:mm:ss
		  $retval = array("hours"=>substr($stringvalue, 0, 2),
		     "minutes"=>substr($stringvalue, 3, 2),
		     "seconds"=>substr($stringvalue, 6, 2));

		  if (!$retval["seconds"]) $retval["seconds"]="00";
		  if (!$retval["minutes"]) $retval["minutes"]="00";

		  if (strlen($retval["hours"])==1)   $retval["hours"] =   "0".$retval["hours"];
		  if (strlen($retval["minutes"])==1) $retval["minutes"] = "0".$retval["minutes"];
		  if (strlen($retval["seconds"])==1) $retval["seconds"] = "0".$retval["seconds"];
		  return $retval;
	  }
  }
?>
