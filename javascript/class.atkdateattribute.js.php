<?php
  chdir("../../");
  include "atk/defaultconfig.inc.php";
  include "config.inc.php";
  include "atk//atktools.inc";
  include "atk/languages/".$config_languagefile;

  $m_options_short = Array(1 => "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec");
  $m_options_long  = Array(1 => "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december");
?>
function GetDays(month, year)
{
  month_arr = Array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
  if((year % 4 == 0) && (!(year % 100 == 0) || (year % 400 == 0))) month_arr[1] = 29;
  return month_arr[month-1];
}

function DateArray(date)
{
  if(!date) date = new String("00000000");
  else date = new String(date);
  d = parseInt(date.substr(6, 2), 10);
  m = parseInt(date.substr(4, 2), 10);
  y = parseInt(date.substr(0, 4), 10);
  if(d > GetDays(m, y)) d = GetDays(m, y);
  return Array(d, m, y);
}

function AdjustDate(frm, arr, format, min, max)
{
  set_month = Array(<?php for ($i = 1; $i <= 12; $i++) echo "'".text($m_options_long[$i])."'".($i < 12 ? "," : ""); ?>);
  set_day = 0;

  for (i = 0; i < format.length; i++)
  {
    switch(format.substr(i, 1))
    {
      case "F":
        set_month = Array(<?php for ($i = 1; $i <= 12; $i++) echo "'".text($m_options_long[$i])."'".($i < 12 ? "," : ""); ?>);
        break;

      case "M":
        set_month = Array(<?php for ($i = 1; $i <= 12; $i++) echo "'".text($m_options_short[$i])."'".($i < 12 ? "," : ""); ?>);
        break;

      case "m":
        set_month = Array(<?php for ($i = 1; $i <= 12; $i++) echo "'".sprintf("%02d",$i)."'".($i < 12 ? "," : ""); ?>);
        break;

      case "n":
        set_month = Array(<?php for ($i = 1; $i <= 12; $i++) echo $i.($i < 12 ? "," : ""); ?>);
        break;

      case "d":
        set_day = 1;
        break;
    }
  }

  day   = frm.elements[arr + "[day]"];
  month = frm.elements[arr + "[month]"];
  year  = frm.elements[arr + "[year]"];

  day_v   = parseInt(day.options[day.selectedIndex].text, 10);
  month_v = parseInt(month.options[month.selectedIndex].value, 10);
  year_v  = parseInt(year.type == "select-one" ? year.options[year.selectedIndex].value : year.value, 10);    

  if(day != null && month != null && year != null && year_v > 1000)
  {
    date_select = new String(year_v) + (month_v < 10 ? "0" : "") + new String(month_v) + (day_v < 10 ? "0" : "") + new String(day_v);        

    if(min || max)
    {
      if ((min && max && date_select < min) || (min && date_select < min)) date_new = new String(min);
      else if ((min && max && date_select > max) || (max && date_select > max)) date_new = new String(max);
      else date_new = date_select;
    }
    else date_new = date_select;

    date_new_arr = DateArray(date_new);
    date_min_arr = DateArray(min);
    date_max_arr = DateArray(max);

    // day
    if(date_new_arr[1] == date_min_arr[1] && date_new_arr[1] == date_max_arr[1] &&
      date_new_arr[2] == date_min_arr[2] && date_min_arr[0] > 1 &&
      date_max_arr[0] < GetDays(date_new_arr[1], date_new_arr[2]))
      day_i = Array(date_min_arr[0], date_max_arr[0]);
    else if(date_new_arr[1] == date_min_arr[1] && date_new_arr[2] == date_min_arr[2] &&
      date_min_arr[0] > 1) day_i = Array(date_min_arr[0], GetDays(date_new_arr[1], date_new_arr[2]));
    else if(date_new_arr[1] == date_max_arr[1] && date_new_arr[2] == date_max_arr[2] &&
      date_max_arr[0] < GetDays(date_new_arr[1], date_new_arr[2])) day_i = Array(1, date_max_arr[0]);
    else day_i = Array(1, GetDays(date_new_arr[1], date_new_arr[2]));

    for(i = day.options.length; i >= 0; i--) day.options[i] = null;    
    for(i = day_i[0]; i <= day_i[1]; i++) 
    {      
      var dayname = new String(i);
      if (i<10) 
      {
       // alert("k");
        dayname = "0"+dayname;
      }      
      day.options[i-day_i[0]] = new Option(dayname, i);
      //day.options[i-day_i[0]] = new Option(i,i);
    }

    for(i = 0; i < day.options.length; i++)
    {      
      if(parseInt(day.options[i].text,10) == date_new_arr[0]) day.options[i].selected = true;
    }

    // month
    if(date_new_arr[2] == date_min_arr[2] && date_new_arr[2] == date_max_arr[2]) month_i = Array(date_min_arr[1], date_max_arr[1]);
    else if(date_new_arr[2] == date_min_arr[2]) month_i = Array(date_min_arr[1], 12);
    else if(date_new_arr[2] == date_max_arr[2]) month_i = Array(1, date_max_arr[1]);
    else month_i = Array(1, 12);

    for(i = month.options.length; i >= 0; i--) month.options[i] = null;
    for(i = month_i[0]; i <= month_i[1]; i++) month.options[i-month_i[0]] = new Option(set_month[i-1], i);

    for(i = 0; i < month.options.length; i++)
      if(parseInt(month.options[i].value,10) == date_new_arr[1]) month.options[i].selected = true;

    // year
    if(year.type == "select-one")
    {
      for(i = 0; i < year.options.length; i++)
        if(parseInt(year.options[i].value,10) == date_new_arr[2]) year.options[i].selected = true;
    }
    else year.value = date_new_arr[2];
        
  }
}
