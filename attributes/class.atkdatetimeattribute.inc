<?

  /**
   * The atkDateTimeAttribute class can be used for date and time entry.
   * It corresponds to a DATETIME field in the database.   
   *
   * @author Sandy Pleyte <sandy@achievo.com>
   * @version $Revision$
   *
   * $Id$
   *
   */
  class atkDateTimeAttribute extends atkAttribute
  {
    
    
    /**
     * Constructor
     * @param $name        Name of the attribute
     * @param $default_date   start date
     * @param $default_time     start time
     * @param $flags Flags for this attribute
     */
    function atkDateTimeAttribute($name, $default_date="",$default_time="",$flags=0)
    {
		  $m_default_steps = array();
			for($i=0;$i<60;$i++)
			{
			  $m_default_steps[$i] = $i;
			}
			if($default_date=="") { $default_date = ""; }
			if($default_time=="") { $default_time = date("H:i"); }
			
		  $this->m_time = new atkTimeattribute($name,0,23,$m_default_steps,$default_time);
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
     * Display's text version of Record
     * @param $record
     * @return text string of $record
     */
    function display($record)
    { 
      $result=$this->m_date->display($record);
			$result.="&nbsp;";
			$result.=$this->m_time->display($record);
      return $result;
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
      return $this->m_date->edit($record,$fieldprefix)."&nbsp;&nbsp;-&nbsp;&nbsp;".$this->m_time->edit($record,$fieldprefix);  
    }


    function value2db($rec)
    {
		  //var_dump($rec);
      //$datetime = split(" ",$rec[$this->fieldname()]);
			//$tmp_rec = $rec;
			//$tmp_rec[$this->fieldname()]=$datetime[0];
      $result=$this->m_date->value2db($rec);
			$result.=" ";
			//$tmp_rec = $rec;
			//$tmp_rec[$this->fieldname()]=$datetime[1];
			$result.=$this->m_time->value2db($rec);

     return $result;
    }
		/**
     * Convert database value to datetime array
     * @param $rec database record with date field
     * @return array with 3 fields (hours:minutes:seconds)
     */
    function db2value($rec)
    {
		   
			$datetime = split(" ",$rec[$this->fieldname()]);
			$tmp_rec = $rec;
			$tmp_rec[$this->fieldname()]=$datetime[0];
      $result_date=$this->m_date->db2value($tmp_rec);
			$tmp_rec = $rec;
			$tmp_rec[$this->fieldname()]=$datetime[1];
			$result_time = $this->m_time->db2value($tmp_rec);
			//var_dump(array_merge($result_date,$result_time));
			return array_merge($result_date,$result_time);
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
      return array("exact","greaterthan","greaterthanequal","lessthan","lessthanequal");
    }
    
    
  }
?>
