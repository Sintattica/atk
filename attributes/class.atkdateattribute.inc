<?php
  /**
   * The atkDateAttribute class represents an attribute of a node that has a date dingske.
   * @author  Tom Schenkenberg <Tom@achievo.com>
   *
   * @version 1.3
   *
   * <b>Changes</b>:
   * 1.3    - Pre-build of date attribute with valid dates (added 3 new methods for this) (Peter)
   *        - Disabled search until we have the new fetch methods available (Peter)
   *        - New default date format for add/edit time (Peter)
   * 1.2    - Attribute checks on the fly if a valid date is selected (Peter)
   *        - Date can be formatted accordingly to a specified format string at add/edit time (Peter)
   *        - Date can be formatted accordingly to a specified format string at view time (Peter)
   *        - A minimum date can be specified (Peter)
   *        - A maximum date can be specified (Peter)
   *        - Month names can be specified in the language file (long and short one!) (Peter)
   *        - Attribute now registers an external javascript file for use with the add/edit boxes
   *        - Date attribute auto-selects current date
   * 1.1    - Attribute now also supports mysql DATE type (Peter Verhage <peter@ibuildings.nl>)
   *        - Fix for month names enteref without a leading '0' (Ivo Jansch <ivo@ibuildings.nl>)
   *
   * $Id$
   * $Log$
   * Revision 4.15  2001/09/26 12:05:52  maurice
   * Code optimalisations for error handling
   *
   * Revision 4.14  2001/09/26 07:05:13  maurice
   * New error handling. Errors are saved in $record variable
   *
   * Revision 4.13  2001/09/21 11:57:13  sandy
   * updated the time attribute, it now also uses an array for hours, minutes, seconds.
   * Also created a new datetime attrib for the datetime type.
   *
   * Revision 4.12  2001/08/27 07:38:58  ivo
   * changed validate() api: atk now passes record to the function instead of
   * just one value. Also added a mode parameter that indicates whether we
   * are in add or update mode.
   *
   * Revision 4.11  2001/07/15 17:03:59  ivo
   * New feature: Alphabetical index on top of recordlists.
   * (use e.g. $this->setIndex("name"); in your class constructor to activate)
   * Fixed some bugs in extended search.
   *
   * Revision 4.10  2001/07/15 16:37:19  ivo
   * New atk.inc includefile in skel.
   * New feature: extended search.
   * Fixed a bug in session management; style.php and other files that get
   * loaded between two dispatches could corrupt a session.
   *
   * Revision 4.9  2001/06/29 14:48:37  peter
   * Fixed small bug; if you want to know the days of a certain month, you have
   * to use the "0" day of the next month (not the month you want to know it off...)
   *
   * Revision 4.8  2001/05/22 08:35:25  peter
   * Made the API a bit more user friendly by allowing the user to specify
   * "" for the date formats. This means the attribute will automagically
   * change the formats to their default. The default now can also be specified
   * in the language file(s) ($txt_date_format_edit / ..._view)!
   *
   * Revision 4.7  2001/05/11 07:15:56  peter
   * Forgot the new fieldprefix in previous commits.
   *
   * Revision 4.6  2001/05/10 21:39:56  peter
   * Added some new last minute improvements for validation.
   *
   * Revision 4.5  2001/05/10 21:13:50  peter
   * Total rewrite of date attribute, fixed some previous unknown bugs,
   * added server-side validation, made attribute more "stable".
   *
   * Revision 4.4  2001/05/10 08:31:00  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.3  2001/04/25 08:46:08  ivo
   * Big optimisations: using references for attribute manipulation.
   *
   * Revision 4.2  2001/04/25 07:29:40  sandy
   * fixed a bug in the data javascript for multilanguage
   * added the stickyvars to the onetomany relations urls
   *
   * Revision 4.1  2001/04/23 14:20:12  peter
   * Day now includes day-of-the-week name
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:08  ivo
   * Added atk4 to repository.
   *
   * Revision 1.12  2001/04/09 12:26:00  ivo
   * Fixes to dateattribute.
   *
   * Revision 1.11  2001/04/09 11:58:15  ivo
   * Fixes to dateattribute (day went wrong in edit mode)
   *
   * Revision 1.10  2001/04/09 11:03:53  ivo
   * Fixes in dateattribute: better formatting of values.
   *
   * Revision 1.9  2001/04/05 09:40:36  ivo
   * Previous version was committed by accident with a hack that should not
   * go into cvs.
   *
   * Revision 1.8  2001/04/05 09:33:40  ivo
   * *** empty log message ***
   *
   * Revision 1.7  2001/03/14 14:28:26  peter
   * fixed hide implementation bug
   *
   * Revision 1.6  2001/03/14 12:23:23  peter
   * added hide method, to reflect Ivo's changes
   *
   * Revision 1.5  2001/02/22 22:46:17  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.4  2001/02/16 15:54:10  peter
   * fixed some bugs with AF_HIDE_ADD flag
   *
   * Revision 1.3  2001/01/10 23:50:27  peter
   * - Pre-build of date attribute with valid dates (added 3 new methods for this)
   * - Disabled search until we have the new fetch methods available
   * - New default date format for add/edit and list time
   *
   * Revision 1.2  2001/01/10 23:24:44  ivo
   * Fixed wrong filename for javascript files (must be .php3 instead of .php).
   *
   * Revision 1.1.1.1  2001/01/10 13:57:57  sandy
   * Achievo Tool Kit
   *
   */
  class atkDateAttribute extends atkAttribute
  {
    var $m_date_min;
    var $m_date_max;
    var $m_date_format_edit;
    var $m_date_format_view;

    /**
     * Format date according to a format string
     * @param $date date array (gotten with getdate())
     * @param $format format string, compatible with PHP's date format functions
     * @param $weekday include day-of-week or not
     * @return string with formatted date
     */
    function formatDate($date, $format, $weekday=1)
    {
      /* format month */
      $format = str_replace("M", "%-%",   $format);
      $format = str_replace("F", "%=%",   $format);
      
      /* format day */
      if ($weekday)
      {
        $format = str_replace("d", "%*% d", $format);
        $format = str_replace("j", "%*% j", $format);
      }
      
      /* get date string */
      $str_date = date($format, $date[0]);
      
      /* replace month/week name */
      $str_date = str_replace("%-%", text(substr(strtolower($date["month"]), 0, 3)), $str_date);
      $str_date = str_replace("%=%", text(strtolower($date["month"])), $str_date);
      if ($weekday) $str_date = str_replace("%*%", text(strtolower($date["weekday"])), $str_date);

      /* return string */
      return $str_date;
    }

    /**
     * Returns the days in a certain month in a certain year
     * @param $date date array (gotten with getdate())
     * @return integer with number of days
     */
    function getDays($date)
    {
      /* the last day of any given month can be expressed as the "0" day of the next month! */
      $date = getdate(mktime (0, 0, 0, $date["mon"]+1, 0, $date["year"]));
      return $date["mday"];
    }

    /**
     * Converts a date string (YYYMMDD) to an
     * array with 3 fields (day, month, year).
     * @param date the date string
     * @return array with 3 fields (day, month, year)
     */
    function dateArray($date)
    {
      return array("day" => substr($date, 6, 2),
                   "month" => substr($date, 4, 2),
                   "year" => substr($date, 0, 4));
    }

    /**
     * Default Constructor, sets up the atkDateAttribute
     * The API of this method has changed, but is has been made
     * backwards compatible with existing modules!
     *
     * @param $name the attribute's name
     * @param $format_edit the format the edit/add box(es) will look like
     * @param $format_view the format in which dates are listed
     * @param $min the minimum date that has to be selected (0 is unlimited)
     * @param $max the maximum date that may be selected (0 is unlimited)
     * @param $flags the attribute's flags
     *
     * @see atkAttribute
     */
    function atkDateAttribute($name, $format_edit="", $format_view="", $min=0, $max=0, $flags=0)
    {
      global $txt_date_format_edit, $txt_date_format_view;
      
atkdebug("min ".$min." max ".$max);
      // *** API SUPPORT HACK ***
      // Because of backwards compatability and because of the number
      // of arguments this method has we also support the old API: ($name, $flags=0).
      if (is_int($format_edit))
      {
        $flags = $format_edit;
        $format_edit = "";
        $format_view = "";
        $min = 0;
        $max = 0;
      }

      /* edit date format */
      if     (!empty($format_edit)) $this->m_date_format_edit = $format_edit;
      else if (!empty($txt_date_format_edit)) $this->m_date_format_edit = $txt_date_format_edit;
      else   $this->m_date_format_edit = "F j Y";

      /* display date format */
      if     (!empty($format_view)) $this->m_date_format_view = $format_view;
      else if (!empty($txt_date_format_view)) $this->m_date_format_view = $txt_date_format_view;
      else   $this->m_date_format_view = "F j Y";

atkdebug("min ".$this->m_date_min." max ".$this->m_date_max." flags ".$flags);
      /* max / min date (only valid when obligatory) */
	if (($flags & AF_OBLIGATORY) == AF_OBLIGATORY)
			{
atkdebug("inIf");
        $this->m_date_min = $min;
        $this->m_date_max = $max;
			}
			else
			{
atkdebug("inElse");
        $this->m_date_min = 0;
        $this->m_date_max = 0;
			}

atkdebug("min ".$this->m_date_min." max ".$this->m_date_max);
      /* register javascript */
      $GLOBALS['g_layout']->register_script("atk/javascript/class.atkdateattribute.js.php");

      /* base class constructor */
      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH); // FIXME.. until the searchCondition() and
                                                         // search() methods are fixed, 
                                                         // you can't search on dates..
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value. (Month will be a dropdownbox, year and day text fields)
     * @params $record Array with 3 fields (year, month, day)
     * @return Piece a of HTML Code
     */
    function edit($record="", $fieldprefix="")
    {
      /* this field */
      $field = $record[$this->fieldName()];    
      $str_format = $this->m_date_format_edit;
      
      /* currently selected date */
      $current = 0;
      if (is_array($field) && checkdate($field["month"], $field["day"], $field["year"])) 
        $current = mktime(0, 0, 0, $field["month"], $field["day"], $field["year"]);
      else
      {
        $date = $this->dateArray($field);
        if (checkdate($date["month"], $date["day"], $date["year"]))
          $current = mktime(0, 0, 0, $date["month"], $date["day"], $date["year"]);
      }

      /* minimum date */
      $minimum = 0;
      $str_min = $this->m_date_min;
      if (strlen($str_min) == 8)
      {
        $date = $this->dateArray($str_min);
        if (checkdate($date["month"], $date["day"], $date["year"]))
          $minimum = mktime(0, 0, 0, $date["month"], $date["day"], $date["year"]);
        else $str_min = 0;
      }

      /* maximum date */
      $maximum = 0;
      $str_max = $this->m_date_max;
      if (strlen($str_max) == 8)
      {
        $date = $this->dateArray($str_max);
        if (checkdate($date["month"], $date["day"], $date["year"]))
          $maximum = mktime(0, 0, 0, $date["month"], $date["day"], $date["year"]);
        else $str_max = 0;        
      }

      /* did we select a valid date? */
      if      (!empty($current) && !empty($minimum) && $current < $minimum) $current = $minimum;
      else if (!empty($current) && !empty($maximum) && $current > $maximum) $current = $maximum;
      else if (empty($current)  && !empty($minimum) && mktime() < $minimum) $current = $minimum;
      else if (empty($current)  && !empty($maximum) && mktime() > $maximum) $current = $maximum; 
      else if (empty($current)) $current = mktime();
  
      /* get dates in array format */
      $current = getdate($current);
      if (!empty($minimum)) $minimum = getdate($minimum);
      if (!empty($maximum)) $maximum = getdate($maximum);      

      /* minimum and maximum */
      $current["d_min"] = (!empty($minimum) && $current["year"] == $minimum["year"] &&
                           $current["mon"] == $minimum["mon"] ? $minimum["mday"] : 1);
      $current["d_max"] = (!empty($maximum) && $current["year"] == $maximum["year"] &&
                           $current["mon"] == $maximum["mon"] ? $maximum["mday"] : $this->getDays($current));      
      $current["m_min"] = (!empty($minimum) && $current["year"] == $minimum["year"] ? $minimum["mon"] : 1);
      $current["m_max"] = (!empty($maximum) && $current["year"] == $maximum["year"] ? $maximum["mon"] : 12);      
      $current["y_min"] = (!empty($minimum) ? $minimum["year"] : 0);    
      $current["y_max"] = (!empty($maximum) ? $maximum["year"] : 0);

      /* small date selections */
      if (!empty($maximum) && !empty($minimum) && $str_max - $str_min < 25)
      {
        $result = '<select name="'.$fieldprefix.$this->fieldName().'">';
        for ($i = $str_min; $i <= $str_max; $i++)
        { 
          $tmp_date = getdate(mktime(0, 0, 0, substr($i, 4, 2), substr($i, 6, 2), substr($i, 0, 4)));          
          $result .= '<option value="'.$i.'"'.($tmp_date[0] == $current[0] ? ' selected' : '').'>'.$this->formatDate($tmp_date, $str_format).'</option>';
        }
        $result .= '</select>';
        return $result;
      }
      
      /* other date selections */
      for($i = 0; $i < strlen($str_format); $i++)
      {
        /* javascript method */
        $str_script = "AdjustDate(this, '".$fieldprefix.$this->fieldName()."', '$str_format', $str_min, $str_max, ".($this->hasFlag(AF_OBLIGATORY) ? "true" : "false").");";
        
        /* year input box */ 
        if ($str_format[$i] == "y" || $str_format[$i] == "Y")
        {
          /* date must be within 25 years */
          if(!empty($current["y_max"]) && !empty($current["y_min"]) && $current["y_max"] - $current["y_min"] <= 25)
          {
            $result .= '<select name="'.$fieldprefix.$this->fieldName().'[year]" onChange="'.$str_script.'">';
            for($j = $current["y_min"]; $j <= $current["y_max"]; $j++)
            {
              $tmp_date = getdate(mktime(0,0,0, $current["mon"], $current["mday"], $j));          
              $str_year = $this->formatDate($tmp_date, $str_format[$i]);
              $result .= '<option value="'.$j.'" '.($j == $current["year"] ? "selected" : "").'>'.$str_year.'</option>';
            }
            $result .= '</select>';
          }

          /* normal input box */
          else  $result .= '<input type="text" name="'.$fieldprefix.$this->fieldName().'[year]" size="4" maxlength="4" onChange="'.$str_script.'" value="'.$current["year"].'">';
        }
        
        /* day input box */
        elseif ($str_format[$i] == "j" || $str_format[$i] == "d")
        {
          $result .= '<select name="'.$fieldprefix.$this->fieldName().'[day]" onChange="'.$str_script.'">';
					if (!$this->hasFlag(AF_OBLIGATORY)) $result .= '<option value="0"></option>';
          for ($j = $current["d_min"]; $j <= $current["d_max"]; $j++)
          {
            $tmp_date = getdate(mktime(0,0,0, $current["mon"], $j, $current["year"]));
            $str_day = $this->formatDate($tmp_date, $str_format[$i]);
            $result .= '<option value="'.$j.'" '.($j == $current["mday"] ? "selected" : "").'>'.$str_day.'</option>';
          }
          $result .= '</select>';
        }

        /* month input box */
        elseif($str_format[$i] == "m" || $str_format[$i] == "n" || $str_format[$i] == "M" || $str_format[$i] == "F")
        {
          $result .= '<select name="'.$fieldprefix.$this->fieldName().'[month]" onChange="'.$str_script.'">';
					if (!$this->hasFlag(AF_OBLIGATORY)) $result .= '<option value="0"></option>';					
          for ($j = $current["m_min"]; $j <= $current["m_max"]; $j++)
          {
            $tmp_date = getdate(mktime(0,0,0, $j, $current["mday"], $current["year"]));          
            $str_month = $this->formatDate($tmp_date, $str_format[$i]);
            $result .= '<option value="'.$j.'" '.($j == $current["mon"] ? "selected" : "").'>'.$str_month.'</option>';
          }
          $result .= '</select>';
        }
        
        /* other characters */
        else $result .= $str_format[$i];
      }

      /* return result */
      return $result;
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

    /**
     * Returns a piece of html code that can be used in a form to search values.
     * Searching is disabled for the date attribute, we only return a space.
     * @params $record array with 3 fields (year, month, day)
     * @return piece of HTML code
     */
    function search($record="", $extended=false)
    {
      if (!$extended) return "&nbsp;";
      
      // FIXME, we need a way to set the date to 'nothing' (i.e., unselected)
      $res = $this->edit($record, "atksearch_AE_");
      return $res;
    }
    
    function searchCondition(&$query, $table, $value, $searchmode)
    {
      $dateval = $this->value2db(array($this->fieldName()=>$value));
      $is_date = $this->m_tableMeta['type'] == 'date';      
      if($is_date)
      {
        // TODO & FIXME:
        // databases have their own way of comparing dates. (OCI8 needs TO_CHAR etc..
        // so we need to make some $query->dateComparison function..
        atkdebug("Date support for DATE fields not yet supported (FIXME!)");
      }
      else
      {
        // probably a varchar field..
        $query->addCondition($query->exactCondition($table.".".$this->fieldName(),$dateval));
      }      
    }

    /**
     * Convert date array to database value
     * @param $rec array with 3 fields (year, month, day)
     * @return database value for date
     */
    function value2db($rec)
    {
      $is_date = $this->m_tableMeta['type'] == 'date' || $this->m_tableMeta['type'] == 'datetime';      
            
      if (!is_array($rec[$this->fieldName()]))
        $rec[$this->fieldName()] = $this->dateArray($rec[$this->fieldName()]);
        
      $year = $rec[$this->fieldName()]["year"];
      $month = $rec[$this->fieldName()]["month"];
      $day = $rec[$this->fieldName()]["day"];

      if (empty($year) || empty($month) || empty($day)) return "";
      $separator = ($is_date ? "-" :"");
      $result = $year.$separator.sprintf("%02d",$month).$separator.sprintf("%02d",$day);
      return $result;
    }

    /**
     * Convert database value to date array
     * @param $rec database record with date field
     * @return array with 3 fields (year, month, day)
     */
    function db2value($rec)
    {
		 
      $is_date = $this->m_tableMeta['type'] == 'date' || $this->m_tableMeta['type'] == 'datetime';
      if (strlen($rec[$this->fieldName()]) == 0 || (int)substr($rec[$this->fieldName()], 0, 4) == 0) return "";
      return array("year" =>substr($rec[$this->fieldName()], 0, 4),
                   "month"=>substr($rec[$this->fieldName()], $is_date ? 5 : 4, 2),
                   "day"  =>substr($rec[$this->fieldName()], $is_date ? 8 : 6, 2));
    }
    
    /**
     * Return the HTTP post values for this attribute
     * @param $postvars the HTTP post vars
     * @return array with 3 fields (year, month, day)
     */
    function fetchValue($postvars)
    {
      $result = $postvars[$this->formName()];
      return $result;
    }
    
    /**
     * Validate's dates
     * @param &$record Record that contains value to be validated.
     *                 Errors are saved in this record
     * @param $mode can be either "add" or "update"
     * @return $record
     */
    function validate(&$record, $mode)
    {
      $value = $record[$this->fieldName()];
      /* array or no array */
      if (is_array($value)) $date = $this->dateArray($value);
		
      /* if not obligatory and one of the fields is null then the date will be saved as null */
      if (!$this->hasFlag(AF_OBLIGATORY) && (empty($year) || empty($month) || empty($day))) return;
		
      /* currently selected date */
      if (checkdate($value["month"], $value["day"], $value["year"])) 
        $current = mktime(0, 0, 0, $value["month"], $value["day"], $value["year"]);
      else 
      {
        triggerError($record, $this->fieldName(), 'error_date_invalid');
        return;
      }

      /* allright, if not obligatory, and we have come all this way, we'll bail out */
			if (!$this->hasFlag(AF_OBLIGATORY)) return;

      /* minimum date */
      $minimum = 0;
      $str_min = $this->m_date_min;
      if (strlen($str_min) == 8)
      {
        $date = $this->dateArray($str_min);
        if (checkdate($date["month"], $date["day"], $date["year"]))
          $minimum = mktime(0, 0, 0, $date["month"], $date["day"], $date["year"]);
        else $str_min = 0;
      }

      /* maximum date */
      $maximum = 0;
      $str_max = $this->m_date_max;
      if (strlen($str_max) == 8)
      {
        $date = $this->dateArray($str_max);
        if (checkdate($date["month"], $date["day"], $date["year"]))
          $maximum = mktime(0, 0, 0, $date["month"], $date["day"], $date["year"]);
        else $str_max = 0;        
      }      
      
      /* date < minimum */
      if (!empty($minimum) && $current < $minimum)
      {
        triggerError($record, $this->fieldName(), 'error_date_minimum', text("error_date_minimum")." ". $this->formatDate(getdate($minimum), $this->m_date_format_view, 0));
        return;
      }
        
      /* date > maximum */
      if (!empty($maximum) && $current > $maximum)
        triggerError($record, $this->fieldName(), 'error_date_maximum', text("error_date_maximum")." ". $this->formatDate(getdate($maximum), $this->m_date_format_view, 0));
        
      /* no problem */
      //return "";
    }

    /**
     * Function display's the date
     * @param $record array with date
     * @return formatted date string
     */
    function display($record)
    {
      $value = $record[$this->fieldName()];
      $tmp_date = getdate(mktime(0, 0, 0, $value["month"], $value["day"], $value["year"]));
			if (!empty($tmp_date)) return $this->formatDate($tmp_date, $this->m_date_format_view, 0);
			else return "";
    }
  }
?>