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
     * @param $name        Name of the attribute
     * @param $beginTime   Time to start with (eg 8)
     * @param $endTime     Time to end with (eg 24)
     * @param $steps       Array containing possible minute values (eg array("00","15","30","45"))
	   * @param $default  Start Time (exp: 20:30)
     * @param $flags Flags for this attribute
     */
    function atkTimeAttribute($name, $beginTime=0, $endTime=23, $steps=array("00","15","30","45"), $default="", $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
      $this->m_beginTime = $beginTime;
      $this->m_endTime   = $endTime;
      $this->m_steps     = $steps;
      $this->m_default   = $default;
    }

    /**
     * Converts a date string (HHMISS) to an
     * array with 2 fields (hours, minutes, seconds).
     * @param $time the time string
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
     * @param $record
     * @return text string of $record
     */
    function display($record)
    {
	  	$value = $record[$this->fieldName()];
      if (empty($value["hours"]) ) return "";
      $tmp_time = $value["hours"].":".$value["minutes"];
      if ($value["seconds"]) $tmp_time.=":".$value["seconds"];
      return $tmp_time;
    }

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
      return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="", $fieldprefix="")
    {
      if(($this->m_default=="NOW" && $this->m_ownerInstance->m_action=="add") ||
         ($this->m_default=="" && $this->hasFlag(AF_OBLIGATORY)))
      {
	  $this->m_default=date("H:i");
      }
      $default = split(":",$this->m_default);

      $id = $fieldprefix.$this->fieldName();
      $field = $record[$this->fieldName()];

      // set vars for hour / minutes dropdowns
      $this->registerKeyListener($id.'[hours]', KB_CTRLCURSOR|KB_LEFTRIGHT);
      $this->registerKeyListener($id.'[minutes]', KB_CTRLCURSOR|KB_LEFTRIGHT);
      $m_hourBox = '<select id="'.$id.'[hours]" name="'.$id."[hours]\">\n";
      $m_minBox  = '<select id="'.$id.'[minutes]" name="'.$id."[minutes]\">\n";
      // set default values for both boxes
      // depends upon atkaction
      // if add/admin, use $default param, else use time in $record
      if (is_array($field))
      {
        $m_defHour = $field["hours"];
        $m_defMin  = $field["minutes"];
      }
      else
      {
        $m_defHour = $default[0];
        $m_defMin  = $default[1];
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
      for ($i=0;$i<=count($this->m_steps)-1;$i++)
      {
       if($i!=0) $prev = $this->m_steps[$i-1];
       else $prev= -1;
       if ($this->m_steps[$i] >= $m_defMin && $prev < $m_defMin&&($m_defMin!=""))
       {
        $sel = " selected";
       }
       else
       {
        $sel = "";
       }

       $m_minBox .= sprintf("<option value='%02d'%s>%02d</option>\n",$this->m_steps[$i],$sel,$this->m_steps[$i]);
      }

      // close dropdown structures
      $m_hourBox .= "</select>";
      $m_minBox  .= "</select>";
      $m_secBox  .= "<input type=\"hidden\" name=\"".$fieldprefix.$this->fieldName()."[seconds]\" value=\"00\">\n";

      // assemble display version
      return $m_hourBox.":".$m_minBox.$m_secBox;
    }


    function value2db($rec)
    {

     $hours = $rec[$this->fieldName()]["hours"];
     $minutes = $rec[$this->fieldName()]["minutes"];
     $seconds = $rec[$this->fieldName()]["seconds"];

     if ($hours=="" || $minutes=="") return "NULL";

     $result = sprintf("%02d",$hours).":".sprintf("%02d",$minutes).":".sprintf("%02d",$seconds);

     return $result;
    }

		/**
     * Convert database value to time array
     * @param $rec database record with date field
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
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
     return parent::search($record, $extended, $fieldprefix);
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

    function validate(&$rec, $mode)
    {
      $value = $rec[$this->fieldName()];
      if ($this->hasFlag(AF_OBLIGATORY) && ($value["hours"]==-1 || $value['minutes']==-1))
      {
        triggerError($rec, $this->fieldName(), 'error_obligatoryfield');
      }

    }

   /**
     * Add's fields to query
     * @param $query The SQL query
     * @param $tablename Tablename
     * @param $fieldaliasprefix Field alias prefix
     * @param $level ?
     * @param $mode Indicates what kind of query is being processing:
     *              This can be any action performed on a node (edit, add, etc)
     *              Mind you that "add" and "update" are the actions that store
     *              something in the database, whereas the rest are probably
     *              select queries.
     * @return New SQL Query
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode == "add" || $mode == "update")
      {
        $value = $this->value2db($rec);
        if($value == 'NULL')
          $query->addField($this->fieldName(),$value,"","",false);
        else
          $query->addField($this->fieldName(),$value,"","",!$this->hasFlag(AF_NO_QUOTES));
      }
      else
        $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
    }

    function dbFieldType()
    {
      return "time";
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
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
