<?php
  /**
   * Advanced javascript for the dateattribute which tries
   * to limit the user's input to only valid date entries.
   *
   * @author Peter Verhage <peter@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$
   * $Log$
   * Revision 4.2  2001/04/24 18:23:35  peter
   * Complete rewrite of the dateattribute javascript. This version fixes
   * a bug with the day-of-week added in the previous commit. The file has
   * been totally rewritten to be more clear and understandable so other
   * people can also fix bugs in this script. ;)
   *
   */
  
  /* change dir for includes */
  chdir("../../");
  include "atk/defaultconfig.inc.php";
  include "config.inc.php";
  include "atk/atktools.inc";
  include "atk/languages/".$config_languagefile;

  /* english month names and weekdays */
  $m_months_short = Array(1 => "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec");
  $m_months_long  = Array(1 => "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december");
  $m_weekdays     = Array("sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday");
?>

/* javascript month names and weekdays */
var m_months_long  = Array(<?php for ($i = 1; $i <= 12; $i++) echo "'".text($m_months_long[$i])."'".($i < 12 ? "," : ""); ?>);
var m_months_short = Array(<?php for ($i = 1; $i <= 12; $i++) echo "'".text($m_months_short[$i])."'".($i < 12 ? "," : ""); ?>);
var m_weekdays     = Array(<?php for ($i = 0; $i <= 6; $i++) echo "'".text($m_weekdays[$i])."'".($i < 6 ? "," : ""); ?>);

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
 * Checks/changes the date input boxes for a certain date field on the form.
 * @frm pointer to the form which contains the date input boxes
 * @arr name of the input boxes (without [day] etc.)
 * @format a valid date format string (like in PHP)
 * @str_min the minimum valid date
 * @str_max the maximum valid date
 */
function AdjustDate(frm, arr, format, str_min, str_max)
{
  var format_month, format_day, array_months;

  /* check month format */
  if      (format.indexOf("F") >= 0) array_months = m_months_long;
  else if (format.indexOf("M") >= 0) array_months = m_months_short;
  else if (format.indexOf("m") >= 0) format_month = "m";
  else                               format_month = "n";  
  
  /* check day format */
  if (format.indexOf("d")) format_day = "d";
  else format_day = "j";

  /* current date attribute inputs */
  input = Array();
  input["d"] = frm.elements[arr + "[day]"];
  input["m"] = frm.elements[arr + "[month]"];
  input["y"] = frm.elements[arr + "[year]"];

  /* check if valid date attribute inputs */
  if (input["d"] == null || input["m"] == null || input["y"] == null) return;
  
  /* current selected data */
  current = Array();
  current["d"] = parseInt(input["d"].options[input["d"].selectedIndex].value, 10);
  current["m"] = parseInt(input["m"].options[input["m"].selectedIndex].value, 10);  
  current["y"] = parseInt(input["y"].type == "select-one" ? input["y"].options[input["y"].selectedIndex].value : input["y"].value, 10);    

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
  date_current = new Date(current["y"], current["m"]-1, current["d"]);
  date_minimum = new Date(minimum["y"], minimum["m"]-1, minimum["d"]);
  date_maximum = new Date(maximum["y"], maximum["m"]-1, maximum["d"]);  

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
  current["d"] = date_current.getDate();
  current["m"] = date_current.getMonth() + 1;  
  current["y"] = date_current.getFullYear();

  /* minimum and maximum */
  current["d_min"] = (date_minimum != null && date_current.getFullYear() == date_minimum.getFullYear() &&
                      date_current.getMonth() == date_minimum.getMonth() ? date_minimum.getDate() : 1);
  current["d_max"] = (date_maximum != null && date_current.getFullYear() == date_maximum.getFullYear() &&
                      date_current.getMonth() == date_maximum.getMonth() ? date_maximum.getDate() : getDays(date_current));
  current["m_min"] = (date_minimum != null && date_current.getFullYear() == date_minimum.getFullYear() ? date_minimum.getMonth() + 1 : 1);
  current["m_max"] = (date_maximum != null && date_current.getFullYear() == date_maximum.getFullYear() ? date_maximum.getMonth() + 1 : 12);  
  current["y_min"] = (date_minimum != null ? date_minimum.getFullYear() : 0);    
  current["y_max"] = (date_maximum != null ? date_maximum.getFullYear() : 0);

  /* clean day input, and build new one */
  for(i = input["d"].options.length; i >= 0; i--) input["d"].options[i] = null;
  for(i = current["d_min"]; i <= current["d_max"]; i++) 
  {
    date_tmp = new Date(current["y"], current["m"], i);
    str_day  = m_weekdays[date_tmp.getDay()] + " ";    
    if ("d" == format_day) str_day += (i < 10 ? "0" : "") + i;
    else str_day += i;
    input["d"].options[i-current["d_min"]] = new Option(str_day, i);    
    if (i == current["d"]) input["d"].options[i-current["d_min"]].selected = true;
  }
  
  /* clean month input, and build new one */
  for(i = input["m"].options.length; i >= 0; i--) input["m"].options[i] = null;
  for(i = current["m_min"]; i <= current["m_max"]; i++)
  {
    if ("m" == format_month) str_month = (i < 10 ? "0" : "") + i;
    else if ("n" == format_month) str_month = i;
    else str_month = array_months[i-1];
    input["m"].options[i-current["m_min"]] = new Option(str_month, i);
    if (i == current["m"]) input["m"].options[i-current["m_min"]].selected = true;    
  }
  
  /* clean year input, and build new one */
  if(input["y"].type == "select-one")
  {
    for(i = input["y"].options.length; i >= 0; i--) input["y"].options[i] = null;
    for(i = current["y_min"]; i <= current["y_max"]; i++)
    {
      input["y"].options[i-current["y_min"]] = new Option(i, i);
      if (i == current["y"]) input["y"].options[i-current["y_min"]].selected = true;    
    }
  }
  else input["y"].value = current["y"];
}