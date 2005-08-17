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

  /** @internal Includes */
  include_once($config_atkroot."atk/attributes/class.atkdateattribute.inc");
  include_once($config_atkroot."atk/attributes/class.atktimeattribute.inc");

  /**
   * The atkDateTimeAttribute class can be used for date and time entry.
   * It corresponds to a DATETIME field in the database.   
   *
   * @author Sandy Pleyte <sandy@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkDateTimeAttribute extends atkAttribute
  {
    var $m_time = "";
    var $m_date = "";
    
    /**
     * Constructor
     * @param $name        Name of the attribute
     * @param $default_date   start date
     * @param $default_time     start time
     * @param $flags Flags for this attribute
     */
    function atkDateTimeAttribute($name, $default_date="",$default_time="",$flags=0)
    {
		  $default_steps = array();
			for($i=0;$i<60;$i++)
			{
			  $default_steps[$i] = $i;
			}
			if($default_date=="") { $default_date = ""; }
			if($default_time=="") { $default_time = date("H:i"); }
			
		  $this->m_time = new atkTimeattribute($name,0,23,$default_steps,$default_time);
			$this->m_date = new atkDateattribute($name,$default_date);
      $this->atkAttribute($name,$flags); // base class constructor
    }

    function init()
		{
		  
		  $this->m_time->m_owner=$this->m_owner;
		  $this->m_date->m_owner=$this->m_owner;
		  $this->m_time->m_ownerInstance=&$this->m_ownerInstance;
		  $this->m_date->m_ownerInstance=&$this->m_ownerInstance;
		}


    function fetchMeta($metadata)
    {
		  $this->m_date->fetchMeta($metadata);
		  $this->m_time->fetchMeta($metadata);
    }

    /**
     * Display's html version of Record
     * @param $record
     * @return text string of $record
     */
    function display($record, $mode="")
    { 
      $date = $this->m_date->display($record, $mode);
      $time = $this->m_time->display($record, $mode);
      if($date!='' && $time!='')
        return $date.($mode=="csv"?" ":"&nbsp;").$time;
      else
        return "";
    }

    function fetchValue($postvars)
    {
      $result=NULL;
      if(isset($postvars[$this->formName()]))
        $result = $postvars[$this->formName()];
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
      return $this->m_date->edit($record,$fieldprefix)."&nbsp;&nbsp;-&nbsp;&nbsp;".$this->m_time->edit($record,$fieldprefix);  
    }


    function value2db($rec)
    {
      //$datetime = split(" ",$rec[$this->fieldname()]);
			//$tmp_rec = $rec;
			//$tmp_rec[$this->fieldname()]=$datetime[0];
      $date=$this->m_date->value2db($rec);
			
			//$tmp_rec = $rec;
			//$tmp_rec[$this->fieldname()]=$datetime[1];
			$time=$this->m_time->value2db($rec);
      
      if($time!='NULL' && $date!='NULL')
        $result = $date." ".$time;
      else
        $result = 'NULL';
     return $result;
    }
		/**
     * Convert database value to datetime array
     * @param $rec database record with date field
     * @return array with 3 fields (hours:minutes:seconds)
     */
    function db2value($rec)
    {
		  if(isset($rec[$this->fieldName()]))
		  {
  			$datetime = split(" ",$rec[$this->fieldname()]);
  			
  			$tmp_rec = $rec;
  			$tmp_rec[$this->fieldname()]=$datetime[0];
        $result_date=$this->m_date->db2value($tmp_rec);
  			
  			$tmp_rec = $rec;
  			$tmp_rec[$this->fieldname()]=$datetime[1];
  			$result_time = $this->m_time->db2value($tmp_rec);
  			
  			return array_merge($result_date,$result_time);
		  }
    }
	
    /**
     * Add's fields to query
     * @param $query The SQL query
     * @param $tablename Tablename
     * @param $fieldaliasprefix Field alias prefix
     * @param $level ?
     * @param $mode select/admin/copy/edit
     * @return New SQL Query
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode == "add" || $mode == "update")
      {
        if($this->value2db($rec) == '' || $this->value2db($rec) == 'NULL')
        {          
          $query->addField($this->fieldName(),'NULL','','',false);
        }
        else
        {
          if (atkconfig('database') != 'oci9')
            $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
          else
          {
            $value = $this->value2db($rec);
            $query->addField($this->fieldName(),$value,"","",!$this->hasFlag(AF_NO_QUOTES), $mode, "DATETIME");      
          }
        }
      }
      else
      {
        if (atkconfig('database') != 'oci9')
          $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
        else
        {
          $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES), $mode, "DATETIME");      
        }
      }
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
    
    function hide($record="", $fieldprefix="")
    {
      // we only need to return the date part, because the dateattribute also
      // hides the other (time) elements that are present in the record (is that
      // a bug of the dateattribute?)
      return $this->m_date->hide($record, $fieldprefix);
    }
    
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      //return array("exact","greaterthan","greaterthanequal","lessthan","lessthanequal");
      return array();
    }
    
    function dbFieldType()
    {
      // TODO FIXME: Is this correct? Or does the datetimeattribute currently only support varchar fields?
      return "datetime";
    }
    
    
  }
?>
