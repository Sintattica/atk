<?

  /**
   * The atkTimeAttribute class represents an attribute of a node
   * that has a selectbox to select from predefined time values.
   * @author Wim Kosten (wimachievo.com)
   */
  class atkTimeAttribute extends atkAttribute
  {
    var $m_beginTime = 0;
    var $m_endTime = 23;
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
			atkdebug("TIJD: ".$value);
      $tmp_time = $value["hours"].":".$value["minutes"].":".$value["seconds"];
			if (!empty($tmp_time)) return $tmp_time;
  		else return "";
      
    }

    function fetchValue($postvars)
    {
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
      global $ATK_VARS;
			//echo "action: ".$ATK_VARS["atkaction"];
			//echo "<br>Action: ".$this->m_ownerInstance->m_postvars["atkaction"];
			//var_dump($this->m_ownerInstance->m_postvars);
      $field = $record[$this->fieldName()];
			
      // set vars for hour / minutes dropdowns
      $m_hourBox = "<select name=\"".$fieldprefix.$this->fieldName()."[hours]\">\n";
      $m_minBox  = "<select name=\"".$fieldprefix.$this->fieldName()."[minutes]\">\n";    

      // set default values for both boxes
      // depends upon atkaction
      // if add/admin, use $default param, else use time in $record
      if ($ATK_VARS["atkaction"] == "add" || $ATK_VARS["atkaction"]== "admin")
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
      for ($i=0;$i<=count($this->m_steps)-1;$i++)
      {
			 
       if ($this->m_steps[$i] == $m_defMin)
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
      $m_hourBox .= "</select>\n";
      $m_minBox  .= "</select>\n";
		  $m_secBox  .= "<input type=\"hidden\" name=\"".$fieldprefix.$this->fieldName()."[seconds]\" value=\"00\">\n";
      // assemble display version
      return $m_hourBox."&nbsp;".$m_minBox."&nbsp;".$m_secBox;
    }


    function value2db($rec)
    {

     $is_time = $this->m_tableMeta['type'] == 'time' || $this->m_tableMeta['type'] == 'datetime';
		 
		 $hours = $rec[$this->fieldName()]["hours"];
     $minutes = $rec[$this->fieldName()]["minutes"];
     $seconds = $rec[$this->fieldName()]["seconds"];

     if (empty($hours) || empty($minutes) || empty($seconds)) return "";
     $separator = ":";
		 
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
		  
      $is_time = $this->m_tableMeta['type'] == 'time' || $this->m_tableMeta['type'] == 'datetime';
      if (strlen($rec[$this->fieldName()]) == 0 ) return "";
      return array("hours"=>substr($rec[$this->fieldName()], 0, 2),
                   "minutes"=>substr($rec[$this->fieldName()], 3, 2),
                   "seconds"=>substr($rec[$this->fieldName()], 6, 2));
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
    
  }
?>
