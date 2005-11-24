<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage javascript
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Advanced javascript for the dateattribute which tries
   * to limit the user's input to only valid date entries.
   *
   * Note: after the rewrite a few months ago, some new behaviour
   * was created without really knowing it. The old javascript code changed
   * the day to the last day in the month if you choose a date in a valid
   * month, in a valid year, with an invalid day. The new code circulates
   * to the next month if you choose a day that's out of reach for the
   * selected month. E.g. old attribute; 30-02-2001 -> 28-02-2001, new
   * attribute; 30-02-2001 -> 02-03-2001! This is not a bug, just different
   * behaviour!
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$
   *
   */

  /* change dir for includes */
  include_once($config_atkroot."atk/class.atkconfig.inc");
  include_once($config_atkroot."atk/defaultconfig.inc.php");
  include_once($config_atkroot."config.inc.php");
  include_once($config_atkroot."atk/atktools.inc");

  /* english month names and weekdays */
  $m_months_short = Array(1 => "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec");
  $m_months_long  = Array(1 => "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december");
  $m_weekdays     = Array("sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday");
?>

/* javascript month names and weekdays */
var m_months_long  = Array(<?php for ($i = 1; $i <= 12; $i++) echo "'".atktext($m_months_long[$i],"atk")."'".($i < 12 ? "," : ""); ?>);
var m_months_short = Array(<?php for ($i = 1; $i <= 12; $i++) echo "'".atktext($m_months_short[$i],"atk")."'".($i < 12 ? "," : ""); ?>);
var m_weekdays     = Array(<?php for ($i = 0; $i <= 6; $i++) echo "'".atktext($m_weekdays[$i],"atk")."'".($i < 6 ? "," : ""); ?>);

/**
 * Returns the number of days in the month/year of the supplied date object
 * @param date a valid javascript date object
 * @return number of days in month/year combination
 */
function getDays(date)
{
  if (date == null) return -1;
  array_month = Array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
  if ((date.getFullYear() % 4 == 0) && (!(date.getFullYear() % 100 == 0) || (date.getFullYear() % 400 == 0))) array_month[1] = 29;
  return array_month[date.getMonth()];
}

/**
 * Wrapper function for ReadAdjustDate, since it's crashing in some IE 6.0 browsers
 */
function AdjustDate(el, arr, format, str_min, str_max, emptyfield)
{
  setTimeout(function() { RealAdjustDate(el, arr, format, str_min, str_max, emptyfield) }, 1);
}

/**
 * Checks/changes the date input boxes for a certain date field on the form.
 * @param el pointer to the form element which initiated the call to this method
 * @param arr name of the input boxes (without [day] etc.)
 * @param format a valid date format string (like in PHP)
 * @param str_min the minimum valid date
 * @param str_max the maximum valid date
 * @param obligatory is the date field obligatory
 */
