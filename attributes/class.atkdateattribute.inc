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
   * Revision 1.1  2001/04/23 10:17:08  ivo
   * Initial revision
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
    var $date_min;
    var $date_max;
    var $date_format_edit;
    var $date_format_view;

    // english month names short and long, will be overruled by other languages
    var $m_options_short = Array(1 => "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec");
    var $m_options_long  = Array(1 => "january", "february", "march", "april", "may", "june",
                                 "july", "august", "september", "october", "november", "december");
    
    /**
     * Format date according to a format string
     * @param $date date string formatted: YYYYMMDD
     * @param $format format string, compatible with PHP's date format functions
     * @return string with formatted date
     */
    function formatdate($date, $format)
    {
      $year = substr($date, 0, 4);
      $month = substr($date, 4, 2);
      $day = substr($date, 6, 2);
      $pattern = array("/y/", "/Y/", "/j/", "/d/", "/m/", "/n/", "/M/", "/F/");
      $replace = array(substr($year, 2, 2), $year, (int)$day, $day, (int)$month, $month, "MONTH_SHORT", "MONTH_LONG");
      $date = preg_replace($pattern, $replace, $format);
      $date = preg_replace(array("/MONTH_SHORT/", "/MONTH_LONG/"),
                           array(text($this->m_options_short[(int)$month]), text($this->m_options_long[(int)$month])),
                           $date);
      return $date;
    }

    /**
     * Returns the days in a certain month in a certain year
     * @param $year the year
     * @param $month the month
     * @return integer with number of days
     */
    function daysInMonth($month, $year)
    {
      $days = array(1 => 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
      if(($year % 4 == 0) && (!($year % 100 == 0) || ($year % 400 == 0))) $days[2] = 29;
      return $days[(int)$month];
    }

    /**
     * Returns an array with month numbers in a certain year
     * @param $year the year we are talking about
     * @param $min the minimum date
     * @param $max the maximum date
     * @return array with months (integers)
     */
    function getMonths($year, $min, $max)
    {
      $year_min = substr($min, 0, 4);
      $year_max = substr($max, 0, 4);
      $month_min = substr($min, 4, 2);
      $month_max = substr($max, 4, 2);

      $months = array();
      if ($min && $max && $year == $year_min && $year == $year_max) for ($i = $month_min; $i <= $month_max; $i++) $months[] = (int)$i;
      else if ($min && $year == $year_min) for ($i = $month_min; $i <= 12; $i++) $months[] = (int)$i;
      else if ($max && $year == $year_max) for ($i = 1; $i <= $month_max; $i++) $months[] = (int)$i;
      else for($i = 1; $i <= 12; $i++) $months[] = (int)$i;
      
      return $months;
    }

    /**
     * Returns an array with day numbers in a certain month in a certain year
     * @param $month the month we are talking about
     * @param $year the year we are talking about
     * @param $min the minimum date
     * @param $max the maximum date
     * @return array with days (integers)
     */
    function getDays($month, $year, $min, $max)
    {
      $year_min = substr($min, 0, 4);
      $year_max = substr($max, 0, 4);
      $month_min = substr($min, 4, 2);
      $month_max = substr($max, 4, 2);
      $day_min = substr($min, 6, 2);
      $day_max = substr($max, 6, 2);

      $days = array();

      if ($min && $max && $year == $year_min && $year == $year_max)
      {
	      if ($month == $month_min && $month == $month_max) for ($i = $day_min; $i <= $day_max && $i <= $this->daysInMonth($month, $year); $i++) $days[] = (int)$i; 
	      if ($month == $month_min) for ($i = $day_min; $i <= $this->daysInMonth($month, $year); $i++) $days[] = (int)$i; 
	      if ($month == $month_max) for ($i = 1; $i <= $day_max && $i <= $this->daysInMonth($month, $year); $i++) $days[] =(int) $i; 
	      else for ($i = 1; $i <= $this->daysInMonth($month, $year); $i++) $days[] = (int)$i; 
      }

      else if ($min && $year == $year_min && $month == $month_min)
        for ($i = $day_min; $i <= $this->daysInMonth($month, $year); $i++) $days[] = (int)$i; 

      else if ($max && $year == $year_max && $month == $month_max)
        for ($i = 1; $i <= $day_max && $i <= $this->daysInMonth($month, $year); $i++) $days[] = (int)$i; 

      else 
        for ($i = 1; $i <= $this->daysInMonth($month, $year); $i++) $days[] = (int)$i;

      return $days;
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
    function atkDateAttribute($name, $format_edit="F d Y", $format_view="F d Y", $min=0, $max=0, $flags=0)
    {
      // *** API CHANGE HACK ***
      // This function used to have the parameters: ($name, $flags=0) but because of
      // the new features of the atkDateAttribute this has changed as you can see above.
      // However, to remain backward compatible with modules based on the old api we
      // (try) to detect which api is used, and initialize accordingly.

      if (is_int($format_edit))
      {
        // old api use detected...
        $flags = $format_edit;
        $format_edit = "F d Y";
        $format_view = "F d Y";
        $date_min = 0;
        $date_max = 0;
      }

      $this->date_format_edit = $format_edit;
      $this->date_format_view = $format_view;
      $this->date_min = $min;
      $this->date_max = $max;

      // register javascript
      $GLOBALS['g_layout']->register_script("atk/javascript/class.atkdateattribute.js.php");

      // base class constructor
      $this->atkAttribute($name,$flags);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value. (Month will be a dropdownbox, year and day text fields)
     * @params $record Array with 3 fields (year, month, day)
     * @return Piece a of HTML Code
     */
    function edit($record="")
    {
      $min    = $this->date_min;
      $max    = $this->date_max;
      $format = $this->date_format_edit;

      // default date / set date
      if (is_array($record[$this->fieldName()]))
      {
        $set_day = (int)$record[$this->fieldName()]["day"];
        $set_month = (int)$record[$this->fieldName()]["month"];
        $set_year = (int)$record[$this->fieldName()]["year"];
        $set_date = sprintf("%04d%02d%02d", $set_year, $set_month, $set_day);
      }
      else
      {
        $set_day = date("j");
        $set_month = date("n");
        $set_year = date("Y");
        $set_date = sprintf("%04d%02d%02d", $set_year, $set_month, $set_day);
      }

      if ($min && $set_date < $min || $min && $max && $set_date > $max)
      {
        $set_date = $min;
	      $set_year = substr($set_date, 0, 4);
	      $set_month = substr($set_date, 4, 2);
	      $set_day = substr($set_date, 6, 2);
      }
   
      if($min && $max)
      {
        // selections with just a few days
        if ($max - $min <= 25)
        {
          $result = '<select name="'.$this->fieldName().'">';
          for ($i = $min; $i <= $max; $i++) $result .= '<option value="'.$i.'">'.$this->formatdate($i, $format).'</option>';
          $result .= '</select>';
        }

        else
        {
          for($i = 0; $i < strlen($format); $i++)
          {
            // script
            $jscript = "AdjustDate(this.form, '".$this->fieldName()."', '".$format."', $min, $max);";

            // year
            if ($format[$i] == "y" || $format[$i] == "Y")
            {
              // minimum / maximum year
              $year_min = substr($min, 0, 4);
              $year_max = substr($max, 0, 4);

              // less then or 25 years
              if($year_max - $year_min <= 25)
              {
                $result .= '<select name="'.$this->fieldName().'[year]" onChange="'.$jscript.'">';
                for($j = $year_min; $j <= $year_max; $j++) $result .= '<option value="'.$j.'" '.($j == $set_date ? "selected" : "").'>'.$this->formatdate($j."0000", $format[$i]).'</option>';
                $result .= '</select>';
              }

              // else input box (always years in 4 digits!)
              else $result .= '<input type="text" name="'.$this->fieldName().'[year]" size="4" maxlength="4" onChange="'.$jscript.'" value="'.$set_date.'">';
            }

            // day
            elseif ($format[$i] == "j" || $format[$i] == "d")
            {
              // selection box
              $result .= '<select name="'.$this->fieldName().'[day]" onChange="'.$jscript.'">';
              $days = $this->getDays($set_month, $set_year, $min, $max);
              while (list ($key, $j) = each ($days))
                $result .= '<option value="'.sprintf("%02d",$j).'" '.($j == $set_day ? "selected" : "").'>'.($format[$i] == "d" && $j < 10 ? "0" : "").$j.'</option>';
              $result .= '</select>';
            }

            // month
            elseif($format[$i] == "m" || $format[$i] == "n" || $format[$i] == "M" || $format[$i] == "F")
            {
              // select
              $result .= '<select name="'.$this->fieldName().'[month]" onChange="'.$jscript.'">';

              // options
	            $months = $this->getMonths($set_year, $min, $max);
              while (list ($key, $j) = each ($months))
              {
                $result .= "<option value=\"".sprintf("%02d",$j)."\" ".($j == $set_month ? "selected" : "").">";
                $result .= ($format[$i] == "n" || $format[$i] == "m") ? (int)$j : $this->formatdate("0000".sprintf("%02d",$j)."00", $format[$i]);
                $result .= "</option>";
              }
  
              // end select
              $result .= '</select>';
            }

            // just some character
            else $result .= $format[$i];
          }
        }
      }

      // unlimited date attribute
      else
      {
        if ($min && $set_date < $min) $set_date = $min;
	      else if ($max && $set_date > $max) $set_date = $max;

        for($i = 0; $i < strlen($format); $i++)
        {
          $jscript = "AdjustDate(this.form, '".$this->fieldName()."', '".$format."', $min, $max);";

          // year
          if($format[$i] == "y" || $format[$i] == "Y")
          {
            $result .= '<input type="text" name="'.$this->fieldName().'[year]" size="4" maxlength="4" onChange="'.$jscript.'" value="'.$set_year.'">';
          }

          // day
          elseif($format[$i] == "j" || $format[$i] == "d")
          {
            $result .= '<select name="'.$this->fieldName().'[day]" onChange="'.$jscript.'">';
            $days = $this->getDays($set_month, $set_year, $min, $max);
            while (list ($key, $j) = each ($days))
              $result .= '<option value="'.sprintf("%02d",$j).'" '.($j == $set_day ? "selected" : "").'>'.($format[$i] == "d" && $j < 10 ? "0" : "").$j.'</option>';
            $result .= '</select>';
          }

          // month
          elseif($format[$i] == "m" || $format[$i] == "n" || $format[$i] == "M" || $format[$i] == "F")
          {
            $result .= '<select name="'.$this->fieldName().'[month]" onChange="'.$jscript.'">';
            $months = $this->getMonths($set_year, $min, $max);
            while (list ($key, $j) = each ($months))
            {
              $result .= "<option value=\"".sprintf("%02d",$j)."\" ".($j == $set_month ? "selected" : "").">";
              $result .= ($format[$i] == "n" || $format[$i] == "m") ? (int)$j : $this->formatdate("0000".sprintf("%02d",$j)."00", $format[$i]);
              $result .= "</option>";
            }
            $result .= '</select>';
          }

          // other characters
          else $result .= $format[$i];
        }
      }

      return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="")
    {
      $field = $record[$this->fieldName()];

      if (is_array($field))
        for (reset($field); list($key, $value) = each($field);)
          $result .= '<input type="hidden" name="'.$this->formName().'['.$key.']" '.'value="'.$value.'">';
      else  $result = '<input type="hidden" name="'.$this->formName().'" value="'.$field.'">';

      return $result;             
    }

    /**
     * Returns a piece of html code that can be used in a form to search values.
     * @params $record Array with 3 fields (year, month, day)
     * @return Piece a of HTML Code
     */
    function search($record="")
    {
      return "&nbsp;";
    }

    /**
     * Convert Date Array to String
     * @param $rec Array with 3 fields (year, month, day)
     * @return String with YYYY-MM-DD
     */
    function value2db($rec)
    {
      $is_date = $this->m_tableMeta[$this->fieldName()]['type'] == 'date';
            
      if(is_array($rec[$this->fieldName()]))
      {
        $year = $rec[$this->fieldName()]["year"];
        $month = $rec[$this->fieldName()]["month"].($is_date ? "-" :"");
        $day = $rec[$this->fieldName()]["day"];
      }
      else
      {
        $year = substr($rec[$this->fieldName()], 0, 4);
        $month = substr($rec[$this->fieldName()], 4, 2);
        $day = substr($rec[$this->fieldName()], 6, 2);
      }

      if (strlen($year) == 0) return "";

      $separator = ($is_date ? "-" :"");

      $res = $year.$separator.sprintf("%02d",$month).$separator.sprintf("%02d",$day);
    
      return $res;
    }

    /**
     * Convert Date String to Array
     * @param $rec String with date (YYYY-MM-DD)
     * @return Array with 3 fields (year, month, day)
     */
    function db2value($rec)
    {
      $is_date = $this->m_tableMeta[$this->fieldName()]['type'] == 'date';

      if (strlen($rec[$this->fieldName()]) == 0 || (int)substr($rec[$this->fieldName()], 0, 4) == 0) return "";
      
      return Array("year" =>substr($rec[$this->fieldName()], 0, 4),
                   "month"=>substr($rec[$this->fieldName()], $is_date ? 5 : 4, 2),
                   "day"  =>substr($rec[$this->fieldName()], $is_date ? 8 : 6, 2));

    }
    
    function fetchValue($postvars)
    {
      $res = $postvars[$this->formName()];
      return $res;
    }

    /**
     * Function display's the date
     * @param $record Array with date
     * @return String with YYYY-MM-DD
     */
    function display($record)
    {
      $format = $this->date_format_view;
      $value = $record[$this->fieldName()];
      $value = $value["year"].$value["month"].$value["day"];
      return $this->formatdate($value, $format);
    }
  }
?>