<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage ui
 *
 * @copyright (c)2004 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 4682 $
 * $Id$
 */

/**
 * Function for embedding a date control in html.
 * 
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
function smarty_function_atkdatefield($params, &$smarty)
{
  $name = isset($params['name']) ? $params['name'] : 'date';
  $format = isset($params['format']) ? $params['format'] : atktext("date_format_edit", "atk", "", "", "", true);
  $mandatory = isset($params['mandatory']) && $params['mandatory'] || isset($params['obligatory']) && $params['obligatory'];
  $noweekday = isset($params['noweekday']) && $params['noweekday'];
  $calendar = isset($params['calendar']) && $params['calendar'];
  $time = isset($params['time']) ? $params['time'] : ($mandatory ? mktime() : NULL);
  $min = isset($params['min']) ? $params['min'] : 0;
  $max = isset($params['max']) ? $params['max'] : 0;
  
  if (is_array($time))
  {
    $date = $time;
  }
  else if ($time == NULL)
  {
    $date = NULL;
  }
  else if (is_numeric($time))
  {
    $date = getdate($time);
    $date = array('day' => $date['mday'], 'month' => $date['mon'], 'year' => $date['year']);
  }
  else if (preg_match('/([0-9]{1,2})-([0-9]{1,2})-([0-9]{4})/', $time, $matches))
  {
    $date = array('day' => (int)$matches[1], 'month' => (int)$matches[2], 'year' => $matches[3]);    
  }
  else 
  {
    $date = getdate(strtotime($time));
    $date = array('day' => $date['mday'], 'month' => $date['mon'], 'year' => $date['year']);    
  }
  
  useattrib('atkdateattribute');
  $attr = new atkDateAttribute($name, $format, '', $min, $max, ($noweekday ? AF_DATE_EDIT_NO_DAY : 0)|($mandatory ? AF_OBLIGATORY : 0)|($calendar ? 0 : AF_DATE_NO_CALENDAR));
  $html = $attr->edit(array($name => $date));
  return $html;
}
?>