function RealAdjustDate(el, arr, format, str_min, str_max, emptyfield)
{
  var format_month, format_day, array_months;
  var frm = el.form;


  /* current date attribute inputs */
  input = Array();
  input["d"] = frm.elements[arr + "[day]"];
  input["m"] = frm.elements[arr + "[month]"];
  input["y"] = frm.elements[arr + "[year]"];

    /* check if valid date attribute inputs */
  if (input["d"] == null || input["m"] == null || input["y"] == null) return;

  /* currently selected date */
  current = Array();
  current["d"] = parseInt(input["d"].options[input["d"].selectedIndex].value, 10);
  current["m"] = parseInt(input["m"].options[input["m"].selectedIndex].value, 10);
  current["y"] = parseInt(input["y"].type == "select-one" ? input["y"].options[input["y"].selectedIndex].value : input["y"].value, 10);
  if (current["y"].toString() == "NaN") current["y"] = 0;

  /* check month format */
  if      (format.indexOf("F") >= 0) array_months = m_months_long;
  else if (format.indexOf("M") >= 0) array_months = m_months_short;
  else if (format.indexOf("m") >= 0) format_month = "m";
  else                               format_month = "n";

  /* check day format */
  if (format.indexOf("d") >= 0) format_day = "d";
  else format_day = "j";



  /* we just changed one of the fields to null */
  if (emptyfield && ((el.type == "select-one" && el.selectedIndex == 0) || (el.type != "select-one" && el.value == "")))
  {
    for (i = input["d"].options.length; i >= 0; i--) input["d"].options[i] = null;
    input["d"].options[0] = new Option("", 0);
    for (i = 1; i <= 31; i++) input["d"].options[input["d"].options.length] = new Option(("d" == format_day) ? (i < 10 ? "0" : "") + i : i, i);
    input["m"].options[0].selected = true;
    input["m"].selectedIndex = 0;
    input["y"].value = "";
    return;
  }

  /* we just changed one of the fields from null to something */
  else if (!emptyfield && (current["d"] == 0 || current["y"] == 0 || current["m"] == 0))
  {
    today = new Date();
    current["d"] = today.getDate();
    current["m"] = today.getMonth() + 1;
    current["y"] = today.getFullYear();
  }

  /* minimum date */
  minimum = Array();
  str_min = new String(str_min);
  if (str_min.length == 8)
  {
    minimum["d"] = parseInt(str_min.substr(6, 2), 10);
    minimum["m"] = parseInt(str_min.substr(4, 2), 10);
    minimum["y"] = parseInt(str_min.substr(0, 4), 10);
  }

  /* maximum date */
  maximum = Array();
  str_max = new String(str_max);
  if (str_max.length == 8)
  {
    maximum["d"] = parseInt(str_max.substr(6, 2), 10);
    maximum["m"] = parseInt(str_max.substr(4, 2), 10);
    maximum["y"] = parseInt(str_max.substr(0, 4), 10);
  }

  /* convert to real dates */
  date_now     = new Date();
  date_minimum = new Date(minimum["y"], minimum["m"]-1, minimum["d"]);
  date_maximum = new Date(maximum["y"], maximum["m"]-1, maximum["d"]);


  if (current["d"] != 0 && current["y"] != 0 && current["m"] != 0)
  {
    date_current = new Date(current["y"], current["m"]-1, current["d"]);

    /* check dates */
    if (date_current.getDate().toString() == "NaN") date_current = null;
    if (date_minimum.getDate().toString() == "NaN") date_minimum = null;
    if (date_maximum.getDate().toString() == "NaN") date_maximum = null;

    /* did we select a valid date? */
    if      (date_current != null && date_minimum != null && date_current < date_minimum) date_current = date_minimum;
    else if (date_current != null && date_maximum != null && date_current > date_maximum) date_current = date_maximum;
    else if (date_current == null && date_minimum != null && date_now < date_minimum) date_current = date_minimum;
    else if (date_current == null && date_maximum != null && date_now > date_maximum) date_current = date_maximum;
    else if (date_current == null) date_current = date_now;

    /* put current date back into array */
    if (current["d"] != 0) current["d"] = date_current.getDate();
    if (current["m"] != 0) current["m"] = date_current.getMonth() + 1;
    if ((current["y"] != 0) || !emptyfield) current["y"] = date_current.getFullYear();
    else current["y"] = "";

    /* minimum and maximum */
    current["d_min"] = (date_minimum != null && date_current.getFullYear() == date_minimum.getFullYear() &&
                        date_current.getMonth() == date_minimum.getMonth() ? date_minimum.getDate() : 1);
    current["d_max"] = (date_maximum != null && date_current.getFullYear() == date_maximum.getFullYear() &&
                        date_current.getMonth() == date_maximum.getMonth() ? date_maximum.getDate() : getDays(date_current));
    current["m_min"] = (date_minimum != null && date_current.getFullYear() == date_minimum.getFullYear() ? date_minimum.getMonth() + 1 : 1);
    current["m_max"] = (date_maximum != null && date_current.getFullYear() == date_maximum.getFullYear() ? date_maximum.getMonth() + 1 : 12);
    current["y_min"] = (date_minimum != null ? date_minimum.getFullYear() : 0);
    current["y_max"] = (date_maximum != null ? date_maximum.getFullYear() : 0);

  }
  else
  {
    if (!emptyfield)
    {
      if (current["y"] == "" && current["d"] != 0 && current["m"] != 0)
      {
        date_current = new Date(date_now["y"], date_now["m"]-1, date_now["d"]);
      }
    }
    else
    {
      current["d_min"] = 1;
      current["d_max"] = 31;
      current["m_min"] = 1;
      current["m_max"] = 12;
      if(input["y"].type == "select-one")
      {
        current["y_min"] = (date_minimum != null ? date_minimum.getFullYear() : 0);
        current["y_max"] = (date_maximum != null ? date_maximum.getFullYear() : 0);
      }
    }
  }

  if (current["y"] == 0) current["y"] = "";


//alert(current["d"] + " " + current["y"] + " " + current["m"] + " " + current["d_min"] + " " + current["d_max"]);
  /* clean day input, and build new one */
  for(i = input["d"].options.length; i >= 0; i--) input["d"].options[i] = null;
  if (emptyfield) input["d"].options[0] = new Option("", 0);
  //if (current["y"] == "" || current["m"] == 0)  alert('heee');
  for(i = current["d_min"]; i <= current["d_max"]; i++)
  {
    if (current["y"] == "" || current["m"] == 0)
    {
      value = (("d" == format_day) ? (i < 10 ? "0" : "") + i : i);
    }
    else
    {
      date = new Date(current["y"], current["m"]-1, i);
      value = m_weekdays[date.getDay()] + " " + (("d" == format_day) ? (i < 10 ? "0" : "") + i : i);
    }
    input["d"].options[input["d"].options.length] = new Option(value, i);
    if (i == current["d"])
    {
      input["d"].options[input["d"].options.length-1].selected = true;
      input["d"].options.selectedIndex = input["d"].options.length-1;
    }
  }

  /* clean month input, and build new one */
  for(i = input["m"].options.length; i >= 0; i--) input["m"].options[i] = null;
  if (emptyfield) input["m"].options[0] = new Option("", 0);
  for(i = current["m_min"]; i <= current["m_max"]; i++)
  {
    value = ("m" == format_month) ? (i < 10 ? "0" : "") + i : ("n" == format_month) ? i : array_months[i-1];
    input["m"].options[input["m"].options.length] = new Option(value, i);
    if (i == current["m"])
    {
      input["m"].options[input["m"].options.length-1].selected = true;
      input["m"].options.selectedIndex = input["m"].options.length-1;
    }
  }

  /* clean year input, and build new one */
  if(input["y"].type == "select-one")
  {
    for(i = input["y"].options.length; i >= 0; i--) input["y"].options[i] = null;
    if (emptyfield) input["y"].options[0] = new Option("", 0);
    for(i = current["y_min"]; i <= current["y_max"]; i++)
    {
      input["y"].options[input["y"].options.length] = new Option(i, i);
      if (i == current["y"])
      {
        input["y"].options[input["y"].options.length-1].selected = true;
        input["y"].options.selectedIndex = input["y"].options.length-1;
      }
    }
  }
  else input["y"].value = current["y"];
}

