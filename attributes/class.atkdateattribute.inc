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
   * @internal baseclass include
   */
  useattrib("atkattribute");

  /** flag(s) specific for atkDateAttribute */
  define("AF_DATE_STRING",              AF_SPECIFIC_1); // date must be entered as an english date string (strtotime), also checks edit format
  define("AF_DATE_EMPTYFIELD",          AF_SPECIFIC_2); // Fields have one empty option
  define("AF_DATE_NO_CALENDAR",         AF_SPECIFIC_3); // Do not append the popup calendar.
  define("AF_DATE_DISPLAY_DAY",         AF_SPECIFIC_4); // Show the day of the week in the display
  define("AF_DATE_EDIT_NO_DAY",         AF_SPECIFIC_5); // Don't display the day of the week in edit mode

  // Include adodb workarounds for <1970 dates on windows.
  $config_atkroot = atkconfig("atkroot");
  require_once($config_atkroot."atk/utils/adodb-time.inc.php");
  // Include browsertools for PDA detection
  include_once($config_atkroot."atk/atkbrowsertools.inc");

  /**
   * The atkDateAttribute class offers a date widget for date fields.
   * @author Peter C. Verhage <peter@achievo.org>
   * @author Tom Schenkenberg <tom@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkDateAttribute extends atkAttribute
  {
    /**
     * Static var to keep track if js scripts are already registered.
     *
     * @var bool
     */
    protected static $s_baseScriptsRegistered = false;

    var $m_date_min;
    var $m_date_max;
    var $m_date_format_edit;
    var $m_date_format_view;
    var $m_onchangecode = array();
    var $m_maxyears = 25;

    /**
     * Format date according to a format string
     * @param $date date array (gotten with getdate())
     * @param $format format string, compatible with PHP's date format functions
     * @param $weekday include day-of-week or not
     * @return string with formatted date
     */
    function formatDate($date, $format, $weekday=1)
    {
      static $langcache = array();

      /* format month */
      $format = str_replace("M", "%-%",   $format);
      $format = str_replace("F", "%=%",   $format);

      /* format day */
      $format = str_replace("D", "%&%", $format);
      $format = str_replace("l", "%*%", $format);

      if ($weekday && strpos($format, '%&%') === FALSE && strpos($format, '%*%') === FALSE)
      {
        $format = str_replace("d", "%*% d", $format);
        $format = str_replace("j", "%*% j", $format);
      }

      /* get date string */
      $str_date = adodb_date($format, $date[0]);

      $month = $date['month'];
      $shortmonth = substr(strtolower($date["month"]), 0, 3);

      /* store the text calls */
      if(!isset($langcache[$month]))
      {
        $langcache[$month]= atktext(strtolower($month),"atk");
      }

      if(!isset($langcache[$shortmonth]))
      {
        $langcache[$shortmonth] = atktext($shortmonth);
      }

      /* replace month/week name */
      $str_date = str_replace("%-%", $langcache[$shortmonth], $str_date);
      $str_date = str_replace("%=%", $langcache[$month], $str_date);
      $str_date = str_replace("%*%", atktext(strtolower($date["weekday"]),"atk"), $str_date);
      $str_date = str_replace("%&%", atktext(substr(strtolower($date["weekday"]), 0, 3),"atk"), $str_date);

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
      if (isset($date["mon"])&&isset($date["year"]))
      {
        $date = adodb_getdate(adodb_mktime (0, 0, 0, $date["mon"]+1, 0, $date["year"]));
        return $date["mday"];
      }
      return "";
    }

    /**
     * Converts a date string (YYYYMMDD) to an
     * array with 3 fields (day, month, year).
     * @param date the date string
     * @return array with 3 fields (day, month, year)
     */
    function dateArray($date)
    {
      if(strstr($date,'-'))
      {
        return array("day" => substr($date, 8, 2),
                     "month" => substr($date, 5, 2),
                     "year" => substr($date, 0, 4));
      }
      else
      {
      return array("day" => substr($date, 6, 2),
                   "month" => substr($date, 4, 2),
                   "year" => substr($date, 0, 4));
      }
    }

   /**
     * Converts a date array to a timestamp
     * year, month, day are obligatory !!
     *
     * @param array $dateArray Date Array
     * @return int Timestamp
     */
    function arrayToTime($dateArray)
    {
      return atkDateAttribute::_arrayToTime($dateArray);
    }

    /**
     * Validates a given date array
     * @param array Array with 3 fields (day, month, year)
     * @return boolean True if valid, false if not.
     */
    function checkDateArray($datearray)
    {
      return checkdate((int)$datearray["month"], (int)$datearray["day"], (int)$datearray["year"]);
    }

    /**
     * Converts a date array to a timestamp
     * year, month, day are obligatory !!
     *
     * @param array $dateArray Date Array
     * @return int Timestamp
     */
    function _arrayToTime($dateArray)
    {
      $hour = 0;
      $min = 0;
      $sec = 0;
      $dateValid = true;

      if(!empty($dateArray["hour"])) $hour = $dateArray["hour"];
      if(!empty($dateArray["min"])) $min = $dateArray["min"];
      if(!empty($dateArray["sec"])) $sec = $dateArray["sec"];
      if(!empty($dateArray["day"])) $day = $dateArray["day"];
      else $dateValid = false;
      if(!empty($dateArray["month"])) $month = $dateArray["month"];
      else $dateValid = false;
      if(!empty($dateArray["year"])) $year = $dateArray["year"];
      else $dateValid = false;

      if($dateValid) return adodb_mktime($hour,$min,$sec,$month,$day,$year);
      else return adodb_mktime(0,0,0);
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
      /* *** API SUPPORT HACK ***
       * Because of backwards compatability and because of the number
       * of arguments this method has we also support the old API: ($name, $flags=0).
       */
      if (is_int($format_edit))
      {
        $flags = $format_edit;
        $format_edit = "";
        $format_view = "";
        $min = 0;
        $max = 0;
      }

      /* edit and display date format */
      $this->setFormatEdit($format_edit);
      $this->setFormatView($format_view);

      /* max / min date */
      $this->setDateMin($min);
      $this->setDateMax($max);

      /* base class constructor */
      $this->atkAttribute($name, $flags);

     // If this attribute is obligatory set the initial value to the current timestamp
     if ($this->hasFlag(AF_OBLIGATORY))
     {
       $this->setInitialValue(atkDateAttribute::dateArray(date('Ymd')));
     }
    }

    /**
     * Set the format for the boxes in edit mode.
     *
     * @param String $format_edit The format (see format for date() function)
     */
    function setFormatEdit($format_edit)
    {
      $txt_date_format_edit = atktext("date_format_edit","atk","","","",true);

      if ($this->hasFlag(AF_DATE_STRING) && empty($format_edit)) $this->m_date_format_edit = "Y-m-d";
      elseif (!empty($format_edit)) $this->m_date_format_edit = $format_edit;
      elseif (!empty($txt_date_format_edit)) $this->m_date_format_edit = $txt_date_format_edit;
      else $this->m_date_format_edit = "F j Y";
    }

    /**
     * Set the format for the boxes in view mode.
     *
     * @param String $format_view The format (see format for date() function)
     */
    function setFormatView($format_view)
    {
      $txt_date_format_view = atktext("date_format_view","atk","","","",true);

      if     (!empty($format_view))           $this->m_date_format_view = $format_view;
      elseif (!empty($txt_date_format_view))  $this->m_date_format_view = $txt_date_format_view;
      else                                    $this->m_date_format_view = "F j Y";
    }

    /**
     * Set the maximum date that may be select (0 means unlimited).
     * It can be set in 3 formats:
     * 1. Unix timestamp.
     * 2. String (parsed by strtotime)
     * 3. Array (with year,month,day,hour,min,sec)
     *
     * @param mixed $max The maximum date that may be selected.
     */
    function setDateMax($max=0)
    {
      if      ($max === 0)        $this->m_date_max = 0;
      else if (is_array($max))    $this->m_date_max = $this->_arrayToTime($max);
      else if (is_integer($max))  $this->m_date_max = $max;
      else                        $this->m_date_max = strtotime($max);
    }

    /**
     * Set the minimum date that may be select (0 means unlimited).
     * It can be set in 3 formats:
     * 1. Unix timestamp.
     * 2. String (parsed by strtotime)
     * 3. Array (with year,month,day,hour,min,sec)
     *
     * @param mixed $min The minimum date that may be selected.
     */
    function setDateMin($min=0)
    {
      if      ($min === 0)        $this->m_date_min = 0;
      else if (is_array($min))    $this->m_date_min = $this->_arrayToTime($min);
      else if (is_integer($min))  $this->m_date_min = $min;
      else                        $this->m_date_min = strtotime($min);
    }

    function edit($record="", $fieldprefix="", $mode="")
    {
      return $this->draw($record, $fieldprefix, "", $mode, $this->hasFlag(AF_OBLIGATORY));
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value. (Month will be a dropdownbox, year and day text fields)
     * @todo We can't show a calendar when we have a year dropdown?
     * @todo The calendar doesn't use the min/max values?
     * @param array $record Array with 3 fields (year, month, day)
     * @return Piece a of HTML Code
     */
    function draw($record="", $fieldprefix="", $postfix="", $mode="", $obligatory=false)
    {
      $result = "";

      $pda = browserInfo::detectPDA();
      $yeardropdown = false;

      if (!$pda)
      {
        self::registerScriptsAndStyles(!$this->hasFlag(AF_DATE_NO_CALENDAR));
      }

      $fieldname = $fieldprefix.$this->fieldName().$postfix;

      /* text mode? */
      if ($this->hasFlag(AF_DATE_STRING) || $mode == 'list')
      {
        $value = &$record[$this->fieldName()];

        if (is_array($value))
          $value = adodb_date($this->m_date_format_edit, adodb_mktime(0, 0, 0, $value["month"], $value["day"], $value["year"]));
        elseif ($obligatory)
          $value = adodb_date($this->m_date_format_edit);
        else
          $value = "";

        $fieldname = $fieldname.'[date]';
        $this->registerKeyListener($fieldname, KB_CTRLCURSOR|KB_UPDOWN);
        $result = '<input type="text" id="'.$fieldname.'" class="atkdateattribute" name="'.$fieldname.'" value="'.$value.'" size="10">';

        if (!$this->hasFlag(AF_DATE_NO_CALENDAR) && $mode != 'list')
        {
          $format = str_replace(array("y", "Y", "m", "n", "j", "d"), array("yy", "y", "mm", "m", "d", "dd"), $this->m_date_format_edit);
          $result.=' <input '.$this->getCSSClassAttribute(array("button", "atkbutton")).' type="button" value="..." onclick="return showCalendar(\''.$fieldname.'\', \''.$fieldname.'\', \''.$format.'\', false);">';
        }
        return $result;
      }

      /* this field */
      $field = atkArrayNvl($record, $this->fieldName());
      $str_format = $this->m_date_format_edit;

      /* currently selected date */
      if (is_array($field) && $field["year"] == 0 && $field["month"] == 0 && $field["day"] == 0)
        $current = NULL; /* NULL date selected (normal date selection) */
      elseif (!is_array($field) && empty($field))
        $current = NULL; /* NULL date selected (NULL value in database) */
      elseif (is_array($field))
      {
        if ($this->checkDateArray($field))
        {
          $current = adodb_mktime(0, 0, 0, $field["month"], $field["day"], $field["year"]);
        }
        else
        {
          $current = NULL;
          triggerError($record, $this->fieldName(), "error_date_invalid");
        }
      }
      else
      {
        $date = $this->dateArray($field);
        if ($this->checkDateArray($date))
        {
          $current = adodb_mktime(0, 0, 0, $date["month"], $date["day"], $date["year"]);
        }
        else
        {
          $current = NULL;
        }
      }

      /* minimum date */
      $minimum = $this->m_date_min;
      if ($minimum != 0) $str_min = adodb_date("Ymd", $minimum);
      else $str_min = 0;

      /* maximum date */
      $maximum = $this->m_date_max;
      if ($maximum != 0) $str_max = adodb_date("Ymd", $maximum);
      else $str_max = 0;

      /* did we select a valid date? */
      if ($current === NULL && !$this->hasFlag(AF_OBLIGATORY)); // NULL date selection; ignore this if statement
      elseif (!empty($current) && !empty($minimum) && $current < $minimum) $current = $minimum;
      elseif (!empty($current) && !empty($maximum) && $current > $maximum) $current = $maximum;

      /* get dates in array format */
      if ($current !== NULL) $current = adodb_getdate($current);
      if (!empty($minimum))  $minimum = adodb_getdate($minimum);
      if (!empty($maximum))  $maximum = adodb_getdate($maximum);

      /* minimum and maximum */
      $current["d_min"] = (!empty($minimum) && $current["year"] == $minimum["year"] && $current["mon"] == $minimum["mon"] ? $minimum["mday"] : 1);
      $current["d_max"] = (!empty($maximum) && $current["year"] == $maximum["year"] && $current["mon"] == $maximum["mon"] ? $maximum["mday"] : $this->getDays($current));
      $current["m_min"] = (!empty($minimum) && $current["year"] == $minimum["year"] ? $minimum["mon"] : 1);
      $current["m_max"] = (!empty($maximum) && $current["year"] == $maximum["year"] ? $maximum["mon"] : 12);
      $current["y_min"] = (!empty($minimum) ? $minimum["year"] : 0);
      $current["y_max"] = (!empty($maximum) ? $maximum["year"] : 0);

      /* small date selections, never possible is field isn't obligatory (no min/max date) */
      if (!empty($maximum) && !empty($minimum) && $str_max - $str_min < 25)
      {
        $this->registerKeyListener($fieldname, KB_CTRLCURSOR|KB_LEFTRIGHT);
        $result = '<select id="'.$fieldname.'" name="'.$fieldname.'">';
        for ($i = $str_min; $i <= $str_max; $i++)
        {
          $tmp_date = adodb_getdate(adodb_mktime(0, 0, 0, substr($i, 4, 2), substr($i, 6, 2), substr($i, 0, 4)));
          $result .= '<option value="'.$i.'"'.($current !== NULL && $tmp_date[0] == $current[0] ? ' selected' : '').'>'.$this->formatDate($tmp_date, $str_format, !$this->hasFlag(AF_DATE_EDIT_NO_DAY)).'</option>';
        }
        $result .= '</select>';
        return $result;
      }

      if ($this->hasFlag(AF_DATE_EMPTYFIELD))  $emptyfield=true;
      else if (!$obligatory) $emptyfield=true;
      else $emptyfield=false;

      $info = array(
        'format' => $str_format,
        'min' => $str_min,
        'max' => $str_max,
        'emptyfield' => $emptyfield,
        'weekday' => !$this->hasFlag(AF_DATE_EDIT_NO_DAY)
      );

      atkimport('atk.utils.atkjson');
      if (!$pda) $result .= '<script language="javascript">var atkdateattribute_'.$fieldname.' = '.atkJSON::encode($info).';</script>';

      /* other date selections */
      $weekdayFormat = NULL;
      for($i = 0; $i < strlen($str_format); $i++)
      {
        /* javascript method */
        if (!$pda) $str_script = "AdjustDate(this, '".$fieldname."');";

        if (count($this->m_onchangecode))
        {
          $this->renderChangeHandler($fieldprefix);
          $str_script .= $this->getHtmlId($fieldprefix).'_onChange(this);"';
        }

        /* year input box */
        if ($str_format[$i] == "y" || $str_format[$i] == "Y")
        {
          /* date must be within specified (default: 25) years */
          if(!empty($current["y_max"]) && !empty($current["y_min"]) && $current["y_max"] - $current["y_min"] <= $this->m_maxyears)
          {
            $this->registerKeyListener($fieldname.'[year]', KB_CTRLCURSOR|KB_LEFTRIGHT);
            $result .= '<select id="'.$fieldname.'[year]" name="'.$fieldname.'[year]" class="atkdateattribute" onChange="'.$str_script.'">';
            if (!$obligatory || $this->hasflag(AF_DATE_EMPTYFIELD))
              $result .= '<option value="0"'.($current === NULL ? ' selected' : '').'></option>';
            for($j = $current["y_min"]; $j <= $current["y_max"]; $j++)
            {
              if(empty($current["mon"])) $current["mon"] = 1;
              if(empty($current["mday"])) $current["mday"] = 1;
              $tmp_date = adodb_getdate(adodb_mktime(0,0,0, $current["mon"], $current["mday"], $j));
              $str_year = $this->formatDate($tmp_date, $str_format[$i]);
              $result .= '<option value="'.$j.'" '.($current !== NULL && $j == $current["year"] ? "selected" : "").'>'.$str_year.'</option>';
            }
            $result .= '</select>';
            $yeardropdown = true;
          }

          /* normal input box */
          else
          {
            $this->registerKeyListener($fieldname.'[year]', KB_CTRLCURSOR|KB_UPDOWN);
            $result .= '<input type="text" id="'.$fieldname.'[year]" name="'.$fieldname.'[year]" class="atkdateattribute" size="4" maxlength="4" onChange="'.$str_script.'" value="'.(isset($current["year"])?$current["year"]:"").'">';
          }
        }

        /* weekday format */
        elseif ($str_format[$i] == 'D' || $str_format[$i] == 'l')
        {
          $weekdayFormat = $str_format[$i];
        }

        /* day input box */
        elseif ($str_format[$i] == "j" || $str_format[$i] == "d")
        {
          $this->registerKeyListener($fieldname.'[day]', KB_CTRLCURSOR|KB_LEFTRIGHT);
          $result .= '<select id="'.$fieldname.'[day]" name="'.$fieldname.'[day]" class="atkdateattribute" onChange="'.$str_script.'">';
          if (!$obligatory || $this->hasflag(AF_DATE_EMPTYFIELD))
            $result .= '<option value="0"'.($current === NULL ? ' selected' : '').'></option>';
          if (!$pda)
          {
            for ($j = $current["d_min"]; $j <= $current["d_max"]; $j++)
            {
              $tmp_date = adodb_getdate(adodb_mktime(0,0,0, $current["mon"], $j, $current["year"]));
              if (($current['year']!="") && ($current['mon']!=""))
              {
                $str_day = $this->formatDate($tmp_date, (empty($weekdayFormat) ? $str_format[$i] : "$weekdayFormat {$str_format[$i]}"), !$this->hasFlag(AF_DATE_EDIT_NO_DAY));
              }
              else
              {
                $str_day = $this->formatDate($tmp_date, (empty($weekdayFormat) ? $str_format[$i] : "$weekdayFormat {$str_format[$i]}"), 0);
              }
              $result .= '<option value="'.$j.'" '.($current !== NULL && $j == $current["mday"] ? "selected" : "").'>'.$str_day.'</option>';
            }
          }
          else
          {
            for ($j = 0; $j <= 31; $j++)
            {
              $tmp_date = adodb_getdate(adodb_mktime(0,0,0, $current["mon"], $j, $current["year"]));
              if (($current['year']!="") && ($current['mon']!=""))
              {
                $str_day = $this->formatDate($tmp_date, $str_format[$i], !$this->hasFlag(AF_DATE_EDIT_NO_DAY));
              }
              else
              {
                $str_day = $this->formatDate($tmp_date, $str_format[$i],0);
              }
              $result .= '<option value="'.$j.'" '.($current !== NULL && $j == $current["mday"] ? "selected" : "").'>'.$str_day.'</option>';
            }
          }
          $result .= '</select>';
        }

        /* month input box */
        elseif($str_format[$i] == "m" || $str_format[$i] == "n" || $str_format[$i] == "M" || $str_format[$i] == "F")
        {
          $this->registerKeyListener($fieldname.'[month]', KB_CTRLCURSOR|KB_LEFTRIGHT);
          $result .= '<select id="'.$fieldname.'[month]" name="'.$fieldname.'[month]" class="atkdateattribute" onChange="'.$str_script.'">';
          if (!$obligatory || $this->hasflag(AF_DATE_EMPTYFIELD))
            $result .= '<option value="0"'.($current === NULL ? ' selected' : '').'></option>';
          if (!$pda)
          {
            for ($j = $current["m_min"]; $j <= $current["m_max"]; $j++)
            {
              $tmp_date = adodb_getdate(adodb_mktime(0,0,0, $j,1, (isset($current["year"])?$current["year"]:0)));
              $str_month = $this->formatDate($tmp_date, $str_format[$i]);
              $result .= '<option value="'.$j.'" '.(isset($current["mon"]) && $j == $current["mon"] ? "selected" : "").'>'.$str_month.'</option>';
            }
          }
          else
          {
            for ($j = 0; $j <= 12; $j++)
            {
              $tmp_date = adodb_getdate(adodb_mktime(0,0,0, $j,1, $current["year"]));
              $str_month = $this->formatDate($tmp_date, $str_format[$i]);
              $result .= '<option value="'.$j.'" '.($current !== NULL && $j == $current["mon"] ? "selected" : "").'>'.$str_month.'</option>';
            }
          }
          $result .= '</select>';
        }

        /* other characters */
        else $result .= $str_format[$i];
      }

      if (!$this->hasFlag(AF_DATE_NO_CALENDAR) && !$yeardropdown && !$pda && $mode != 'list')
      {
        $result.=' <input '.$this->getCSSClassAttribute(array("button", "atkbutton")).' type="reset" value="..." onclick="return showCalendar(\''.$fieldname.'\', \''.$fieldname.'[year]\', \'y-mm-dd\', true);">';
      }

      /* return result */
      return $result;
    }

    /**
     * Registers the scripts and styles for the date attribute. Can be used
     * to load the scripts beforehand from another location.
     *
     * @param boolean $useCalendar use calendar widget? (defaults to true)
     */
    public static function registerScriptsAndStyles($useCalendar=true)
    {
      $page = atkinstance('atk.ui.atkpage');

      // make sure we register the script code with translations for the months etc. only once!
      if (!self::$s_baseScriptsRegistered)
      {
        self::$s_baseScriptsRegistered = true;

        $m_months_short   = array(1 => "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec");
        $m_months_long    = array(1 => "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december");
        $m_weekdays_long  = array("sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday");
        $m_weekdays_short = array("sun", "mon", "tue", "wed", "thu", "fri", "sat");

        foreach ($m_months_short   as &$m) {$m = atktext($m,"atk");}
        foreach ($m_months_long    as &$m) {$m = atktext($m,"atk");}
        foreach ($m_weekdays_long  as &$m) {$m = atktext($m,"atk");}
        foreach ($m_weekdays_short as &$m) {$m = atktext($m,"atk");}

        $page->register_scriptcode('
          var m_months_long    = Array("'.implode('","',$m_months_long).'");
          var m_months_short   = Array("'.implode('","',$m_months_short).'");
          var m_weekdays_long  = Array("'.implode('","',$m_weekdays_long).'");
          var m_weekdays_short = Array("'.implode('","',$m_weekdays_short).'");
        ', true);

        $page->register_script(atkconfig('atkroot').'atk/javascript/class.atkdateattribute.js');
      }

      if ($useCalendar)
      {
        $page->register_script(atkconfig("atkroot")."atk/javascript/calendar/calendar.js");
        $page->register_script(atkconfig("atkroot")."atk/javascript/calendar/calendar-runner.js");
        $page->register_script(atkconfig("atkroot")."atk/javascript/calendar/lang/calendar-".atkconfig("language").".js");

        $theme = atkinstance("atk.ui.atktheme");
        $page->register_style($theme->stylePath("atkdateattribute.css"));
      }
    }

    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="", $fieldprefix="")
    {
      $result="";
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
     * @param array $record array with 3 fields (year, month, day)
     * @return piece of HTML code
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
      if (!$extended)
      {
        // plain text search, check if we didn't come from extended search (then current value is an array)
        if (isset($record[$this->fieldName()]) && is_array($record[$this->fieldName()]))
          $record[$this->fieldName()] = NULL;

        $maxSize = $this->m_maxsize;
        $this->m_maxsize = 25; // temporary increase max size to allow from/to dates
        $result = parent::search($record, $extended, $fieldprefix);
        $this->m_maxsize = $maxSize;
        return $result;
      }

      // Set default values to null.
      if (!isset($record[$this->fieldName()]) || empty($record[$this->fieldName()]))
        $record[$this->fieldName()] = NULL;

      $rec = isset($record[$this->fieldName()]['from']) ? array($this->fieldName() => $record[$this->fieldName()]['from']) : $record;
      $res = $this->draw($rec, "atksearch_AE_".$fieldprefix, "_AE_from");
      $rec = isset($record[$this->fieldName()]['to']) ? array($this->fieldName() => $record[$this->fieldName()]['to']) : $record;
      $res.= "&nbsp;".atktext("until").": ".$this->draw($rec, "atksearch_AE_".$fieldprefix, "_AE_to");

      return $res;
    }

    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      $db = &$this->getDb();

      // If we search through datagrid we got no from/to values
      // Therefore we will simulate them
      if (!is_array($value))
      {
        if (substr_count($value, '/') == 1)
        {
          list($from, $to) = explode('/', $value);
          $value = array('from' => trim($from), 'to' => trim($to));
        }
        else if (substr_count($value, '/') == 5)
        {
          $parts = explode('/', $value);
          $value = array('from' => trim($parts[0].'/'.$parts[1].'/'.$parts[2]), 'to' => trim($parts[3].'/'.$parts[4].'/'.$parts[5]));
        }
        else
        {
          $value = array('from'=>$value, 'to'=>$value);
        }
      }

      $valueFrom = $this->fetchValue(array($this->fieldName() => $value["from"]));
      $valueTo = $this->fetchValue(array($this->fieldName() => $value["to"]));

      $fromval = $this->value2db(array($this->fieldName()=>$valueFrom));
      $toval   = $this->value2db(array($this->fieldName()=>$valueTo));

      $field = $db->func_datetochar($table.".".$this->fieldName());

      if ($fromval==NULL && $toval==NULL); // do nothing
      else if ($fromval!=NULL && $toval!=NULL)
      {
        if ($fromval>$toval)
        {
          // User entered dates in wrong order. Let's put them in the right order.
          $tmp=$fromval; $fromval=$toval; $toval=$tmp;
        }
        $searchcondition =
                   $query->betweenCondition(
                      $field,
                      $fromval,
                      $toval);
      }
      else if ($fromval!=NULL && $toval==NULL)
      {
        $searchcondition =
                   $query->greaterthanequalCondition(
                      $field,
                      $fromval);
      }
      else if ($fromval==NULL && $toval!=NULL)
      {
        $searchcondition =
                   $query->lessthanequalCondition(
                      $field,
                      $toval);
      }
      else if ((is_array($value["from"])) or (is_array($value["to"])))
      {
        $searchcondition = $this->_getDateArraySearchCondition($query,$table,$value);
      }
      else
      {
        // plain text search condition
        $value = $this->_autoCompleteDateString($value);
        $searchcondition = $query->exactCondition($field, $value);
      }

      return $searchcondition;
    }

    /**
     * Completes a date string by adding zeros to day and month
     * (if absent) and 19 or 20 in front of the year, depending
     * on the current value of the year. If the year is below 50
     * it will assume it's a year of the 21th century otherwise
     * (50 or above) it will assume it's a 20th century year.
     * @todo make this suitable for other date formats like
     *       YYYY-MM-DD
     * @todo change this code when it's approaching 2050 :-)
     * @param $value String A date in String format (like 9-12-2005)
     * @return String The auto-completed date String
     */
    function _autoCompleteDateString($value)
    {
      $elems = explode("-", $value);
      return sprintf("%02d", $elems[0])."-".
             sprintf("%02d", $elems[1])."-".
             ($elems[2]<100?($elems[2]<50?"20":"19"):"").sprintf("%02d", $elems[2]);
    }

    /**
     * Makes the search conditions if the normal conditions are
     * not met and if given date is an array,
     * for example when only the year or year-month is given
     * @param $query Query which is given in getSearchCondition
     * @param $table Table on which the condition must be executed
     * @param $value Array with values given for the search
     * @return String YYYY-MM or YYYY
     */
    function _getDateArraySearchCondition($query,$table,$value)
    {
      $db = &atkGetDb();
      $fromvalue = $this->_MakeDateForCondition($value["from"]);
      $tovalue   = $this->_MakeDateForCondition($value["to"]);

      if ($fromvalue!="")
      {
        $field = $db->func_datetochar($table.".".$this->fieldName(),$this->_SetDateFormat($value["from"]));
        $datearraysearchcondition = $query->greaterthanequalCondition($field,$fromvalue);
        // check if tovalue is set, if so add the AND
        if ($tovalue!="") $datearraysearchcondition.=" AND ";
      }
      if ($tovalue!="")
      {
        $field = $db->func_datetochar($table.".".$this->fieldName(),$this->_SetDateFormat($value["to"]));
        $datearraysearchcondition.= $query->lessthanequalCondition($field,$tovalue);
      }
      return $datearraysearchcondition;
    }

    /**
     * Checks which of the two values are filled in the array
     * and returns them
     * @param $value Array with 3 fields (year, month, day)
     * @return String YYYY-MM or YYYY
     */
    function _MakeDateForCondition($value)
    {
      if ($value["year"]!="") $fromvalue.= $value["year"];
      if ($value["year"]!="" && $value["month"]!=0) $fromvalue.= "-".sprintf("%02d",$value["month"]);
      return $fromvalue;
    }

    /**
    * Checks which of the two values are filled in the array
    * and returns the DATE_FORMAT for the database
    * @param $value Array with 3 fields (year, month, day)
    * @return String DATE_FORMAT
    */
    function _SetDateFormat($value)
    {
      if ($value["year"]!="") $format = 'Y';
      if ($value["year"]!="" && $value["month"]!=0) $format = 'Y-m';
      return $format;
    }

    /**
     * Convert date array to database value
     * @param $rec array with 3 fields (year, month, day)
     * @return database value for date
     */
    function value2db($rec)
    {
      if (!is_array($rec[$this->fieldName()]))
        $rec[$this->fieldName()] = $this->dateArray($rec[$this->fieldName()]);

      $year = $rec[$this->fieldName()]["year"];
      $month = $rec[$this->fieldName()]["month"];
      $day = $rec[$this->fieldName()]["day"];

      if (empty($year) || empty($month) || empty($day)) return NULL;
      if ($year=='' || $month==0 || $day==0) return NULL; //one of the fields is left empty

      $result = $year."-".sprintf("%02d",$month)."-".sprintf("%02d",$day);
      return $result;
    }

    /**
     * Convert database value to date array
     * @param $rec database record with date field
     * @return array with 3 fields (year, month, day)
     */
    function db2value($rec)
    {
      if (!isset($rec[$this->fieldName()]) || strlen($rec[$this->fieldName()]) == 0 || (int)substr($rec[$this->fieldName()], 0, 4) == 0) return "";
      return array("year" =>substr($rec[$this->fieldName()], 0, 4),
                   "month"=>substr($rec[$this->fieldName()], 5, 2),
                   "day"  =>substr($rec[$this->fieldName()], 8, 2));
    }

    /**
     * Return the HTTP post values for this attribute
     * @param $postvars the HTTP post vars
     * @return array with 3 fields (year, month, day)
     */
    function fetchValue($postvars)
    {
      if (!is_array($postvars) || !isset($postvars[$this->fieldName()])) return NULL;

      $value = $postvars[$this->fieldName()];


      // edit in text mode
      if (is_array($value) && array_key_exists('date', $value))
      {
        $value = $postvars[$this->fieldName()]['date'];
      }

      // null
      if (is_array($value) && $this->value2db($postvars) == null)
      {
        return null;
      }

      // array with year / month / day
      else if (is_array($value))
      {
        return $value;
      }

      // text format
      else if (!empty($value))
      {
        // maybe we should use strptime in PHP >= 5.1
        $formats = array();
        $formats[] = str_replace(array("y", "Y", "m", "n", "j", "d"), array("yyyy", "yyyy", "mm", "mm", "dd", "dd"), $this->m_date_format_edit);
        $formats[] = str_replace(array("y", "Y", "m", "n", "j", "d"), array("yyyy", "yyyy", "m",  "m",  "dd", "dd"), $this->m_date_format_edit);
        $formats[] = str_replace(array("y", "Y", "m", "n", "j", "d"), array("yyyy", "yyyy", "mm", "mm", "d",  "d"),  $this->m_date_format_edit);
        $formats[] = str_replace(array("y", "Y", "m", "n", "j", "d"), array("yyyy", "yyyy", "m",  "m",  "d",  "d"),  $this->m_date_format_edit);
        $formats[] = str_replace(array("y", "Y", "m", "n", "j", "d"), array("yy",   "yy",   "mm", "mm", "dd", "dd"), $this->m_date_format_edit);
        $formats[] = str_replace(array("y", "Y", "m", "n", "j", "d"), array("yy",   "yy",   "m",  "m",  "dd", "dd"), $this->m_date_format_edit);
        $formats[] = str_replace(array("y", "Y", "m", "n", "j", "d"), array("yy",   "yy",   "mm", "mm", "d",  "d"),  $this->m_date_format_edit);
        $formats[] = str_replace(array("y", "Y", "m", "n", "j", "d"), array("yy",   "yy",   "m",  "m",  "d",  "d"),  $this->m_date_format_edit);

        $arr = $this->parseDate($value, $formats);
        if ($arr['day'] == 0 || $arr['month'] == 0 || $arr['year'] == 0)
          return $this->dateArray(adodb_date("Ymd", strtotime($value)));
        else return $arr;
      }

      return NULL;
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
      $value = &$record[$this->fieldName()];

      /* array or no array */
      if (!is_array($value)) $value = $this->dateArray(adodb_date("Ymd", strtotime($value)));

      /* if not obligatory and one of the fields is null then the date will be saved as null */
      if (!$this->hasFlag(AF_OBLIGATORY) && (empty($value["year"]) || empty($value["month"]) || empty($value["day"]))) return;

      // If one of the fields is not filled, we don't check
      if(!($value["year"]=='' || $value['month']==0 || $value['day']==0))
      {
        /* currently selected date */
        if ($this->checkDateArray($value))
          $current = adodb_mktime(0, 0, 0, $value["month"], $value["day"], $value["year"]);
        else
        {
          triggerError($record, $this->fieldName(), 'error_date_invalid');
          return;
        }
      }

      /* allright, if not obligatory, and we have come all this way, we'll bail out */
      if (!$this->hasFlag(AF_OBLIGATORY)) return;
      else
      {
        if($value["year"]=='' || $value['month']==0 || $value['day']==0)
        {
          triggerError($record, $this->fieldName(), 'error_obligatoryfield');
          return;
        }
      }

      /* minimum date */
      $minimum = 0;
      $str_min = $this->m_date_min;
      if (strlen($str_min) == 8)
      {
        $date = $this->dateArray($str_min);
        if ($this->checkDateArray($date))
          $minimum = adodb_mktime(0, 0, 0, $date["month"], $date["day"], $date["year"]);
        else $str_min = 0;
      }

      /* maximum date */
      $maximum = 0;
      $str_max = $this->m_date_max;
      if (strlen($str_max) == 8)
      {
        $date = $this->dateArray($str_max);
        if ($this->checkDateArray($date))
          $maximum = adodb_mktime(0, 0, 0, $date["month"], $date["day"], $date["year"]);
        else $str_max = 0;
      }

      /* date < minimum */
      if (!empty($minimum) && $current < $minimum)
      {
        triggerError($record, $this->fieldName(), 'error_date_minimum', atktext("error_date_minimum")." ". $this->formatDate(adodb_getdate($minimum), $this->m_date_format_view, 0));
        return;
      }

      /* date > maximum */
      if (!empty($maximum) && $current > $maximum)
        triggerError($record, $this->fieldName(), 'error_date_maximum', atktext("error_date_maximum")." ". $this->formatDate(adodb_getdate($maximum), $this->m_date_format_view, 0));
    }

    /**
     * Function display's the date
     * @param $record array with date
     * @return formatted date string
     */
    function display($record)
    {
      $value = $record[$this->fieldName()];
      if (!is_array($value) || empty($value["month"]) || empty($value["day"]) || empty( $value["year"])) return "";
      $tmp_date = adodb_getdate(adodb_mktime(0, 0, 0, $value["month"], $value["day"], $value["year"]));
      if (!empty($tmp_date))
      {
        if ($this->hasFlag(AF_DATE_DISPLAY_DAY)) return $this->formatDate($tmp_date, $this->m_date_format_view, 1);
        else return $this->formatDate($tmp_date, $this->m_date_format_view, 0);
      }
      else return "&nbsp;";
    }

    function getSearchModes()
    {
      return array("between");
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
        if($this->value2db($rec) == NULL)
        {
          $query->addField($this->fieldName(),'NULL','','',false);
        }
        else
        {
          $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
        }
      }
      else
      {
        $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
      }
    }

    function dbFieldType()
    {
      return "date";
    }

    /**
     * Add a javascript onchange event handler.
     * @param $jscode A block of valid javascript code.
     */
    function addOnChangeHandler($jscode)
    {
      $this->m_onchangecode[] = $jscode;
    }

    function renderChangeHandler($fieldprefix)
    {
      if (count($this->m_onchangecode))
      {
        $page = &$this->m_ownerInstance->getPage();
        $page->register_scriptcode("function ".$this->getHtmlId($fieldprefix)."_onChange(el)
                                    {
                                      ".implode("\n", $this->m_onchangecode)."
                                    }\n");
      }
    }

    /**
     * Convert a String representation into an internal value.
     *
     * This implementation converts datestring to a array with day, month and
     * year separated
     *
     * @param String $stringvalue The value to parse.
     * @return Internal value for a date
     */
    function parseStringValue($stringvalue)
    {
      $formats = array("dd-mm-yyyy","dd-mm-yy","d-mm-yyyy","dd-m-yyyy","d-m-yyyy","yyyy-mm-dd","yyyy-mm-d","yyyy-m-dd","yyyy-m-d");

      return $this->parseDate($stringvalue,$formats);
    }

    function parseDate($stringvalue, $formats){
      //looking in witch format the stringvalue match and then get the data
      foreach($formats as $format){
        //make vars to know te position of the d,m and y symbols
        $dayBegin = strpos($format,'d');
        $dayLength = 0; while(substr($format,$dayBegin+$dayLength,1) == 'd') $dayLength++;

        $monthBegin = strpos($format,'m');
        $monthLength = 0; while(substr($format,$monthBegin+$monthLength,1) == 'm') $monthLength++;

        $yearBegin = strpos($format,'y');
        $yearLength = 0; while(substr($format,$yearBegin+$yearLength,1) == 'y') $yearLength++;

        //analyze the formate and make a regular expression
        $replaces = array();
        $replaces[$dayBegin] = array("[0-9]{".$dayLength."}",$dayLength);
        $replaces[$monthBegin] = array("[0-9]{".$monthLength."}",$monthLength);
        $replaces[$yearBegin] = array("[0-9]{".$yearLength."}",$yearLength);

        ksort($replaces);

        $regexpr = str_replace("-"," ",$format);
        $marge = 0; //this is the marge that the new string greater is than the old one
        foreach($replaces as $begin=>$replace){
          $newpart = $replace[0];
          $length = $replace[1];
          $newbegin = $begin + $marge;

          $regexpr = substr($regexpr,0,$newbegin).$newpart.substr($regexpr,$newbegin+$length);

          $marge = strlen($regexpr) - strlen($format);
        }

        $regexpr = "^$regexpr$";

        $valueSeparators = array ("-","/","\.","\\\\","a");

        //if the value has the format given by regexpr.
        //also try to replace - by "/","." or "\""
        foreach($valueSeparators as $valueSeparator)
        {
          $expr = str_replace(" ",$valueSeparator,$regexpr);
          if(ereg($expr,$stringvalue))
          {
            $day    = substr($stringvalue,$dayBegin,$dayLength);
            $month  = substr($stringvalue,$monthBegin,$monthLength);
            $year   = substr($stringvalue,$yearBegin,$yearLength);

            if($month > 12 && $day <= 12)
            {
              $month += $day;
              $day = $month-$day;
              $month -= $day;
            }
            return array('day'=>$day,'month'=>$month,'year'=>$year);
          }
        }
      }

      return array('day'=>0,'month'=>0,'year'=>0);
    }

    /**
     * Setter for max years, this specifies the maximum amount of years in the dropdown
     * if the amount is more than specified in the max years the years field is shown
     * as a normal textbox instead of a dropdown.
     * @param int $maxyears The maximum amount of years for the years dropdown
     * @return bool Wether or not we succeed in setting the variable
     */
    function setMaxYears($maxyears)
    {
      if (is_numeric($maxyears))
      {
        $this->m_maxyears = (int)$maxyears;
      }
      else
      {
        return false;
      }
      return true;
    }

    /**
     * Getter for max years, this specifies the maximum amount of years in the dropdown
     * if the amount is more than specified in the max years the years field is shown
     * as a normal textbox instead of a dropdown.
     * @return int The maximum years for the dropdown
     */
    function getMaxYears()
    {
      return $this->m_maxyears;
    }
  }
?>
