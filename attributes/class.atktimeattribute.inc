<?php
  /**
   * The atkTimeAttribute is an attribute for entering a time value.
   * @author  Ivo Jansch <ivo@achievo.com>
   * @version 0.1
   *
   * $Id$
   * $Log$
   * Revision 1.1  2001/04/23 10:17:10  ivo
   * Initial revision
   *
   * Revision 1.4  2001/03/14 15:21:54  ivo
   * New feature: AF_TOTAL flag for attributes. This results in a total row
   * at the bottom of the record list.
   *
   * Revision 1.3  2001/03/14 10:22:28  ivo
   * Attributes now have a hide() function to do their own input=hidden implementation.
   *
   * Revision 1.2  2001/02/22 22:46:18  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:57:58  sandy
   * Achievo Tool Kit
   *
   */
  class atkTimeAttribute extends atkAttribute
  {    
    /** member vars **/
    var $m_resolution_min;
    var $m_maxtime_min;
    
  
    /**
     * Default Constructor, sets up atkAttribute 
     *
     * @param $name
     * @param $resolution
     * @param $maxtime
     * @param $flags
     * @see atkAttribute
     */
    function atkTimeAttribute($name, $resolution="1m", $maxtime="10h", $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
      $hms = substr($resolution,-1);
      $resolution = substr($resolution,0,-1);

      if (strtoupper($hms)=="H") $factor = 60;
      else $factor=1;      
     
      $this->m_resolution_min = $resolution*$factor;
      
      $hms = substr($maxtime,-1);
      $maxtime = substr($maxtime,0,-1);
      if (strtoupper($hms)=="H") $factor = 60;
      else $factor=1;      
      
      $this->m_maxtime_min = $maxtime*$factor;
    }

    /**   
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value. (hours, minutes and seconds will be a dropdownbox)
     * @params $record 
     * @return Piece a of HTML Code
     */
    function edit($record="")
    {  
      if ($this->m_maxtime_min>=60)
      {
        $result.= '<select name="'.$this->formName().'[hours]">';
        for ($h=0;$h<=$this->m_maxtime_min/60;)
        { 
          $result .= '<option value="'.$h.'" ';         
          if ($record[$this->fieldName()]['hours']==$h) $result.= "selected";
          $result .='>'.$h.' '.text('hours');;
          if ($this->m_resolution_min<=60)
          {
            $h++;
          }
          else
          {
            $h = floor($h+$this->m_resolution_min/60);
          }
        }
      }
      $result.='</select>';
      
      if ($this->m_maxtime_min>=1&&$this->m_resolution_min<60)
      {
        $result.= '<select name="'.$this->formName().'[minutes]">';
        for ($m=0;$m<60||($this->m_maxtime_min<60 && $m<$this->m_maxtime_min);)
        { 
          $result .= '<option value="'.$m.'" ';
          if ($record[$this->fieldName()]['minutes']==$m) $result.= "selected";
          $result.='>'.$m.' '.text('minutes');
          if ($this->m_resolution_min<=1)
          {
            $m++;
          }
          else
          {
            $m = $m+$this->m_resolution_min;
          }
        }
      }
      $result.='</select>';   
      
      return $result;
    }
    
    function hide($record="")
    {
      $result = '<input type="hidden" name="'.$this->formName().'[hours]" value="'.$record[$this->fieldName()]["hours"].'">';
      $result.= '<input type="hidden" name="'.$this->formName().'[minutes]" value="'.$record[$this->fieldName()]["minutes"].'">';
      return $result;
    }
    
    /**   
     * Returns a piece of html code that can be used in a form to search values.    
     * @params $record Array of records, with for each record 2 fields (hours, minutes)
     * @return Piece a of HTML Code
     */
    function search($record="")
    {
      return "&nbsp;"; // currently not searchable.
    }
    
    

    /**
     * Convert Time Array to minutes
     * @param $rec Array with 2 fields (hours, minutes)
     * @return integer indicating the number of minutes
     */
    function value2db($rec)
    {
        return $rec[$this->fieldName()]["hours"]*60+$rec[$this->fieldName()]["minutes"];
    }

    /**
     * Convert time to Array
     * @param $rec contains the time in minute format
     * @return Array with 2 fields (hours, minutes)
     */
    function db2value($rec)
    {
      $hours = floor($rec[$this->fieldName()]/60);
      return Array("hours"=>$hours,
                   "minutes"=>$rec[$this->fieldName()]-($hours*60)
                   );
    }

    /**
     * Function display's the time
     * @param $record Array with time
     * @return String with YYYY-MM-DD
     */
    function display($record)
    {
      $value=$record[$this->fieldName()];

      return sprintf("%02d",$value["hours"]).":".sprintf("%02d",$value["minutes"]);
    }
    
    /**
     * Check if the attribute is empty
     * @return true if it's empty
     */
    function isEmpty($postvars)
    {
      return ($postvars[$this->fieldName()]["hours"]==0&&$postvars[$this->fieldName()]["minutes"]==0);
    }
    
    function sum($rec1, $rec2)
    {                  
      if (!is_array($rec1))
      {   
        $a=Array("hours"=>0,"minutes"=>0);       
      }
      else
      {        
        $a = $rec1[$this->fieldName()];
      }      
      
      $b = $rec2[$this->fieldName()];              
      
      $total = ($a["hours"]*60)+$a["minutes"]+($b["hours"]*60)+$b["minutes"];      
         
      return Array($this->fieldName()=>Array("hours"=>(floor($total/60)),"minutes"=>$total%60));
    }

  }
?>