function dateAttribute_getValue(id)
{
  var dayel = document.getElementById(id+"[day]");
  var monthel = document.getElementById(id+"[month]");
  var yearel = document.getElementById(id+"[year]");
  var result = yearel.value+'-'+monthel.options[monthel.selectedIndex].value+'-'+dayel.options[dayel.selectedIndex].value;
  return result;
}

function dateAttribute_setValue(id, year, month, day)
{

  month = parseInt(month, 10); // remove leading '0'
  day = parseInt(day, 10);

  format = eval(id+'_fmt');

  var dayel = document.getElementById(id+"[day]");
  var monthel = document.getElementById(id+"[month]");
  var yearel = document.getElementById(id+"[year]");

  // set year
  yearel.value = year;

  // I need to call this to calculate the correct months and days
  // after setting the year.
  // TODO FIXME: I don't have the arr ('enddate') and format parameters here!!
  AdjustDate(yearel, id, format, 0, 0, false);

  // set month
  for (i=0; i<monthel.options.length;  i++)
  {
    if (monthel.options[i].value==month)
    {
      monthel.selectedIndex = i;
    }
  }

  // I need to call this to calculate the correct days
  // after setting the month.
  // TODO FIXME: I don't have the arr ('enddate') and format parameters here!!
  AdjustDate(monthel, id, format, 0, 0, false);

  // set day
  for (i=0; i<dayel.options.length;  i++)
  {
    if (dayel.options[i].value==day)
    {
      dayel.selectedIndex = i;
    }
  }
}