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
    var $m_default = 0;
    
    /**
     * Constructor
     * @param $name        Name of the attribute
     * @param $beginTime   Time to start with (eg 8)
     * @param $endTime     Time to end with (eg 24)
     * @param $steps       Array containing possible minute values (eg array("00","15","30","45"))
     * @param $flags Flags for this attribute
     */
    function atkTimeAttribute($name, $beginTime, $endTime, $steps, $default="", $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
      $this->m_beginTime = $beginTime;
      $this->m_endTime   = $endTime;
      $this->m_steps     = $steps;
      $this->m_default   = $default;
    }


    /**
     * Display's text version of Record
     * @param $record
     * @return text string of $record
     */
    function display($record)
    {     
      return text($this->m_lookup[$record[$this->fieldName()]]);
    }


    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a checkbox
     */
    function edit($record="", $fieldprefix="")
    {
     global $atkaction;

      // set vars for hour / minutes dropdowns
       $m_hourBox = "<select name='".$this->fieldName()."_hours'>\n";
       $m_minBox  = "<select name='".$this->fieldName()."_mins'>\n";    

      // set default values for both boxes
      // depends upon atkaction
      // if edit, use $default param, else use time in $record
       if ($atkaction == "add")
       {
        $m_defHour = substr($this->m_default,0,2);
        $m_defMin  = substr($this->m_default,3,2);
       }
       else
       {
        $m_defHour = substr($record[$this->fieldName()],0,2);
        $m_defMin  = substr($record[$this->fieldName()],3,2);
       }

       //echo $m_defHour.":".$m_defMin;


      // generate hour dropdown
      for ($i=$this->m_beginTime;$i<=$this->m_endTime;$i++)
      {
       if ($i<10)
       {
        $h = "0".$i;
       }
       else
       {
        $h = $i;
       }

       if ($h == $m_defHour)
       {
        $sel = "selected";
       }
       else
       {
        $sel = "";
       }

       $m_hourBox .= "<option value='$h' $sel>$h</option>\n";
      }

      // generate minute dropdown
      for ($i=0;$i<=count($this->m_steps)-1;$i++)
      {
       if ($this->m_steps[$i] == $m_defMin)
       {
        $sel = "selected";
       }
       else
       {   
        $sel = "";
       }

       $m_minBox .= "<option value='".$this->m_steps[$i]."' $sel>".$this->m_steps[$i]."</option>\n";
      }

      // close dropdown structures
      $m_hourBox .= "</select>\n";
      $m_minBox  .= "</select>\n";

      // assemble display version
      return $m_hourBox."&nbsp;".$m_minBox;
    }


    function value2db($rec)
    {
     global $HTTP_POST_VARS;

     $is_time = $this->m_tableMeta['type'] == 'time';
     $a       = $this->fieldName()."_hours";    
     $b       = $this->fieldName()."_mins";

     if ($is_time)
     {
      $c = ":00";
     }
     else
     {
      $c = "";
     }
        
     return $HTTP_POST_VARS[$a].":".$HTTP_POST_VARS[$b].$c;
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
