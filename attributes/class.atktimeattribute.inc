<?php
  /**
   * The atkTimeAttribute class represents an attribute of a node
   * that has a selectbox to select from predefined time values.
   *
   * @author Wim Kosten (wim@achievo.org)
   * @version $Revision$
   *
   * $Id$
   *
   */
   
  /* flag(s) specific for atkTimeAttribute */      
  define("AF_TIME_EMPTYFIELD", AF_SPECIFIC_1); // Fields have one empty option

  class atkTimeAttribute extends atkAttribute
  {
    var $m_beginTime = 0;
    var $m_endTime = 24;
    var $m_steps = array("0","30");
    var $m_default = array();

    /**
     * Constructor
     * @param $name        Name of the attribute
     * @param $beginTime   Time to start with (eg 8)
     * @param $endTime     Time to end with (eg 24)
     * @param $steps       Array containing possible minute values (eg array("00","15","30","45"))
	 * @param $default  Start Time (exp: 20:30)
     * @param $flags Flags for this attribute
     */
    function atkTimeAttribute($name, $beginTime, $endTime, $steps, $default="", $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
      $this->m_beginTime = $beginTime;
      $this->m_endTime   = $endTime;
      $this->m_steps     = $steps;
			if($default=="") $default=date("H:i");
      $this->m_default   = split(":",$default);
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
      $tmp_time = $value["hours"].":".$value["minutes"].":".$value["seconds"];			
      return $tmp_time;
    }

    function fetchValue($postvars)
    {
      $result = $postvars[$this->fieldName()];
      if(is_array($result))
        return $result;
      else
        return "";
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="", $fieldprefix="")
    {    
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
      if (!is_array($field))
      {
        $m_defHour = $this->m_default[0];
        $m_defMin  = $this->m_default[1];
      }
      else
      {
        $m_defHour = $field["hours"];
        $m_defMin  = $field["minutes"];
      }

      // generate hour dropdown     
      if ($this->hasflag(AF_TIME_EMPTYFIELD))
        $m_hourBox .= '<option value="-1"'.($m_defHour == -1 ? ' selected' : '').'></option>';
      for ($i=$this->m_beginTime;$i<=$this->m_endTime;$i++)
      {
        if ($i == $m_defHour)
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
      if ($this->hasflag(AF_TIME_EMPTYFIELD))
        $m_minBox .= '<option value="-1"'.($m_defMin == -1 ? ' selected' : '').'></option>';
      for ($i=0;$i<=count($this->m_steps)-1;$i++)
      {
       if($i!=0) $prev = $this->m_steps[$i-1];
       else $prev= -1;
       if ($this->m_steps[$i] >= $m_defMin && $prev < $m_defMin)
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

     if (empty($hours) || empty($minutes) || empty($seconds)) return "";
     if ($hours==-1 || $minutes==-1) return "NULL";

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
    function search($record="")
    {
     return "";
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
  }
?>